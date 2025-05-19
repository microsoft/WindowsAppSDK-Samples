// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.


//using WindowsCopilotRuntimeSample.Models.Contracts;
//using Microsoft.Graphics.Imaging;
//using Microsoft.Windows.Management.Deployment;
//using System;
//using System.Threading;
//using System.Threading.Tasks;
//using Windows.Graphics.Imaging;
//using Microsoft.Windows.AI;
//using Microsoft.Windows.AI.Imaging;

//namespace WindowsCopilotRuntimeSample.Models;

//internal class ImageObjectRemoverModel : IModelManager
//{
//    private ImageObjectRemover? _session;

//    private ImageObjectRemover Session => _session ?? throw new InvalidOperationException("Image Object Remover session was not created yet");

//    public async Task CreateModelSessionWithProgress(IProgress<double> progress, CancellationToken cancellationToken = default)
//    {
//        if (ImageObjectRemover.GetReadyState() == AIFeatureReadyState.NotReady)
//        {
//            var objectRemoveDeploymentOperation = ImageObjectRemover.EnsureReadyAsync();
//            objectRemoveDeploymentOperation.Progress = (_, modelDeploymentProgress) =>
//            {
//                progress.Report(modelDeploymentProgress % 0.75);  // all progress is within 75%
//            };
//            using var _ = cancellationToken.Register(() => objectRemoveDeploymentOperation.Cancel());
//            await objectRemoveDeploymentOperation;
//        }
//        else
//        {
//            progress.Report(0.75);
//        }
//        _session = await ImageObjectRemover.CreateAsync();
//        progress.Report(1.0); // 100% progress
//    }

//    public SoftwareBitmap RemoveFromSoftwareBitmap(SoftwareBitmap inputImage, SoftwareBitmap inputMask)
//    {
//        return Session.RemoveFromSoftwareBitmap(inputImage, inputMask);
//    }
//}