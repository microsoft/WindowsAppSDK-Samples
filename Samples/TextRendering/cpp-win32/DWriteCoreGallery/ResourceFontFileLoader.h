// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once

/// <summary>
/// Custom font file loader for accessing font files stored as binary resources in the executable image.
/// </summary>
class ResourceFontFileLoader final : public IDWriteFontFileLoader
{
public:
    static ResourceFontFileLoader* GetInstance();
    static wil::com_ptr<IDWriteFontFile> CreateFontFileReference(uint32_t resourceId);

    // IUnknown methods
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, _COM_Outptr_ void** ppvObject) noexcept override;
    ULONG STDMETHODCALLTYPE AddRef() noexcept override;
    ULONG STDMETHODCALLTYPE Release() noexcept override;

    // IDWriteFontFileLoader method
    HRESULT STDMETHODCALLTYPE CreateStreamFromKey(
        _In_reads_bytes_(fontFileReferenceKeySize) void const* fontFileReferenceKey,
        UINT32 fontFileReferenceKeySize,
        _COM_Outptr_ IDWriteFontFileStream** fontFileStream
        ) noexcept override;

private:
    uint32_t m_refCount = 0;
    static wil::com_ptr<ResourceFontFileLoader> m_instance;
};

/// <summary>
/// Custom font file stream created by ResourceFontFileLoader.
/// </summary>
class ResourceFontFileStream final : public IDWriteFontFileStream
{
public:
    ResourceFontFileStream(std::span<uint8_t const> data) noexcept : m_data{ data }
    {
    }

    // IUnknown methods
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, _COM_Outptr_ void** ppvObject) noexcept override;
    ULONG STDMETHODCALLTYPE AddRef() noexcept override;
    ULONG STDMETHODCALLTYPE Release() noexcept override;

    // IDWriteFontFileStream methods
    HRESULT STDMETHODCALLTYPE ReadFileFragment(
        _Outptr_result_bytebuffer_(fragmentSize) void const** fragmentStart,
        UINT64 fileOffset,
        UINT64 fragmentSize,
        _Out_ void** fragmentContext
        ) noexcept override;

    void STDMETHODCALLTYPE ReleaseFileFragment(
        void* fragmentContext
        ) noexcept override;

    HRESULT STDMETHODCALLTYPE GetFileSize(
        _Out_ UINT64* fileSize
        ) noexcept override;

    HRESULT STDMETHODCALLTYPE GetLastWriteTime(
        _Out_ UINT64* lastWriteTime
        ) noexcept override;

private:
    uint32_t m_refCount = 0;
    std::span<uint8_t const> m_data;
};

