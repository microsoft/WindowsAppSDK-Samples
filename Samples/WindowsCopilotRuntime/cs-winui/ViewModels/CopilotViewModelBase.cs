// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
using WindowsCopilotRuntimeSample.Models.Contracts;
using WindowsCopilotRuntimeSample.Util;
using Microsoft.Windows.Management.Deployment;
using System.Threading.Tasks;
using System.Threading;
using System;
using System.Windows.Input;

namespace WindowsCopilotRuntimeSample.ViewModels;

internal abstract class CopilotViewModelBase : ModelBase
{
    public abstract ICommand CreateSession { get; }

    public abstract bool IsAvailable { get; }
    public abstract string ErrorMessage { get; }
    public abstract bool IsError { get; }
}

internal abstract class CopilotModelBase<T> : CopilotViewModelBase
    where T : IModelManager
{
    private bool _isAvailable;
    private readonly AsyncCommandWithProgress<object, bool, double> _createSessionCommand;
    private bool _isError;
    private string _errorMessage = string.Empty;

    protected CopilotModelBase(T session)
    {
        Session = session;
        _createSessionCommand = new(
            (_) => new AsyncOperationWithProgressAdapter<bool, PackageDeploymentProgress, bool, double>(
                new AsyncOperationWithProgress<bool, PackageDeploymentProgress>(
                    async (progress, ct) =>
                    {
                        await using PackageDeploymentProgressAdapter progressAdapter = new(progress);
                        await Session.CreateModelSessionWithProgress(progress, ct);
                        return true;
                    }),
                result => result,
                progress => progress.Progress),
            (_) => !_isAvailable);
        _createSessionCommand.PropertyChanged += (_, e) =>
        {
            if (e.PropertyName == "Result" && _createSessionCommand.Result == true)
            {
                DispatcherQueue.TryEnqueue(() =>
                {
                    _isAvailable = true;
                    _isError = false;
                    _errorMessage = string.Empty;
                    OnPropertyChanged(nameof(IsAvailable));
                    OnPropertyChanged(nameof(IsError));
                    OnPropertyChanged(nameof(ErrorMessage));
                    _createSessionCommand!.FireCanExecuteChanged();
                    OnIsAvailableChanged();
                });
            }
            else if (e.PropertyName == "Error" && _createSessionCommand.Error is not null)
            {
                DispatcherQueue.TryEnqueue(() =>
                {
                    _isError = true;
                    _isAvailable = false;
                    _errorMessage = _createSessionCommand.Error.Message;
                    OnPropertyChanged(nameof(IsAvailable));
                    OnPropertyChanged(nameof(IsError));
                    OnPropertyChanged(nameof(ErrorMessage));
                    _createSessionCommand!.FireCanExecuteChanged();
                    OnIsAvailableChanged();
                });
            }
        };
    }

    protected T Session { get; }

    protected abstract void OnIsAvailableChanged();

    /// <summary>
    /// Command to make an AI Fabric API available
    /// </summary>
    public override ICommand CreateSession
    {
        get
        {
            return _createSessionCommand;
        }
    }

    /// <summary>
    /// Indicate if a model is available for use
    /// </summary>
    public override bool IsAvailable
    {
        get
        {
            return _isAvailable;
        }
    }

    /// <summary>
    /// Set if the model fails to create a session
    /// </summary>
    public override bool IsError
    {
        get
        {
            return _isError;
        }
    }

    /// <summary>
    /// Set if the model fails to create a session
    /// </summary>
    public override string ErrorMessage
    {
        get
        {
            return _errorMessage;
        }
    }

    private sealed class PackageDeploymentProgressAdapter : IProgress<PackageDeploymentProgress>, IAsyncDisposable
    {
        private static readonly TimeSpan progressTimeout = TimeSpan.FromMilliseconds(2500);
        private const double ProgressIncrement = 5.0;

        private readonly IProgress<PackageDeploymentProgress> _progress;
        private readonly CancellationTokenSource _disposeCts = new();
        private CancellationTokenSource _reportCts;
        private PackageDeploymentProgress _lastProgress;
        private readonly Task _runProgressTask;

        internal PackageDeploymentProgressAdapter(IProgress<PackageDeploymentProgress> progress)
        {
            _progress = progress;
            _reportCts = CancellationTokenSource.CreateLinkedTokenSource(DisposeToken);
            _runProgressTask = Task.Run(RunProgressAsync, DisposeToken);
        }

        private CancellationToken DisposeToken => _disposeCts.Token;

        public void Report(PackageDeploymentProgress value)
        {
            _lastProgress = value;
            _reportCts.Cancel();
            _progress.Report(value);
        }

        private async Task RunProgressAsync()
        {
            while (!DisposeToken.IsCancellationRequested)
            {
                try
                {
                    await Task.Delay(progressTimeout, _reportCts.Token);
                    // no report was found
                    var nextProgressValue = (_lastProgress.Progress + ProgressIncrement) % 95.0;
                    _lastProgress = new PackageDeploymentProgress(PackageDeploymentProgressStatus.InProgress, nextProgressValue);
                    _progress.Report(_lastProgress);
                }
                catch (OperationCanceledException)
                {
                    _reportCts = CancellationTokenSource.CreateLinkedTokenSource(DisposeToken);
                }
            }
        }

        public async ValueTask DisposeAsync()
        {
            _progress.Report(new PackageDeploymentProgress(PackageDeploymentProgressStatus.CompletedSuccess, 100.0));

            _disposeCts.Cancel();
            try
            {
                await _runProgressTask;

            }
            catch (OperationCanceledException)
            {
            }

            _disposeCts.Dispose();
        }
    }

}
