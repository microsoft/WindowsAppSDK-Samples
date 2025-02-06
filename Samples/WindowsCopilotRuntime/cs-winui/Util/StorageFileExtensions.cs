// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
using System;
using System.Threading.Tasks;
using Windows.Storage;

namespace WindowsCopilotRuntimeSample.Util;

internal static class StorageFileExtensions
{
    public static async Task<string> ReadFromStorageFileAsync(this string filepath)
    {
        if (filepath == null)
        {
            return string.Empty;
        }
        var uri = new Uri("ms-appx:///" + filepath);
        StorageFile file = await StorageFile.GetFileFromApplicationUriAsync(uri);

        string content = await FileIO.ReadTextAsync(file);
        return content;
    }

}
