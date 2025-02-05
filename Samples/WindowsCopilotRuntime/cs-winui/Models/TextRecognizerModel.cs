﻿// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
using WindowsCopilotRuntimeSample.Models.Contracts;
using Microsoft.Graphics.Imaging;
using Microsoft.Windows.Management.Deployment;
using Microsoft.Windows.Vision;
using System;
using System.Threading;
using System.Threading.Tasks;

namespace WindowsCopilotRuntimeSample.Models;

internal class TextRecognizerModel : IModelManager
{
    private TextRecognizer? _session;

    private TextRecognizer Session => _session ?? throw new InvalidOperationException("Text Recognizer session was not created yet");

    public async Task CreateModelSessionWithProgress(IProgress<PackageDeploymentProgress> progress, CancellationToken cancellationToken = default)
    {
        if (!TextRecognizer.IsAvailable())
        {
            var textRecognizerDeploymentOperation = TextRecognizer.MakeAvailableAsync();
            textRecognizerDeploymentOperation.Progress = (_, packageDeploymentProgress) =>
            {
                progress.Report(packageDeploymentProgress);
            };
            using var _ = cancellationToken.Register(() => textRecognizerDeploymentOperation.Cancel());
            await textRecognizerDeploymentOperation;
        }
        else
        {
            progress.Report(new PackageDeploymentProgress(PackageDeploymentProgressStatus.CompletedSuccess, 100.0));
        }
        _session = await TextRecognizer.CreateAsync();
    }

    public RecognizedText RecognizeTextFromImage(ImageBuffer inputImage, TextRecognizerOptions recognizerOptions)
    {
        return Session.RecognizeTextFromImage(inputImage, recognizerOptions);
    }
}