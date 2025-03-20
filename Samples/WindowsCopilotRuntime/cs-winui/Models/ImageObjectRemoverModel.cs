// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
using WindowsCopilotRuntimeSample.Models.Contracts;
using Microsoft.Graphics.Imaging;
using Microsoft.Windows.Management.Deployment;
using System;
using System.Threading;
using System.Threading.Tasks;
using Windows.Graphics.Imaging;

namespace WindowsCopilotRuntimeSample.Models;

internal class ImageObjectRemoverModel : IModelManager
{
    private ImageObjectRemover? _session;

    private ImageObjectRemover Session => _session ?? throw new InvalidOperationException("Image Object Remover session was not created yet");

    public async Task CreateModelSessionWithProgress(IProgress<PackageDeploymentProgress> progress, CancellationToken cancellationToken = default)
    {
        if (!ImageObjectRemover.IsAvailable())
        {
            var imageObjectRemoverDeploymentOperation = ImageObjectRemover.MakeAvailableAsync();
            imageObjectRemoverDeploymentOperation.Progress = (_, packageDeploymentProgress) =>
            {
                progress.Report(packageDeploymentProgress);
            };
            using var _ = cancellationToken.Register(() => imageObjectRemoverDeploymentOperation.Cancel());
            await imageObjectRemoverDeploymentOperation;
        }
        else
        {
            progress.Report(new PackageDeploymentProgress(PackageDeploymentProgressStatus.CompletedSuccess, 100.0));
        }
        _session = await ImageObjectRemover.CreateAsync();
    }

    public SoftwareBitmap RemoveFromSoftwareBitmap(SoftwareBitmap inputImage, SoftwareBitmap inputMask)
    {
        return Session.RemoveFromSoftwareBitmap(inputImage, inputMask);
    }
}