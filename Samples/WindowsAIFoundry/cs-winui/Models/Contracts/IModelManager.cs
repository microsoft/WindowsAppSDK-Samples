// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
using Microsoft.Windows.Management.Deployment;
using System;
using System.Threading;
using System.Threading.Tasks;

namespace WindowsAISampleSample.Models.Contracts;

internal interface IModelManager
{
    public Task CreateModelSessionWithProgress(IProgress<double> progress, CancellationToken cancellationToken = default);
}
