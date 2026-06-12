// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#include <Windows.h>
#include <appmodel.h>
#include <Unknwn.h>
#include <libloaderapi2.h>
#include <wil/cppwinrt.h>
#include <wil/stl.h>
#include <wil/resource.h>
#include <vector>
#include <span>
#include <string>
#include <string_view>
#include <algorithm>
#include <cwctype>
#include <iostream>
#include <format>
#include <print>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.ApplicationModel.h>
#include <winrt/Windows.Graphics.Imaging.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Microsoft.Windows.AI.h>
#include <winrt/Microsoft.Windows.AI.Imaging.h>
#include <winrt/Microsoft.Windows.AI.Text.h>
#include <winrt/Microsoft.Windows.AI.Video.h>
#include <winrt/Microsoft.Windows.AI.MachineLearning.h>
#include <winrt/Microsoft.Graphics.Imaging.h>
#include <winrt/Microsoft.Windows.Management.Deployment.h>
#include <winrt/Microsoft.Windows.Workloads.h>
#include "formatters.h"

bool ProcessArgs(std::vector<std::wstring_view> const& args, std::wstring& prompt, bool& useProgress,
                 std::wstring& imagePath, uint32_t& imageScale, std::wstring& videoFilePath);
int RunImageScaler(std::wstring const& imagePath, uint32_t scale);
int RunVideoFile(std::wstring const& inPath, std::wstring const& outPath, uint32_t scale);
std::string_view LanguageModelHintFor(uint32_t hr);
void UnlockLanguageModelFeature();

int wmain(int argc, wchar_t* argv[])
try
{
    winrt::init_apartment();
    std::wstring instructions =
        L"You are a clever storyteller. You write engaging one-paragraph stories that grab the imagination of an 8th grader. Tell a story about a dragon who says: ";

    std::wstring prompt;
    std::wstring imagePath;
    std::wstring videoFilePath;
    uint32_t imageScale = 4;
    bool useProgress = false;
    if (!ProcessArgs({argv + 1, argv + argc}, prompt, useProgress, imagePath, imageScale, videoFilePath))
    {
        return 0;
    }

    // If the user passed --image or --video, run the corresponding imaging flow and exit.
    // Otherwise, fall through to the original LanguageModel storyteller demo.
    if (!imagePath.empty())
    {
        return RunImageScaler(imagePath, imageScale);
    }
    if (!videoFilePath.empty())
    {
        auto const dot = videoFilePath.find_last_of(L'.');
        std::wstring outPath = (dot == std::wstring::npos)
            ? videoFilePath + L"_vsr.mp4"
            : videoFilePath.substr(0, dot) + L"_vsr.mp4";
        return RunVideoFile(videoFilePath, outPath, imageScale);
    }

    if (prompt.empty())
    {
        prompt = L"Where did my treasures go?";
    }

    // On the stable Windows App SDK channel, the LanguageModel (Phi Silica) APIs are a
    // Limited Access Feature (LAF). Unlock it with the token Microsoft issued for this app
    // identity before using the model. The token is read from the LAF_LANGUAGEMODEL_TOKEN
    // environment variable so it is never committed to source. Request a token for your own
    // app at https://aka.ms/laffeatures. (Experimental releases do not require a LAF token.)
    UnlockLanguageModelFeature();

    try
    {
        // Get the language model ready for use. This may involve downloading the model and supporting
        // components. You might want to show the user a progress indicator while that's happening.
        //
        // This sample intentionally blocks until the model is available.
        auto readyState = winrt::Microsoft::Windows::AI::Text::LanguageModel::EnsureReadyAsync().get();
        if (readyState.Status() != winrt::Microsoft::Windows::AI::AIFeatureReadyResultState::Success)
        {
            std::println("Language model not available yet, status: {:x}", static_cast<int>(readyState.Status()));
            throw std::runtime_error(std::format(
                "Language model cannot be made available, status {} error {} (ext error {:x}) - {}\n",
                static_cast<int>(readyState.Status()),
                static_cast<int>(readyState.Error()),
                readyState.ExtendedError().value,
                winrt::to_string(readyState.ErrorDisplayText())));
        }
        std::println("Language model is available.");

        // Create an instance of the language model to use in this app.  Creating can also take time as
        // the model is loaded into the system. Consider showing a progress indicator while CreateAsync
        // is running. This sample intentionally blocks until the model is created.
        //
        // Your app can reuse instances of the LanguageModel once created.
        auto languageModel = winrt::Microsoft::Windows::AI::Text::LanguageModel::CreateAsync().get();
        auto options = winrt::Microsoft::Windows::AI::Text::LanguageModelOptions();
        options.TopK(15);
        options.Temperature(0.9f);

        // Request a response from the language model. The model will generate a response based on the
        // story prompt above.
        std::println("Generating response...");
        if (useProgress)
        {
            // Your app can show incremental progress updates as the model is generating a response. As
            // with other generative language models, intermediate outputs may be changed or discarded
            // while the model is running. Your app should use the final Response string in its work,
            // but showing incremental progress is a great way to show the user something on the way.
            auto responseWait = languageModel.GenerateResponseAsync(instructions + prompt, options);
            responseWait.Progress([](auto const& sender, auto const& progress) {
                std::print(std::cout, "{}", progress);
            });

            auto response = responseWait.get();
            std::println(std::cout, "Response: {}\n(status {})", response.Text(), static_cast<unsigned int>(response.Status()));
        }
        else
        {
            auto response = languageModel.GenerateResponseAsync(instructions + prompt, options).get();
            std::println(std::cout, "Response: {}\n(status {})", response.Text(), static_cast<unsigned int>(response.Status()));
        }
    }
    catch (winrt::hresult_error const& ex)
    {
        auto const hr = static_cast<uint32_t>(ex.code());
        std::println(std::cerr, "LanguageModel failed: 0x{:08x}", hr);
        if (auto hint = LanguageModelHintFor(hr); !hint.empty())
        {
            std::println(std::cerr, "Hint: {}", hint);
        }
        throw;
    }

    return 0;
}
catch (std::exception const& e)
{
    std::println(std::cerr, "Exception: {}\n", e.what());
    return -1;
}
catch (...)
{
    auto ex = wil::ResultFromCaughtException();
    std::println(std::cerr, "Exception: 0x{:08x}", static_cast<uint32_t>(ex));
    return ex;
}

