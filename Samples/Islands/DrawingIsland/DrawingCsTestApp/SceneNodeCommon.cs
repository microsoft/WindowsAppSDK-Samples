using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.Storage.Streams;
using Windows.Storage;
using WinRT;

class SceneNodeCommon
{
    public static async Task<MemoryBuffer> LoadMemoryBufferFromUriAsync(Uri uri)
    {
        var file = await StorageFile.GetFileFromApplicationUriAsync(uri);
        var buffer = await FileIO.ReadBufferAsync(file);

        return CopyToMemoryBuffer(buffer);
    }

    public static MemoryBuffer CopyToMemoryBuffer(IBuffer buffer)
    {
        var dataReader = DataReader.FromBuffer(buffer);

        var memBuffer = new MemoryBuffer(buffer.Length);
        var memBufferRef = memBuffer.CreateReference();
        var memBufferByteAccess = memBufferRef.As<IMemoryBufferByteAccess>();

        unsafe
        {
            byte* bytes = null;
            uint capacity;
            memBufferByteAccess.GetBuffer(&bytes, &capacity);

            for (int i = 0; i < capacity; ++i)
            {
                bytes[i] = dataReader.ReadByte();
            }
        }

        return memBuffer;
    }


    public static MemoryBuffer CopyToMemoryBuffer(byte[] a)
    {
        MemoryBuffer mb = new MemoryBuffer((uint)a.Length);
        var mbr = mb.CreateReference();
        var mba = mbr.As<IMemoryBufferByteAccess>();
        unsafe
        {
            byte* bytes = null;
            uint capacity;
            mba.GetBuffer(&bytes, &capacity);
            for (int i = 0; i < capacity; ++i)
            {
                bytes[i] = a[i];
            }
        }

        return mb;
    }



    [ComImport,
    Guid("5b0d3235-4dba-4d44-865e-8f1d0e4fd04d"),
    InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    public interface IMemoryBufferByteAccess
    {
        unsafe void GetBuffer(byte** bytes, uint* capacity);
    }
}
