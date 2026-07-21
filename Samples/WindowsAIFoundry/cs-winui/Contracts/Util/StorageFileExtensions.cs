// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
using System;
using System.IO;
using System.Threading.Tasks;
using Windows.Storage;
using Windows.Storage.Streams;

namespace WindowsAISample.Util;

public static class StorageFileExtensions
{
    private static bool IsPackagedApp = (Environment.GetEnvironmentVariable("PACKAGED_PRODUCT_ID") != null);

    public static async Task<string> ReadTextAsync(this string filepath, string? assemblyName = null)
    {
        if (string.IsNullOrWhiteSpace(filepath))
        {
            return string.Empty;
        }

        if (IsPackagedApp)
        {
            StorageFile file = await CreateStorageFile(filepath, assemblyName);
            string content = await FileIO.ReadTextAsync(file);
            return content;
        }
        else
        {
            string filePath = CombineWithBasePath(filepath, assemblyName);
            string content = await File.ReadAllTextAsync(filePath);
            return content;
        }
    }

    public static async Task<IRandomAccessStream> CreateStreamAsync(this string filepath, string? assemblyName = null)
    {
        if (string.IsNullOrWhiteSpace(filepath))
        {
            return MemoryStream.Null.AsRandomAccessStream();
        }

        if (IsPackagedApp)
        {
            StorageFile file = await CreateStorageFile(filepath, assemblyName);
            return await file.OpenAsync(FileAccessMode.Read);
        }
        else
        {
            string filePath = CombineWithBasePath(filepath, assemblyName);
            return File.OpenRead(filePath).AsRandomAccessStream();
        }
    }

    private static Task<StorageFile> CreateStorageFile(string filepath, string? assemblyName)
    {
        Uri uri;
        if (filepath.StartsWith("ms-appx:", StringComparison.OrdinalIgnoreCase))
        {
            uri = new Uri(filepath);
        }
        else if (!string.IsNullOrEmpty(assemblyName))
        {
            uri = new Uri($"ms-appx:///{assemblyName}/{filepath}");
        }
        else
        {
            uri = new Uri("ms-appx:///" + filepath);
        }
        return StorageFile.GetFileFromApplicationUriAsync(uri).AsTask();
    }

    private static string CombineWithBasePath(string filepath, string? assemblyName)
    {
        if (!string.IsNullOrEmpty(assemblyName))
        {
            return Path.Combine(AppContext.BaseDirectory, assemblyName, filepath);
        }
        return Path.Combine(AppContext.BaseDirectory, filepath);
    }
}
