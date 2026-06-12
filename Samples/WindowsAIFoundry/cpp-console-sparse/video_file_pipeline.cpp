// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
//
// Video Super Resolution over a real video file (--videofile <in> [<out>]).
//
// VideoScaler's tested/working path is Direct3D surfaces (NV12 in, BGRA8 out) --
// see the Windows AI VSR docs and the AI Dev Gallery sample. This file wires up
// the minimal real pipeline around that:
//
//   Media Foundation decode (NV12, D3D-backed)  ->  VideoScaler.Scale  ->
//   read back BGRA  ->  Media Foundation encode (H.264 MP4)
//
// It runs entirely on the device's NPU/GPU model loaded by VideoScaler.

#include <Windows.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <mferror.h>
#include <d3d11.h>
#include <d3d11_4.h>
#include <dxgi.h>
#include <windows.graphics.directx.direct3d11.interop.h>

#include <wil/cppwinrt.h>
#include <wil/resource.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Graphics.DirectX.Direct3D11.h>
#include <winrt/Microsoft.Windows.AI.h>
#include <winrt/Microsoft.Windows.AI.MachineLearning.h>
#include <winrt/Microsoft.Windows.AI.Video.h>

#include <string>
#include <iostream>
#include <format>
#include <print>

namespace
{
    using winrt::com_ptr;
    using winrt::check_hresult;
    using winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DSurface;
    using winrt::Microsoft::Windows::AI::AIFeatureReadyResultState;
    using winrt::Microsoft::Windows::AI::AIFeatureReadyState;
    using winrt::Microsoft::Windows::AI::MachineLearning::ExecutionProviderCatalog;
    using winrt::Microsoft::Windows::AI::Video::VideoScaler;
    using winrt::Microsoft::Windows::AI::Video::VideoScalerOptions;
    using winrt::Microsoft::Windows::AI::Video::VideoScalerStatus;

    void LogStep(std::string_view header)
    {
        SYSTEMTIME st{};
        GetLocalTime(&st);
        std::println(std::cout, "[{:02}:{:02}:{:02}] {}", st.wHour, st.wMinute, st.wSecond, header);
        std::cout.flush();
    }

    // Human-readable name for a VideoScalerStatus value.
    std::string_view VideoScalerStatusName(VideoScalerStatus status)
    {
        switch (status)
        {
        case VideoScalerStatus::Success:            return "Success";
        case VideoScalerStatus::InvalidInputFormat: return "InvalidInputFormat";
        case VideoScalerStatus::InvalidOutputFormat:return "InvalidOutputFormat";
        case VideoScalerStatus::InvalidPlaneSize:   return "InvalidPlaneSize";
        case VideoScalerStatus::InvalidFrameData:   return "InvalidFrameData";
        case VideoScalerStatus::ModelNotAvailable:  return "ModelNotAvailable";
        case VideoScalerStatus::OutOfMemory:        return "OutOfMemory";
        case VideoScalerStatus::Failure:            return "Failure";
        default:                                    return "Unknown";
        }
    }

    // Actionable hint for common HRESULTs seen in the video pipeline, mirroring the
    // ImageScaler mode's diagnostics so failures are easy to triage from logs.
    std::string_view VideoHintFor(uint32_t hr)
    {
        switch (hr)
        {
        case 0x80070057: // E_INVALIDARG
            return "E_INVALIDARG: a frame/surface parameter was rejected. Check the input/output "
                   "pixel formats (NV12 in, BGRA8 out), surface sizes, and that VSR supports the "
                   "requested resolution range (240p-1440p in, 480p-1440p out).";
        case 0x80040154: // REGDB_E_CLASSNOTREG
            return "REGDB_E_CLASSNOTREG: missing WinAppSDK framework package. Check the sparse "
                   "PackageDependency and that Windows App Runtime 2 is installed.";
        case 0x80070005: // E_ACCESSDENIED
            return "E_ACCESSDENIED: missing package identity or systemAIModels capability. "
                   "Register the sparse package before running.";
        case 0xC00D36B2: // MF_E_INVALIDMEDIATYPE-ish / unsupported
            return "Media Foundation could not configure the media type. Verify the input is a "
                   "supported video file and codec.";
        case 0xC00D3704: // MF_E_NOT_AVAILABLE / no more samples
            return "Media Foundation reported no available stream/sample. Verify the input file "
                   "has a decodable video stream.";
        default:
            return {};
        }
    }

