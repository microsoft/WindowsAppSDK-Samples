// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
using System;
using System.Threading;
using System.Threading.Tasks;
using Windows.Foundation;

namespace WindowsAISampleSample.Util;

internal class AsyncOperationWithProgress<TResult, TProgress> : IAsyncOperationWithProgress<TResult, TProgress>
{
    private static uint nextId;

    private AsyncOperationProgressHandler<TResult, TProgress>? _progress;
    private AsyncOperationWithProgressCompletedHandler<TResult, TProgress>? _completed;

    private readonly TaskCompletionSource<TResult> _completionSource = new();
    private readonly CancellationTokenSource _cts = new();

    internal AsyncOperationWithProgress(Func<IProgress<TProgress>, CancellationToken, Task<TResult>> taskFactory)
    {
        Id = nextId++;
        var progressAdapter = new ProgressAdapter(progress =>
        {
            _progress?.Invoke(this, progress);
        });

        Task.Run(async () =>
        {
            try
            {
                var task = taskFactory(progressAdapter, _cts.Token);
                _completed?.Invoke(this, AsyncStatus.Started);
                var result = await task;
                _completionSource.SetResult(result);
                _completed?.Invoke(this, AsyncStatus.Completed);
            }
            catch (OperationCanceledException)
            {
                _completionSource.SetCanceled();
                _completed?.Invoke(this, AsyncStatus.Canceled);
            }
            catch (Exception ex)
            {
                _completionSource.SetException(ex);
                _completed?.Invoke(this, AsyncStatus.Error);
            }
            finally
            {
                _cts.Dispose();
            }
        });
    }

    public AsyncOperationProgressHandler<TResult, TProgress>? Progress
    {
        get
        {
            return _progress;
        }

        set
        {
            _progress = value;
        }
    }

    public AsyncOperationWithProgressCompletedHandler<TResult, TProgress>? Completed
    {
        get
        {
            return _completed;
        }

        set
        {
            _completed = value;
        }
    }

    public Exception? ErrorCode
    {
        get
        {
            return _completionSource.Task.Exception;
        }
    }

    public uint Id { get; }

    public AsyncStatus Status
    {
        get
        {
            if (_cts.IsCancellationRequested)
            {
                return AsyncStatus.Canceled;
            }

            switch (_completionSource.Task.Status)
            {
                case TaskStatus.Faulted:
                    return AsyncStatus.Error;
                case TaskStatus.RanToCompletion:
                    return AsyncStatus.Completed;
                case TaskStatus.Created:
                    break;
                case TaskStatus.WaitingForActivation:
                    break;
                case TaskStatus.WaitingToRun:
                    break;
                case TaskStatus.Running:
                    break;
                case TaskStatus.WaitingForChildrenToComplete:
                    break;
                case TaskStatus.Canceled:
                    break;
            }

            return AsyncStatus.Started;
        }
    }

    public void Cancel()
    {
        _cts.Cancel();
    }

    public void Close()
    {
    }

    public TResult GetResults()
    {
        return _completionSource.Task.Result;
    }

    private sealed class ProgressAdapter : IProgress<TProgress>
    {
        private readonly Action<TProgress> _reportCallback;

        internal ProgressAdapter(Action<TProgress> reportCallback)
        {
            _reportCallback = reportCallback;
        }

        public void Report(TProgress value)
        {
            _reportCallback(value);
        }
    }
}