std::string_view LanguageModelHintFor(uint32_t hr)
{
    switch (hr)
    {
    case 0x80070005: // E_ACCESSDENIED
        return "E_ACCESSDENIED: on current Learn docs, Phi Silica LanguageModel on stable releases "
               "may require LAF access. Verify package identity, systemAIModels, and Phi Silica "
               "in AI Dev Gallery.";
    case 0x80040154: // REGDB_E_CLASSNOTREG
        return "REGDB_E_CLASSNOTREG: missing WinAppSDK framework package. Check the sparse "
               "PackageDependency and installed Windows App Runtime.";
    default:
        return {};
    }
}

// Unlocks the LanguageModel (Phi Silica) Limited Access Feature on the stable channel.
// The token and publisher id are read from environment variables so no secret is committed
// to source. If LAF_LANGUAGEMODEL_TOKEN is not set, this is a no-op and the model call will
// surface its own access error (the most common case on the experimental channel, where no
// LAF token is required). Request a token for your app at https://aka.ms/laffeatures.
void UnlockLanguageModelFeature()
{
    wchar_t token[256]{};
    if (GetEnvironmentVariableW(L"LAF_LANGUAGEMODEL_TOKEN", token, ARRAYSIZE(token)) == 0)
    {
        return; // No token configured; nothing to unlock here.
    }

    wchar_t publisherId[128]{};
    if (GetEnvironmentVariableW(L"LAF_LANGUAGEMODEL_PUBLISHER_ID", publisherId, ARRAYSIZE(publisherId)) == 0)
    {
        wcscpy_s(publisherId, L"the app publisher");
    }

    constexpr wchar_t featureId[] = L"com.microsoft.windows.ai.languagemodel";
    auto usage = std::format(
        L"{} has registered their use of {} with Microsoft and agrees to the terms of use.",
        publisherId, featureId);

    auto access = winrt::Windows::ApplicationModel::LimitedAccessFeatures::TryUnlockFeature(
        featureId, token, usage);

    using winrt::Windows::ApplicationModel::LimitedAccessFeatureStatus;
    if ((access.Status() != LimitedAccessFeatureStatus::Available) &&
        (access.Status() != LimitedAccessFeatureStatus::AvailableWithoutToken))
    {
        std::println(std::cerr, "LAF unlock for LanguageModel returned status: {}",
                     static_cast<int>(access.Status()));
    }
}


