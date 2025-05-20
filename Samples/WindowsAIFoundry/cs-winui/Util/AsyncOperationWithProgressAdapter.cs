// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
using System;
using Windows.Foundation;

namespace WindowsAISampleSample.Util;

internal class AsyncOperationWithProgressAdapter<TResult, TProgress, TResultAdapter, TProgressAdapter> : IAsyncOperationWithProgress<TResultAdapter, TProgressAdapter>
{
    private readonly IAsyncOperationWithProgress<TResult, TProgress> _source;
    private AsyncOperationProgressHandler<TResultAdapter, TProgressAdapter>? _progress;
    private AsyncOperationWithProgressCompletedHandler<TResultAdapter, TProgressAdapter>? _completed;
    private readonly Func<TResult, TResultAdapter> _resultConverterCallback;
    private readonly Func<TProgress, TProgressAdapter> _progressConverterCallback;

    internal AsyncOperationWithProgressAdapter(
        IAsyncOperationWithProgress<TResult, TProgress> source,
        Func<TResult, TResultAdapter> resultConverterCallback,
        Func<TProgress, TProgressAdapter> progressConverterCallback)
    {
        _source = source;
        _resultConverterCallback = resultConverterCallback;
        _progressConverterCallback = progressConverterCallback;
    }

    public AsyncOperationProgressHandler<TResultAdapter, TProgressAdapter>? Progress
    {
        get
        {
            return _progress;
        }

        set
        {
            _progress = value;
            void adapter(IAsyncOperationWithProgress<TResult, TProgress> _, TProgress progress)
            {
                _progress?.Invoke(this, _progressConverterCallback(progress));
            }
            _source.Progress = adapter;
        }
    }

    public AsyncOperationWithProgressCompletedHandler<TResultAdapter, TProgressAdapter>? Completed
    {
        get
        {
            return _completed;
        }

        set
        {
            _completed = value;
            void adapter(IAsyncOperationWithProgress<TResult, TProgress> _, AsyncStatus status)
            {
                // Note: the winRT async with progress brodge is only expecting 'real' completion status
                if (status != AsyncStatus.Started)
                {
                    _completed?.Invoke(this, status);
                }
            };
            _source.Completed = adapter;
        }
    }

    public Exception ErrorCode
    {
        get
        {
            return _source.ErrorCode;
        }
    }

    public uint Id
    {
        get
        {
            return _source.Id;
        }
    }

    public AsyncStatus Status
    {
        get
        {
            return _source.Status;
        }
    }

    public void Cancel()
    {
        _source.Cancel();
    }

    public void Close()
    {
        _source.Close();
    }

    public TResultAdapter GetResults()
    {
        return _resultConverterCallback(_source.GetResults());
    }
}