    // Wrap an ID3D11Texture2D as a WinRT IDirect3DSurface for VideoScaler.
    IDirect3DSurface WrapAsDirect3DSurface(ID3D11Texture2D* texture)
    {
        com_ptr<IDXGISurface> dxgiSurface;
        check_hresult(texture->QueryInterface(__uuidof(IDXGISurface), dxgiSurface.put_void()));
        com_ptr<::IInspectable> inspectable;
        check_hresult(CreateDirect3D11SurfaceFromDXGISurface(dxgiSurface.get(), inspectable.put()));
        return inspectable.as<IDirect3DSurface>();
    }
}

// Returns 0 on success.
int RunVideoFile(std::wstring const& inPath, std::wstring const& outPath, uint32_t scale)
{
    try
    {
        LogStep("[Step 1/7] Initializing Media Foundation and Direct3D");
        check_hresult(MFStartup(MF_VERSION));
        auto mfCleanup = wil::scope_exit([] { MFShutdown(); });

        // D3D11 device with video + BGRA support.
        com_ptr<ID3D11Device> d3dDevice;
        com_ptr<ID3D11DeviceContext> d3dContext;
        UINT createFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_VIDEO_SUPPORT;
        check_hresult(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createFlags,
                                        nullptr, 0, D3D11_SDK_VERSION,
                                        d3dDevice.put(), nullptr, d3dContext.put()));

        // Multithread protection is required when sharing the device with MF.
        if (auto mt = d3dContext.try_as<ID3D11Multithread>())
        {
            mt->SetMultithreadProtected(TRUE);
        }

        com_ptr<IMFDXGIDeviceManager> dxgiManager;
        UINT resetToken = 0;
        check_hresult(MFCreateDXGIDeviceManager(&resetToken, dxgiManager.put()));
        check_hresult(dxgiManager->ResetDevice(d3dDevice.get(), resetToken));

        LogStep("[Step 2/7] Preparing VideoScaler (NPU/GPU model)");
        ExecutionProviderCatalog::GetDefault().EnsureAndRegisterCertifiedAsync().get();
        if (VideoScaler::GetReadyState() != AIFeatureReadyState::Ready)
        {
            auto ready = VideoScaler::EnsureReadyAsync().get();
            if (ready.Status() != AIFeatureReadyResultState::Success)
            {
                std::println(std::cerr, "VideoScaler model not available (0x{:08X}).",
                             static_cast<uint32_t>(ready.ExtendedError().value));
                return 2;
            }
        }
        auto videoScaler = VideoScaler::CreateAsync().get();

        LogStep("[Step 3/7] Opening source video and configuring NV12 decode");
        com_ptr<IMFAttributes> readerAttribs;
        check_hresult(MFCreateAttributes(readerAttribs.put(), 3));
        check_hresult(readerAttribs->SetUnknown(MF_SOURCE_READER_D3D_MANAGER, dxgiManager.get()));
        check_hresult(readerAttribs->SetUINT32(MF_SOURCE_READER_ENABLE_ADVANCED_VIDEO_PROCESSING, TRUE));
        check_hresult(readerAttribs->SetUINT32(MF_READWRITE_ENABLE_HARDWARE_TRANSFORMS, TRUE));

        com_ptr<IMFSourceReader> reader;
        check_hresult(MFCreateSourceReaderFromURL(inPath.c_str(), readerAttribs.get(), reader.put()));

        // Ask the reader to deliver NV12.
        {
            com_ptr<IMFMediaType> nv12;
            check_hresult(MFCreateMediaType(nv12.put()));
            check_hresult(nv12->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video));
            check_hresult(nv12->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_NV12));
            check_hresult(reader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, nullptr, nv12.get()));
        }

        // Read back the negotiated frame geometry / rate.
        com_ptr<IMFMediaType> current;
        check_hresult(reader->GetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, current.put()));
        UINT32 inW = 0, inH = 0;
        check_hresult(MFGetAttributeSize(current.get(), MF_MT_FRAME_SIZE, &inW, &inH));
        UINT32 fpsNum = 30, fpsDen = 1;
        MFGetAttributeRatio(current.get(), MF_MT_FRAME_RATE, &fpsNum, &fpsDen);

        UINT32 outW = inW * scale;
        UINT32 outH = inH * scale;
        std::println(std::cout, "  Input {}x{} @ {}/{} fps  ->  Output {}x{}", inW, inH, fpsNum, fpsDen, outW, outH);

        LogStep("[Step 4/7] Configuring H.264 encoder (sink writer)");
        com_ptr<IMFSinkWriter> writer;
        check_hresult(MFCreateSinkWriterFromURL(outPath.c_str(), nullptr, nullptr, writer.put()));

        com_ptr<IMFMediaType> outType;
        check_hresult(MFCreateMediaType(outType.put()));
        check_hresult(outType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video));
        check_hresult(outType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_H264));
        check_hresult(outType->SetUINT32(MF_MT_AVG_BITRATE, 8'000'000));
        check_hresult(outType->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive));
        check_hresult(MFSetAttributeSize(outType.get(), MF_MT_FRAME_SIZE, outW, outH));
        check_hresult(MFSetAttributeRatio(outType.get(), MF_MT_FRAME_RATE, fpsNum, fpsDen));
        check_hresult(MFSetAttributeRatio(outType.get(), MF_MT_PIXEL_ASPECT_RATIO, 1, 1));
        DWORD streamIndex = 0;
        check_hresult(writer->AddStream(outType.get(), &streamIndex));

        // Encoder input is RGB32 (== BGRA in memory), top-down.
        com_ptr<IMFMediaType> inType;
        check_hresult(MFCreateMediaType(inType.put()));
        check_hresult(inType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video));
        check_hresult(inType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB32));
        check_hresult(inType->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive));
        check_hresult(inType->SetUINT32(MF_MT_DEFAULT_STRIDE, outW * 4));
        check_hresult(MFSetAttributeSize(inType.get(), MF_MT_FRAME_SIZE, outW, outH));
        check_hresult(MFSetAttributeRatio(inType.get(), MF_MT_FRAME_RATE, fpsNum, fpsDen));
        check_hresult(MFSetAttributeRatio(inType.get(), MF_MT_PIXEL_ASPECT_RATIO, 1, 1));
        check_hresult(writer->SetInputMediaType(streamIndex, inType.get(), nullptr));
        check_hresult(writer->BeginWriting());

        // Reusable output (BGRA) texture for VideoScaler and a CPU-readable staging copy.
        auto makeTexture = [&](UINT w, UINT h, D3D11_USAGE usage, UINT bind, UINT cpuFlags) {
            D3D11_TEXTURE2D_DESC desc{};
            desc.Width = w; desc.Height = h; desc.MipLevels = 1; desc.ArraySize = 1;
            desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
            desc.SampleDesc.Count = 1;
            desc.Usage = usage; desc.BindFlags = bind; desc.CPUAccessFlags = cpuFlags;
            com_ptr<ID3D11Texture2D> tex;
            check_hresult(d3dDevice->CreateTexture2D(&desc, nullptr, tex.put()));
            return tex;
        };
        auto outputTex = makeTexture(outW, outH, D3D11_USAGE_DEFAULT,
                                     D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, 0);
        auto stagingTex = makeTexture(outW, outH, D3D11_USAGE_STAGING, 0, D3D11_CPU_ACCESS_READ);
        auto outputSurface = WrapAsDirect3DSurface(outputTex.get());

        LogStep("[Step 5/7] Scaling frames");
        uint64_t frameIndex = 0;
        uint64_t frameDuration = (fpsNum > 0) ? (10'000'000ULL * fpsDen / fpsNum) : 333'333ULL;
        std::vector<uint8_t> rowBuf(static_cast<size_t>(outW) * outH * 4);

        for (;;)
        {
            DWORD streamFlags = 0;
            LONGLONG timestamp = 0;
            com_ptr<IMFSample> sample;
            check_hresult(reader->ReadSample(MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, nullptr,
                                             &streamFlags, &timestamp, sample.put()));
            if (streamFlags & MF_SOURCE_READERF_ENDOFSTREAM)
            {
                break;
            }
            if (!sample)
            {
                continue;
            }

            // Pull the decoded NV12 frame as a D3D texture.
            com_ptr<IMFMediaBuffer> mediaBuffer;
            check_hresult(sample->ConvertToContiguousBuffer(mediaBuffer.put()));
            auto dxgiBuffer = mediaBuffer.try_as<IMFDXGIBuffer>();
            if (!dxgiBuffer)
            {
                std::println(std::cerr, "  Frame {} is not a D3D buffer; skipping.", frameIndex);
                continue;
            }
            com_ptr<ID3D11Texture2D> nv12Array;
            check_hresult(dxgiBuffer->GetResource(__uuidof(ID3D11Texture2D), nv12Array.put_void()));
            UINT subresource = 0;
            dxgiBuffer->GetSubresourceIndex(&subresource);

            // Copy the decoded subresource into a standalone NV12 texture VSR can consume.
            D3D11_TEXTURE2D_DESC nv12Desc{};
            nv12Array->GetDesc(&nv12Desc);
            nv12Desc.ArraySize = 1; nv12Desc.MipLevels = 1;
            nv12Desc.Usage = D3D11_USAGE_DEFAULT;
            nv12Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            nv12Desc.CPUAccessFlags = 0;
            nv12Desc.MiscFlags = 0;
            com_ptr<ID3D11Texture2D> nv12Frame;
            check_hresult(d3dDevice->CreateTexture2D(&nv12Desc, nullptr, nv12Frame.put()));
            d3dContext->CopySubresourceRegion(nv12Frame.get(), 0, 0, 0, 0,
                                              nv12Array.get(), subresource, nullptr);

            auto inputSurface = WrapAsDirect3DSurface(nv12Frame.get());

            auto result = videoScaler.Scale(inputSurface, outputSurface, VideoScalerOptions());
            if (result.Status() != VideoScalerStatus::Success)
            {
                std::println(std::cerr, "  Scale failed at frame {}: status {} ({})",
                             frameIndex, static_cast<int32_t>(result.Status()),
                             VideoScalerStatusName(result.Status()));
                return 4;
            }

            // Read the BGRA output back to the CPU for the encoder.
            d3dContext->CopyResource(stagingTex.get(), outputTex.get());
            D3D11_MAPPED_SUBRESOURCE mapped{};
            check_hresult(d3dContext->Map(stagingTex.get(), 0, D3D11_MAP_READ, 0, &mapped));
            auto const* srcRow = static_cast<uint8_t const*>(mapped.pData);
            for (UINT y = 0; y < outH; ++y)
            {
                memcpy(rowBuf.data() + static_cast<size_t>(y) * outW * 4,
                       srcRow + static_cast<size_t>(y) * mapped.RowPitch,
                       static_cast<size_t>(outW) * 4);
            }
            d3dContext->Unmap(stagingTex.get(), 0);

            // Wrap the BGRA frame as an MF sample and feed the encoder.
            DWORD bufLen = static_cast<DWORD>(rowBuf.size());
            com_ptr<IMFMediaBuffer> encBuffer;
            check_hresult(MFCreateMemoryBuffer(bufLen, encBuffer.put()));
            BYTE* dst = nullptr;
            check_hresult(encBuffer->Lock(&dst, nullptr, nullptr));
            memcpy(dst, rowBuf.data(), bufLen);
            check_hresult(encBuffer->Unlock());
            check_hresult(encBuffer->SetCurrentLength(bufLen));

            com_ptr<IMFSample> encSample;
            check_hresult(MFCreateSample(encSample.put()));
            check_hresult(encSample->AddBuffer(encBuffer.get()));
            check_hresult(encSample->SetSampleTime(static_cast<LONGLONG>(frameIndex * frameDuration)));
            check_hresult(encSample->SetSampleDuration(static_cast<LONGLONG>(frameDuration)));
            check_hresult(writer->WriteSample(streamIndex, encSample.get()));

            if ((frameIndex % 30) == 0)
            {
                std::println(std::cout, "  ...frame {}", frameIndex);
                std::cout.flush();
            }
            ++frameIndex;
        }

        LogStep("[Step 6/7] Finalizing output video");
        check_hresult(writer->Finalize());

        LogStep(std::format("[Step 7/7] SUCCESS - {} frames -> {}", frameIndex, winrt::to_string(outPath)));
        return 0;
    }
    catch (winrt::hresult_error const& ex)
    {
        auto const hr = static_cast<uint32_t>(ex.code());
        std::println(std::cerr, "RunVideoFile failed: 0x{:08X}", hr);
        std::println(std::cerr, "  Message: {}", winrt::to_string(ex.message()));
        if (auto hint = VideoHintFor(hr); !hint.empty())
        {
            std::println(std::cerr, "  Hint:    {}", hint);
        }
        return static_cast<int>(ex.code());
    }
    catch (std::exception const& ex)
    {
        std::println(std::cerr, "RunVideoFile std::exception: {}", ex.what());
        return -1;
    }
}