bool ProcessArgs(std::vector<std::wstring_view> const& args, std::wstring& prompt, bool& useProgress,
                 std::wstring& imagePath, uint32_t& imageScale, std::wstring& videoFilePath)
{
    for (size_t i = 0; i < args.size(); ++i)
    {
        auto const& arg = args[i];
        if (arg == L"--progress")
        {
            useProgress = true;
        }
        else if (arg == L"--image")
        {
            if (i + 1 < args.size())
            {
                imagePath.assign(args[++i]);
            }
        }
        else if (arg == L"--video")
        {
            if (i + 1 < args.size())
            {
                videoFilePath.assign(args[++i]);
            }
        }
        else if (arg == L"--scale")
        {
            if (i + 1 < args.size())
            {
                try { imageScale = static_cast<uint32_t>(std::stoul(std::wstring(args[++i]))); }
                catch (...) { /* keep default */ }
            }
        }
        else if ((arg == L"--help") || (arg == L"-h"))
        {
            std::println(std::cout, "Usage: cmake-ai-generator [--progress] <prompt text>");
            std::println(std::cout, "       cmake-ai-generator --image <path> [--scale <factor>]");
            std::println(std::cout, "       cmake-ai-generator --video <path> [--scale <factor>]");
            std::println(std::cout, "");
            std::println(std::cout, "  Storyteller mode (default):");
            std::println(std::cout, "    --progress             Show incremental progress updates.");
            std::println(std::cout, "    <prompt text>          The prompt to use for the language model.");
            std::println(std::cout, "");
            std::println(std::cout, "  Image super-resolution mode:");
            std::println(std::cout, "    --image <path>         Upscale the given JPG/PNG/BMP via ImageScaler.");
            std::println(std::cout, "    --scale <factor>       Integer scale factor (default: 4).");
            std::println(std::cout, "                           Output is written as <name>_x<factor><ext>.");
            std::println(std::cout, "");
            std::println(std::cout, "  Video super-resolution mode:");
            std::println(std::cout, "    --video <path>         Upscale a whole video file via VideoScaler (writes <name>_vsr.mp4).");
            std::println(std::cout, "    --scale <factor>       Integer scale factor (default: 4).");
            std::println(std::cout, "");
            std::println(std::cout, "  --help, -h              Show this help message.");
            return false;
        }
        else
        {
            prompt.append(L" ").append(arg);
        }
    }

    return true;
}

// ---------------------------------------------------------------------------
// ImageScaler super-resolution mode (--image)
//
// Demonstrates Microsoft.Windows.AI.Imaging.ImageScaler from the same
// sparse-packaged console app. Prints a [Step N/6] header at each phase and
// maps common HRESULTs to actionable hints, so partners hitting the typical
// packaging / identity / model-readiness failure modes can self-diagnose.
// ---------------------------------------------------------------------------
namespace ImageScalerMode
{
    using winrt::hresult_error;
    using winrt::hstring;
    using winrt::Windows::Foundation::IAsyncAction;
    using winrt::Windows::Graphics::Imaging::BitmapDecoder;
    using winrt::Windows::Graphics::Imaging::BitmapEncoder;
    using winrt::Windows::Graphics::Imaging::SoftwareBitmap;
    using winrt::Windows::Storage::FileAccessMode;
    using winrt::Windows::Storage::StorageFile;
    using winrt::Microsoft::Windows::AI::AIFeatureReadyResultState;
    using winrt::Microsoft::Windows::AI::AIFeatureReadyState;
    using winrt::Microsoft::Windows::AI::Imaging::ImageScaler;

    void LogStep(std::string_view header)
    {
        SYSTEMTIME st{};
        GetLocalTime(&st);
        std::println(std::cout, "[{:02}:{:02}:{:02}] {}", st.wHour, st.wMinute, st.wSecond, header);
        std::cout.flush();
    }

