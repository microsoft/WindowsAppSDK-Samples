// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Numerics;
using System.Runtime.InteropServices;

using Windows.Foundation;
using Windows.Graphics;
using Windows.Storage;
using Windows.Storage.Streams;

using Microsoft.Graphics.Canvas;
using Microsoft.Graphics.Canvas.UI.Composition;
using Microsoft.Graphics.DirectX;
using Microsoft.UI.Composition;
using Microsoft.UI.Composition.Scenes;
using Microsoft.UI.Content;
using WinRT;

class SceneNodeScenario
{
    protected static async Task<CompositionMipmapSurface> LoadMipmapFromUriAsync(CompositionGraphicsDevice graphicsDevice, Uri uri)
    {
        var canvasDevice = CanvasComposition.GetCanvasDevice(graphicsDevice);
        var canvasBitmap = await CanvasBitmap.LoadAsync(canvasDevice, uri);
        
        var mipmapSurface = graphicsDevice.CreateMipmapSurface(
            new SizeInt32(1024, 1024),
            Microsoft.Graphics.DirectX.DirectXPixelFormat.B8G8R8A8UIntNormalized,
            Microsoft.Graphics.DirectX.DirectXAlphaMode.Premultiplied);

        var drawDestRect = new Rect(0, 0, mipmapSurface.SizeInt32.Width, mipmapSurface.SizeInt32.Height);
        var drawSourceRect = new Rect(0, 0, canvasBitmap.Size.Width, canvasBitmap.Size.Height);
        for (uint level = 0; level < mipmapSurface.LevelCount; ++level)
        {
            // Draw the image to the surface
            var drawingSurface = mipmapSurface.GetDrawingSurfaceForLevel(level);

            using (var session = CanvasComposition.CreateDrawingSession(drawingSurface))
            {
                session.Clear(Windows.UI.Color.FromArgb(0, 0, 0, 0));
                session.DrawImage(canvasBitmap, drawDestRect, drawSourceRect);
            }

            drawDestRect = new Rect(0, 0, drawDestRect.Width / 2, drawDestRect.Height / 2);
        }

        return mipmapSurface;
    }

    protected static async Task<MemoryBuffer> LoadMemoryBufferFromUriAsync(Uri uri)
    {
        var file = await StorageFile.GetFileFromApplicationUriAsync(uri);
        var buffer = await FileIO.ReadBufferAsync(file);

        return CopyToMemoryBuffer(buffer);
    }

    private static MemoryBuffer CopyToMemoryBuffer(IBuffer buffer)
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

    [ComImport,
    Guid("5b0d3235-4dba-4d44-865e-8f1d0e4fd04d"),
    InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    interface IMemoryBufferByteAccess
    {
        unsafe void GetBuffer(byte** bytes, uint* capacity);
    }
}

class DuckSceneNodeScenario : SceneNodeScenario
{
    public static ContentIsland CreateIsland(Compositor compositor)
    {
        var loadSceneTask = LoadSceneAsync(compositor);
        loadSceneTask.Wait();

        var island = ContentIsland.Create(loadSceneTask.Result);
        return island;
    }

