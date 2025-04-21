// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
using System;
using System.Windows.Input;
using Windows.Foundation;

namespace WindowsCopilotRuntimeSample.Util;

/// <summary>
/// Async command support with progress support
/// </summary>
internal partial class AsyncCommandWithProgress<TParameter, TResult, TProgress> : AsyncCommand<TParameter, TResult>
{
    private readonly AsyncCommand<object?, bool> _cancelCommand;

    private bool _isExecutingWithProgress;
    private IAsyncOperationWithProgress<TResult, TProgress>? _currentAsyncOperationWithProgress;
    private TProgress? _currentProgress;
    private TResult? _finalResult;

    internal AsyncCommandWithProgress(
        Func<TParameter, IAsyncOperationWithProgress<TResult, TProgress>> executeCallbackWithProgress,
        Func<TParameter?, bool> canExecuteCallback)
        : base(new DefaultAsyncCommandDelegate<TParameter, TResult>())
    {
        _cancelCommand = new(
            _ =>
            {
                (_currentAsyncOperationWithProgress ?? throw new InvalidOperationException("Not executing")).Cancel();
                return true;
            },
            _ => _isExecutingWithProgress);

        var asyncCommandDelegateInstance = (DefaultAsyncCommandDelegate<TParameter, TResult>)AsyncCommandDelegate;
        asyncCommandDelegateInstance.ExecuteHandler = async parameter =>
        {
            _currentAsyncOperationWithProgress = executeCallbackWithProgress(parameter!);
            _isExecutingWithProgress = true;

            await DispatcherQueue.EnqueueAsync(() => _cancelCommand.FireCanExecuteChanged());
            _currentAsyncOperationWithProgress.Progress = (_, progress) =>
            {
                DispatcherQueue.TryEnqueue(() =>
                {
                    _currentProgress = progress;
                    OnPropertyChanged(nameof(CurrentProgress));
                    ResultProgressHandler?.Invoke(this, progress);
                });
            };

            try
            {
                _finalResult = await _currentAsyncOperationWithProgress;
                ResultHandler?.Invoke(this, _finalResult);
                return _finalResult;
            }
            finally
            {
                _currentAsyncOperationWithProgress = null;
                _isExecutingWithProgress = false;
                await DispatcherQueue.EnqueueAsync(() => _cancelCommand.FireCanExecuteChanged());
            }
        };
        asyncCommandDelegateInstance.CanExecuteHandler = canExecuteCallback;
    }

    public TProgress? CurrentProgress
    {
        get
        {
            return _currentProgress;
        }
    }

    public TResult? FinalResult
    {
        get
        {
            return _finalResult;
        }

    }

    public event EventHandler<TProgress>? ResultProgressHandler;
    public event EventHandler<TResult>? ResultHandler;

    public ICommand CancelCommand
    {
        get
        {
            return _cancelCommand;
        }
    }
}