    GUID EncoderIdForPath(std::wstring const& path)
    {
        auto const dot = path.find_last_of(L'.');
        if (dot == std::wstring::npos)
        {
            return BitmapEncoder::PngEncoderId();
        }

        auto extension = path.substr(dot);
        std::transform(extension.begin(), extension.end(), extension.begin(),
                       [](wchar_t c) { return static_cast<wchar_t>(std::towlower(c)); });

        if ((extension == L".jpg") || (extension == L".jpeg"))
        {
            return BitmapEncoder::JpegEncoderId();
        }
        if (extension == L".bmp")
        {
            return BitmapEncoder::BmpEncoderId();
        }
        if ((extension == L".tif") || (extension == L".tiff"))
        {
            return BitmapEncoder::TiffEncoderId();
        }
        return BitmapEncoder::PngEncoderId();
    }

    std::string_view HintFor(uint32_t hr)
    {
        switch (hr)
        {
        case 0x80040154: // REGDB_E_CLASSNOTREG
            return "REGDB_E_CLASSNOTREG: missing WinAppSDK framework package. Check the sparse "
                   "PackageDependency and installed Windows App Runtime.";
        case 0x80070005: // E_ACCESSDENIED
            return "E_ACCESSDENIED: missing package identity or systemAIModels capability.";
        case 0x80073D06: // ERROR_INSTALL_PACKAGE_NOT_REGISTERED
            return "Package not registered. Re-run Add-AppxPackage -ExternalLocation.";
        case 0x80004005: // E_FAIL
            return "E_FAIL: likely manifest issue. Check MaxVersionTested and systemAIModels.";
        case 0x80070002: // ERROR_FILE_NOT_FOUND
            return "File not found. Verify the input image path exists and is readable.";
        case 0x80070057: // E_INVALIDARG
            return "E_INVALIDARG: check the image format and requested scale factor.";
        default:
            return {};
        }
    }

    void LogHResultError(std::string_view step, hresult_error const& ex)
    {
        auto const hr = static_cast<uint32_t>(ex.code());
        std::println(std::cerr, "ERROR at step \"{}\"", step);
        std::println(std::cerr, "  HRESULT: 0x{:08X}", hr);
        std::println(std::cerr, "  Message: {}", winrt::to_string(ex.message()));
        auto const hint = HintFor(hr);
        if (!hint.empty())
        {
            std::println(std::cerr, "  Hint:    {}", hint);
        }
        std::cerr.flush();
    }

    bool ReportPackageIdentity()
    {
        try
        {
            auto pkg = winrt::Windows::ApplicationModel::Package::Current();
            auto id = pkg.Id();
            std::println(std::cout, "  Package identity: YES");
            std::println(std::cout, "    Name      : {}", id.Name());
            std::println(std::cout, "    Publisher : {}", id.Publisher());
            std::println(std::cout, "    FullName  : {}", id.FullName());
            return true;
        }
        catch (hresult_error const& ex)
        {
            std::println(std::cout, "  Package identity: NO (0x{:08X})",
                         static_cast<uint32_t>(ex.code()));
            std::println(std::cout,
                         "    The ImageScaler API requires package identity. Register the "
                         "sparse package via Add-AppxPackage before running this exe.");
            return false;
        }
    }

    std::wstring ResolveAbsolutePath(std::wstring_view input)
    {
        wchar_t resolved[MAX_PATH * 4]{};
        DWORD len = GetFullPathNameW(std::wstring(input).c_str(),
                                     static_cast<DWORD>(std::size(resolved)), resolved, nullptr);
        if (len == 0 || len >= std::size(resolved))
        {
            return std::wstring(input);
        }
        return std::wstring(resolved, len);
    }

    std::wstring ComputeOutputPath(std::wstring const& inputPath, uint32_t scale)
    {
        auto const dot = inputPath.find_last_of(L'.');
        std::wstring suffix = L"_x" + std::to_wstring(scale);
        if (dot == std::wstring::npos || dot < inputPath.find_last_of(L"\\/"))
        {
            return inputPath + suffix;
        }
        return inputPath.substr(0, dot) + suffix + inputPath.substr(dot);
    }

