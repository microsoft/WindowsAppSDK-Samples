// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Numerics;
using System.Runtime.InteropServices;

using Windows.Foundation;

using Microsoft.Graphics.DirectX;
using Microsoft.UI.Composition;
using Microsoft.UI.Composition.Scenes;
using Microsoft.UI.Content;
using WinRT;
using Microsoft.Graphics.Canvas.UI.Composition;
using Microsoft.Graphics.Canvas;


class DuckScenario
{
    public static ContentIsland CreateIsland(Compositor compositor)
    {
        var visual = LoadScene_Duck(compositor);

        var island = ContentIsland.Create(visual);
        return island;
    }

    public static Visual LoadScene_Duck(Compositor compositor)
    {
        // Initialize Win2D, used for loading bitmaps.

        var canvasDevice = new CanvasDevice();
        var graphicsDevice = (CompositionGraphicsDevice)CanvasComposition.CreateCompositionGraphicsDevice(compositor, canvasDevice);


        // Create the Visuals and SceneNode structure, along with default rotation animations.

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


        // Load all file data in parallel:
        // - Although Scene Graph objects prefer a UI thread, Win2D can load and create the bitmaps
        //   on parallel background threads.

        var vertexData = SceneNodeCommon.LoadMemoryBufferFromUriAsync(
            new Uri("ms-appx:///Assets/SceneNode/Duck2.bin"));

        var normalData = SceneNodeCommon.LoadMemoryBufferFromUriAsync(
            new Uri("ms-appx:///Assets/SceneNode/Duck3.bin"));

        var texCoordData = SceneNodeCommon.LoadMemoryBufferFromUriAsync(
            new Uri("ms-appx:///Assets/SceneNode/Duck4.bin"));

        var indexData = SceneNodeCommon.LoadMemoryBufferFromUriAsync(
            new Uri("ms-appx:///Assets/SceneNode/Duck5.bin"));

        var canvasBitmap0 = CanvasBitmap.LoadAsync(
            canvasDevice, new Uri("ms-appx:///Assets/SceneNode/Duck1.bmp")).AsTask();

        Task.WaitAll(
            vertexData, normalData, texCoordData, indexData, canvasBitmap0);


        // Generate mipmaps from the bitmaps, which are needed for 3D rendering.

        var materialInput0 = SceneNodeCommon.LoadMipmapFromBitmap(graphicsDevice, canvasBitmap0.Result);


        // Copy loaded binary data into mesh: verticies, normals, ...

        var mesh0 = SceneMesh.Create(compositor);
        mesh0.PrimitiveTopology = DirectXPrimitiveTopology.TriangleList;
        mesh0.FillMeshAttribute(SceneAttributeSemantic.Vertex, DirectXPixelFormat.R32G32B32Float, vertexData.Result);
        mesh0.FillMeshAttribute(SceneAttributeSemantic.Normal, DirectXPixelFormat.R32G32B32Float, normalData.Result);
        mesh0.FillMeshAttribute(SceneAttributeSemantic.TexCoord0, DirectXPixelFormat.R32G32Float, texCoordData.Result);
        mesh0.FillMeshAttribute(SceneAttributeSemantic.Index, DirectXPixelFormat.R16UInt, indexData.Result);


        // Initialize the material with different texture inputs (color, roughness, normals, ...)

        var sceneMaterial0 = SceneMetallicRoughnessMaterial.Create(compositor);

        var renderComponent0 = SceneMeshRendererComponent.Create(compositor);
        renderComponent0.Mesh = mesh0;
        renderComponent0.Material = sceneMaterial0;
        sceneNodeForTheGLTFMesh0.Components.Add(renderComponent0);

        sceneMaterial0.BaseColorFactor = new Vector4(1.0f, 1.0f, 1.0f, 1.0f);
        sceneMaterial0.BaseColorInput = SceneNodeCommon.CreateMaterial(
            compositor, materialInput0, renderComponent0, "BaseColorInput"); ;

        return sceneVisual;
    }
}