    private static async Task<SceneVisual> LoadSceneAsync(Compositor compositor)
    {
        var canvasDevice = new CanvasDevice();
        var graphicsDevice = (CompositionGraphicsDevice)CanvasComposition.CreateCompositionGraphicsDevice(compositor, canvasDevice);

        var sceneVisual = SceneVisual.Create(compositor);
        sceneVisual.RelativeOffsetAdjustment = new Vector3(0.5f, 0.5f, 0.0f);

        var worldNode = SceneNode.Create(compositor);
        sceneVisual.Root = worldNode;

        var rotateAngleAnimation = compositor.CreateScalarKeyFrameAnimation();
        rotateAngleAnimation.InsertKeyFrame(0.0f, 360.0f);
        rotateAngleAnimation.InsertKeyFrame(1.0f, 0.0f, compositor.CreateLinearEasingFunction());
        rotateAngleAnimation.Duration = TimeSpan.FromSeconds(10);
        rotateAngleAnimation.IterationBehavior = AnimationIterationBehavior.Forever;
        worldNode.Transform.RotationAxis = new Vector3(0, 1, 0);
        worldNode.Transform.StartAnimation("RotationAngleInDegrees", rotateAngleAnimation);

        var sceneNode0 = SceneNode.Create(compositor);
        sceneNode0.Transform.Scale = new Vector3(4.0f);
        sceneNode0.Transform.Translation = new Vector3(0.0f, -400.0f, 0.0f);
        worldNode.Children.Add(sceneNode0);
            
        var sceneNodeForTheGLTFMesh0 = SceneNode.Create(compositor);
        sceneNode0.Children.Add(sceneNodeForTheGLTFMesh0);

        var sceneMaterial0 = SceneMetallicRoughnessMaterial.Create(compositor);

        var mesh0 = SceneMesh.Create(compositor);
        mesh0.PrimitiveTopology = DirectXPrimitiveTopology.TriangleList;
        mesh0.FillMeshAttribute(SceneAttributeSemantic.Vertex, DirectXPixelFormat.R32G32B32Float, await LoadMemoryBufferFromUriAsync(new Uri("ms-appx:///Assets/SceneNode/Duck2.bin")));
        mesh0.FillMeshAttribute(SceneAttributeSemantic.Normal, DirectXPixelFormat.R32G32B32Float, await LoadMemoryBufferFromUriAsync(new Uri("ms-appx:///Assets/SceneNode/Duck3.bin")));
        mesh0.FillMeshAttribute(SceneAttributeSemantic.TexCoord0, DirectXPixelFormat.R32G32Float, await LoadMemoryBufferFromUriAsync(new Uri("ms-appx:///Assets/SceneNode/Duck4.bin")));
        mesh0.FillMeshAttribute(SceneAttributeSemantic.Index, DirectXPixelFormat.R16UInt, await LoadMemoryBufferFromUriAsync(new Uri("ms-appx:///Assets/SceneNode/Duck5.bin")));

        var renderComponent0 = SceneMeshRendererComponent.Create(compositor);
        renderComponent0.Mesh = mesh0;
        renderComponent0.Material = sceneMaterial0;
        sceneNodeForTheGLTFMesh0.Components.Add(renderComponent0);
        
        var sceneSurfaceMaterialInput0 = SceneSurfaceMaterialInput.Create(compositor);
        sceneSurfaceMaterialInput0.Surface = await LoadMipmapFromUriAsync(graphicsDevice, new Uri("ms-appx:///Assets/SceneNode/Duck1.bmp"));
        sceneSurfaceMaterialInput0.BitmapInterpolationMode = CompositionBitmapInterpolationMode.MagLinearMinLinearMipLinear;
        sceneSurfaceMaterialInput0.WrappingUMode = SceneWrappingMode.Repeat;
        sceneSurfaceMaterialInput0.WrappingVMode = SceneWrappingMode.Repeat;
        sceneMaterial0.BaseColorInput = sceneSurfaceMaterialInput0;
        sceneMaterial0.BaseColorFactor = new Vector4(1.0f);
        renderComponent0.UVMappings["BaseColorInput"] = SceneAttributeSemantic.TexCoord0;

        sceneMaterial0.RoughnessFactor = 1.0f;
        sceneMaterial0.MetallicFactor = 0.0f;
        sceneMaterial0.NormalScale = 1.0f;
        sceneMaterial0.OcclusionStrength = 1.0f;
        sceneMaterial0.EmissiveFactor = new Vector3(0.0f);
        sceneMaterial0.AlphaMode = SceneAlphaMode.Opaque;
        sceneMaterial0.IsDoubleSided = false;

        return sceneVisual;
    }
}

class DamagedHelmetSceneNodeScenario : SceneNodeScenario
{
    public static ContentIsland CreateIsland(Compositor compositor)
    {
        var loadSceneTask = LoadSceneAsync(compositor);
        loadSceneTask.Wait();

        var island = ContentIsland.Create(loadSceneTask.Result);
        return island;
    }