    IAsyncAction RunAsync(std::wstring inputPath, uint32_t scale)
    {
        inputPath = ResolveAbsolutePath(inputPath);
        auto outputPath = ComputeOutputPath(inputPath, scale);
        std::println(std::cout, "  Input  : {}", winrt::to_string(inputPath));
        std::println(std::cout, "  Output : {}", winrt::to_string(outputPath));

        LogStep("[Step 2/6] ImageScaler::GetReadyState");
        auto readyState = ImageScaler::GetReadyState();
        std::println(std::cout, "  ReadyState = {}", static_cast<int32_t>(readyState));

        if (readyState != AIFeatureReadyState::Ready)
        {
            LogStep("[Step 3/6] ImageScaler::EnsureReadyAsync (downloading model if needed)");
            auto readyResult = co_await ImageScaler::EnsureReadyAsync();
            auto status = readyResult.Status();
            std::println(std::cout, "  EnsureReady Status = {}", static_cast<int32_t>(status));
            if (status != AIFeatureReadyResultState::Success)
            {
                auto extErr = readyResult.ExtendedError();
                std::println(std::cerr, "  ExtendedError    : 0x{:08X}",
                             static_cast<uint32_t>(extErr.value));
                std::println(std::cerr, "  ErrorDisplayText : {}",
                             winrt::to_string(readyResult.ErrorDisplayText()));
                std::println(std::cerr,
                             "  Hint: model could not be made available. Verify Settings > "
                             "Windows Update is current, the device is a Copilot+ PC, and that "
                             "Settings > System > AI Components lists the required model.");
                throw hresult_error(extErr);
            }
        }
        else
        {
            LogStep("[Step 3/6] ImageScaler model already ready");
        }

        LogStep("[Step 4/6] ImageScaler::CreateAsync");
        auto imageScaler = co_await ImageScaler::CreateAsync();
        std::println(std::cout, "  MaxSupportedScaleFactor = {}", imageScaler.MaxSupportedScaleFactor());
        if (scale > static_cast<uint32_t>(imageScaler.MaxSupportedScaleFactor()))
        {
            throw hresult_error(E_INVALIDARG,
                                L"Requested scale factor exceeds MaxSupportedScaleFactor.");
        }

        LogStep("[Step 5/6] Decoding input image");
        auto file = co_await StorageFile::GetFileFromPathAsync(hstring(inputPath));
        auto stream = co_await file.OpenAsync(FileAccessMode::Read);
        auto decoder = co_await BitmapDecoder::CreateAsync(stream);
        auto inputBitmap = co_await decoder.GetSoftwareBitmapAsync();
        auto const w = inputBitmap.PixelWidth();
        auto const h = inputBitmap.PixelHeight();
        std::println(std::cout, "  Decoded: {}x{}  pixelFormat={}", w, h,
                     static_cast<int>(inputBitmap.BitmapPixelFormat()));

        auto const newW = static_cast<int32_t>(w) * static_cast<int32_t>(scale);
        auto const newH = static_cast<int32_t>(h) * static_cast<int32_t>(scale);
        std::println(std::cout, "  Scaling to {}x{}", newW, newH);
        auto scaled = imageScaler.ScaleSoftwareBitmap(inputBitmap, newW, newH);

        LogStep("[Step 6/6] Encoding and saving output image");
        auto parent = co_await file.GetParentAsync();
        auto outName = outputPath.substr(outputPath.find_last_of(L"\\/") + 1);
        auto outFile = co_await parent.CreateFileAsync(
            hstring(outName),
            winrt::Windows::Storage::CreationCollisionOption::ReplaceExisting);
        auto outStream = co_await outFile.OpenAsync(FileAccessMode::ReadWrite);
        auto encoder = co_await BitmapEncoder::CreateAsync(EncoderIdForPath(outputPath), outStream);
        encoder.SetSoftwareBitmap(scaled);
        co_await encoder.FlushAsync();
        std::println(std::cout, "  Saved: {}", winrt::to_string(outFile.Path()));
    }
}

int RunImageScaler(std::wstring const& imagePath, uint32_t scale)
{
    using namespace ImageScalerMode;
    try
    {
        LogStep("[Step 1/6] Verifying package identity");
        if (!ReportPackageIdentity())
        {
            return 2;
        }
        if (GetFileAttributesW(imagePath.c_str()) == INVALID_FILE_ATTRIBUTES)
        {
            std::println(std::cerr, "ERROR: input file not found: {}", winrt::to_string(imagePath));
            return 3;
        }
        RunAsync(imagePath, scale).get();
        LogStep("SUCCESS");
        return 0;
    }
    catch (hresult_error const& ex)
    {
        LogHResultError("RunImageScaler", ex);
        return static_cast<int>(ex.code());
    }
    catch (std::exception const& ex)
    {
        std::println(std::cerr, "std::exception: {}", ex.what());
        return -1;
    }
}
