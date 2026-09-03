// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "Main.h"
#include "Helpers.h"
#include "Resource.h"
#include "ResourceFontFileLoader.h"

wil::com_ptr<ResourceFontFileLoader> ResourceFontFileLoader::m_instance;

ResourceFontFileLoader* ResourceFontFileLoader::GetInstance()
{
    // Lazily initialize the static instance.
    if (m_instance == nullptr)
    {
        // Create the loader instance.
        m_instance = new ResourceFontFileLoader{};

        // Register the loader with the factory.
        // The loader must be registered before it can be used.
        // A custom loader can also be unregistered, but in this case we'll just leave it 
        // registered for the lifetime of the factory.
        THROW_IF_FAILED(g_factory->RegisterFontFileLoader(m_instance.get()));
    }

    return m_instance.get();
}

wil::com_ptr<IDWriteFontFile> ResourceFontFileLoader::CreateFontFileReference(uint32_t resourceId)
{
    // Each font file is uniquely identified by a font file loader and loader-defined reference key,
    // which is an array of bytes that gets passed to IDWriteFontFileLoader::CreateStreamFromKey.
    // The key representation is up to the loader, but it should be trivially copyable and comparable
    // using memcpy and memcmp and should not contain internal pointers. For this loader, the
    // key is an integer resource ID.
    void const* referenceKey = &resourceId;
    uint32_t referenceKeySize = sizeof(resourceId);

    // Create the font file reference, which encapsulates the loader and key.
    wil::com_ptr<IDWriteFontFile> fontFile;
    THROW_IF_FAILED(g_factory->CreateCustomFontFileReference(referenceKey, referenceKeySize, GetInstance(), &fontFile));

    return fontFile;
}

// IUnknown method
HRESULT STDMETHODCALLTYPE ResourceFontFileLoader::QueryInterface(REFIID riid, _COM_Outptr_ void** ppvObject) noexcept
{
    if (riid == __uuidof(IDWriteFontFileLoader) ||
        riid == __uuidof(IUnknown))
    {
        AddRef();
        *ppvObject = this;
        return S_OK;
    }
    else
    {
        *ppvObject = nullptr;
        return E_NOINTERFACE;
    }
}

// IUnknown method
ULONG STDMETHODCALLTYPE ResourceFontFileLoader::AddRef() noexcept
{
    return ++m_refCount;
}

// IUnknown method
ULONG STDMETHODCALLTYPE ResourceFontFileLoader::Release() noexcept
{
    uint32_t newCount = --m_refCount;
    if (newCount == 0)
    {
        delete this;
    }
    return newCount;
}

// IDWriteFontFileLoader method
HRESULT STDMETHODCALLTYPE ResourceFontFileLoader::CreateStreamFromKey(
    _In_reads_bytes_(fontFileReferenceKeySize) void const* fontFileReferenceKey,
    UINT32 fontFileReferenceKeySize,
    _COM_Outptr_ IDWriteFontFileStream** fontFileStream
) noexcept
{
    try
    {
        // We expect the font file key to be a uint32_t resource ID.
        uint32_t resourceId;
        THROW_HR_IF(E_INVALIDARG, fontFileReferenceKeySize != sizeof(resourceId));
        memcpy(&resourceId, fontFileReferenceKey, sizeof(resourceId));

        // The key should identify a binary resource of type ID_FONT_RESOURCE.
        HRSRC info = FindResourceW(g_hInstance, MAKEINTRESOURCE(resourceId), MAKEINTRESOURCE(ID_FONT_RESOURCE));
        THROW_LAST_ERROR_IF_NULL(info);

        HGLOBAL handle = LoadResource(g_hInstance, info);
        THROW_LAST_ERROR_IF_NULL(handle);

        auto size = SizeofResource(g_hInstance, info);
        THROW_LAST_ERROR_IF(size == 0);

        auto data = static_cast<uint8_t const*>(LockResource(handle));
        THROW_LAST_ERROR_IF_NULL(data);

        // Create the stream object and wrap it in a com_ptr.
        wil::com_ptr<ResourceFontFileStream> result{ new ResourceFontFileStream{ { data, size } } };

        // Transfer ownership to the caller.
        *fontFileStream = result.detach();
        return S_OK;
    }
    catch (wil::ResultException& e)
    {
        *fontFileStream = nullptr;
        return e.GetErrorCode();
    }
}

// IUnknown method
HRESULT STDMETHODCALLTYPE ResourceFontFileStream::QueryInterface(REFIID riid, _COM_Outptr_ void** ppvObject) noexcept
{
    if (riid == __uuidof(IDWriteFontFileStream) ||
        riid == __uuidof(IUnknown))
    {
        AddRef();
        *ppvObject = this;
        return S_OK;
    }
    else
    {
        *ppvObject = nullptr;
        return E_NOINTERFACE;
    }
}

// IUnknown method
ULONG STDMETHODCALLTYPE ResourceFontFileStream::AddRef() noexcept
{
    return ++m_refCount;
}

// IUnknown method
ULONG STDMETHODCALLTYPE ResourceFontFileStream::Release() noexcept
{
    uint32_t newCount = --m_refCount;
    if (newCount == 0)
    {
        delete this;
    }
    return newCount;
}

// IDWriteFontFileStream method
HRESULT STDMETHODCALLTYPE ResourceFontFileStream::ReadFileFragment(
    _Outptr_result_bytebuffer_(fragmentSize) void const** fragmentStart,
    UINT64 fileOffset,
    UINT64 fragmentSize,
    _Out_ void** fragmentContext
) noexcept
{
    // We don't need a fragment context because there's nothing that needs
    // to be released (i.e., ReleaseFileFragment is a no-op).
    *fragmentContext = nullptr;

    // Make sure the entire fragment is in range.
    // This method of checking is safe against integer overflow.
    if (fileOffset <= m_data.size() && fragmentSize <= m_data.size() - fileOffset)
    {
        // The fragment is in range. Return a pointer to the start of the fragment.
        *fragmentStart = m_data.data() + fileOffset;
        return S_OK;
    }
    else
    {
        // The fragment is out of range.
        *fragmentStart = nullptr;
        return E_INVALIDARG;
    }
}

// IDWriteFontFileStream method
void STDMETHODCALLTYPE ResourceFontFileStream::ReleaseFileFragment(
    void* fragmentContext
) noexcept
{
    // The data exists for the lifetime of the module, so this is a no-op.
}

// IDWriteFontFileStream method
HRESULT STDMETHODCALLTYPE ResourceFontFileStream::GetFileSize(
    _Out_ UINT64* fileSize
) noexcept
{
    *fileSize = m_data.size();
    return S_OK;
}

// IDWriteFontFileStream method
HRESULT STDMETHODCALLTYPE ResourceFontFileStream::GetLastWriteTime(
    _Out_ UINT64* lastWriteTime
) noexcept
{
    *lastWriteTime = 0;
    return E_NOTIMPL;
}
