// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System;
using System.Threading;
using System.Threading.Tasks;

namespace WindowsAISample;

public interface IModelManager
{
    Task CreateModelSessionWithProgress(IProgress<double> progress, CancellationToken cancellationToken = default);
}
