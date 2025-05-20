// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
using WindowsAISample.Models.Contracts;
using Microsoft.Graphics.Imaging;
using Microsoft.Windows.Management.Deployment;
using System;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.Windows.AI;
using Microsoft.Windows.AI.Imaging;

namespace WindowsAISample.Models;

internal class TextRecognizerModel : IModelManager
{
    private TextRecognizer? _session;

    private TextRecognizer Session => _session ?? throw new InvalidOperationException("Text Recognizer session was not created yet");

    public async Task CreateModelSessionWithProgress(IProgress<double> progress, CancellationToken cancellationToken = default)
    {
        if (TextRecognizer.GetReadyState() == AIFeatureReadyState.NotReady)
        {
            var textRecognizerDeploymentOperation = TextRecognizer.EnsureReadyAsync();
            textRecognizerDeploymentOperation.Progress = (_, modelDeploymentProgress) =>
            {
                progress.Report(modelDeploymentProgress % 0.75);
            };
            using var _ = cancellationToken.Register(() => textRecognizerDeploymentOperation.Cancel());
            await textRecognizerDeploymentOperation;
        }
        else
        {
            progress.Report(0.75);
        }
        _session = await TextRecognizer.CreateAsync();
        progress.Report(1.0);

    }

    public RecognizedText RecognizeTextFromImage(ImageBuffer inputImage)
    {
        return Session.RecognizeTextFromImage(inputImage);
    }
}