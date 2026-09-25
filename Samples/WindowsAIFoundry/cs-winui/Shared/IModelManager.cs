// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
using System;
using System.Threading;
using System.Threading.Tasks;

namespace WindowsAISample.Models.Contracts;

public interface IModelManager
{
    public Task CreateModelSessionWithProgress(IProgress<double> progress, CancellationToken cancellationToken = default);
}