    private static async Task<SceneVisual> LoadSceneAsync(Compositor compositor)
    {
        var canvasDevice = new CanvasDevice();
        var graphicsDevice = (CompositionGraphicsDevice)CanvasComposition.CreateCompositionGraphicsDevice(compositor, canvasDevice);

        var sceneVisual = SceneVisual.Create(compositor);
        sceneVisual.RelativeOffsetAdjustment = new Vector3(0.5f, 0.5f, 0.0f);

        var worldNode = SceneNode.Create(compositor);
        sceneVisual.Root = worldNode;

        var rotateAngleAnimation = compositor.CreateScalarKeyFrameAnimation();
        rotateAngleAnimation.InsertKeyFrame(0.0f, 0.0f);
        rotateAngleAnimation.InsertKeyFrame(0.5f, 360.0f);
        rotateAngleAnimation.InsertKeyFrame(1.0f, 0.0f);
        rotateAngleAnimation.Duration = TimeSpan.FromSeconds(15);
        rotateAngleAnimation.IterationBehavior = AnimationIterationBehavior.Forever;
        worldNode.Transform.RotationAxis = new Vector3(0, 1, 0);
        worldNode.Transform.StartAnimation("RotationAngleInDegrees", rotateAngleAnimation);

        var sceneNode0 = SceneNode.Create(compositor);
        sceneNode0.Transform.Scale = new Vector3(350);
        sceneNode0.Transform.Orientation = new Quaternion(0.70710683f, 0.0f, 0.0f, 0.70710683f);
        worldNode.Children.Add(sceneNode0);
            
        var sceneNodeForTheGLTFMesh0 = SceneNode.Create(compositor);
        sceneNode0.Children.Add(sceneNodeForTheGLTFMesh0);

        var mesh0 = SceneMesh.Create(compositor);
        mesh0.PrimitiveTopology = DirectXPrimitiveTopology.TriangleList;
        mesh0.FillMeshAttribute(SceneAttributeSemantic.Vertex, DirectXPixelFormat.R32G32B32Float, await LoadMemoryBufferFromUriAsync(new Uri("ms-appx:///Assets/SceneNode/DamagedHelmet6.bin")));
        mesh0.FillMeshAttribute(SceneAttributeSemantic.Normal, DirectXPixelFormat.R32G32B32Float, await LoadMemoryBufferFromUriAsync(new Uri("ms-appx:///Assets/SceneNode/DamagedHelmet7.bin")));
        mesh0.FillMeshAttribute(SceneAttributeSemantic.TexCoord0, DirectXPixelFormat.R32G32Float, await LoadMemoryBufferFromUriAsync(new Uri("ms-appx:///Assets/SceneNode/DamagedHelmet8.bin")));
        mesh0.FillMeshAttribute(SceneAttributeSemantic.Index, DirectXPixelFormat.R16UInt, await LoadMemoryBufferFromUriAsync(new Uri("ms-appx:///Assets/SceneNode/DamagedHelmet9.bin")));

        var sceneMaterial0 = SceneMetallicRoughnessMaterial.Create(compositor);

        var renderComponent0 = SceneMeshRendererComponent.Create(compositor);
        renderComponent0.Mesh = mesh0;
        renderComponent0.Material = sceneMaterial0;
        sceneNodeForTheGLTFMesh0.Components.Add(renderComponent0);

        var sceneSurfaceMaterialInput0 = SceneSurfaceMaterialInput.Create(compositor);
        sceneSurfaceMaterialInput0.Surface = await LoadMipmapFromUriAsync(graphicsDevice, new Uri("ms-appx:///Assets/SceneNode/DamagedHelmet1.bmp"));
        sceneSurfaceMaterialInput0.BitmapInterpolationMode = CompositionBitmapInterpolationMode.MagLinearMinLinearMipLinear;
        sceneSurfaceMaterialInput0.WrappingUMode = SceneWrappingMode.Repeat;
        sceneSurfaceMaterialInput0.WrappingVMode = SceneWrappingMode.Repeat;
        sceneMaterial0.BaseColorInput = sceneSurfaceMaterialInput0;
        sceneMaterial0.BaseColorFactor = new Vector4(1.0f, 1.0f, 1.0f, 1.0f);
        renderComponent0.UVMappings["BaseColorInput"] = SceneAttributeSemantic.TexCoord0;

        var sceneSurfaceMaterialInput1 = SceneSurfaceMaterialInput.Create(compositor);
        sceneSurfaceMaterialInput1.Surface = await LoadMipmapFromUriAsync(graphicsDevice, new Uri("ms-appx:///Assets/SceneNode/DamagedHelmet2.bmp"));
        sceneSurfaceMaterialInput1.BitmapInterpolationMode = CompositionBitmapInterpolationMode.MagLinearMinLinearMipLinear;
        sceneSurfaceMaterialInput1.WrappingUMode = SceneWrappingMode.Repeat;
        sceneSurfaceMaterialInput1.WrappingVMode = SceneWrappingMode.Repeat;
        sceneMaterial0.MetallicRoughnessInput = sceneSurfaceMaterialInput1;
        sceneMaterial0.RoughnessFactor = 1.0f;
        sceneMaterial0.MetallicFactor = 1.0f;
        renderComponent0.UVMappings["MetallicRoughnessInput"] = SceneAttributeSemantic.TexCoord0;

        var sceneSurfaceMaterialInput2 = SceneSurfaceMaterialInput.Create(compositor);
        sceneSurfaceMaterialInput2.Surface = await LoadMipmapFromUriAsync(graphicsDevice, new Uri("ms-appx:///Assets/SceneNode/DamagedHelmet3.bmp"));
        sceneSurfaceMaterialInput2.BitmapInterpolationMode = CompositionBitmapInterpolationMode.MagLinearMinLinearMipLinear;
        sceneSurfaceMaterialInput2.WrappingUMode = SceneWrappingMode.Repeat;
        sceneSurfaceMaterialInput2.WrappingVMode = SceneWrappingMode.Repeat;
        sceneMaterial0.NormalInput = sceneSurfaceMaterialInput2;
        sceneMaterial0.NormalScale = 1.0f;
        renderComponent0.UVMappings["NormalInput"] = SceneAttributeSemantic.TexCoord0;

        var sceneSurfaceMaterialInput3 = SceneSurfaceMaterialInput.Create(compositor);
        sceneSurfaceMaterialInput3.Surface = await LoadMipmapFromUriAsync(graphicsDevice, new Uri("ms-appx:///Assets/SceneNode/DamagedHelmet4.bmp"));
        sceneSurfaceMaterialInput3.BitmapInterpolationMode = CompositionBitmapInterpolationMode.MagLinearMinLinearMipLinear;
        sceneSurfaceMaterialInput3.WrappingUMode = SceneWrappingMode.Repeat;
        sceneSurfaceMaterialInput3.WrappingVMode = SceneWrappingMode.Repeat;
        sceneMaterial0.OcclusionInput = sceneSurfaceMaterialInput3;
        sceneMaterial0.OcclusionStrength = 1.0f;
        renderComponent0.UVMappings["OcclusionInput"] = SceneAttributeSemantic.TexCoord0;

        var sceneSurfaceMaterialInput4 = SceneSurfaceMaterialInput.Create(compositor);
        sceneSurfaceMaterialInput4.Surface = await LoadMipmapFromUriAsync(graphicsDevice, new Uri("ms-appx:///Assets/SceneNode/DamagedHelmet5.bmp"));
        sceneSurfaceMaterialInput4.BitmapInterpolationMode = CompositionBitmapInterpolationMode.MagLinearMinLinearMipLinear;
        sceneSurfaceMaterialInput4.WrappingUMode = SceneWrappingMode.Repeat;
        sceneSurfaceMaterialInput4.WrappingVMode = SceneWrappingMode.Repeat;
        sceneMaterial0.EmissiveInput = sceneSurfaceMaterialInput4;
        sceneMaterial0.EmissiveFactor = new Vector3(1.0f, 1.0f, 1.0f);
        sceneMaterial0.AlphaMode = SceneAlphaMode.Opaque;
        sceneMaterial0.IsDoubleSided = false;
        renderComponent0.UVMappings["EmissiveInput"] = SceneAttributeSemantic.TexCoord0;

        return sceneVisual;
    }
}