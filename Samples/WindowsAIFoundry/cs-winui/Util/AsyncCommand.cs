// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
using System;
using System.Threading.Tasks;
using System.Windows.Input;

namespace WindowsAISampleSample.Util;

internal interface IAsyncCommandDelegate<TParameter, TResult>
{
    Task<TResult> Execute(TParameter? parameter);
    bool CanExecute(TParameter? parameter);
}

/// <summary>
/// ICommand implementation to use Task async callbacks
/// </summary>
/// <typeparam name="TParameter">Command parameter type</typeparam>
/// <typeparam name="TResult">Result parameter type</typeparam>
internal partial class AsyncCommand<TParameter, TResult> : ModelBase, ICommand
{
    private Task? _executeTask;
    private TResult? _result;
    private Exception? _error;
    private bool _canExecute;

    protected IAsyncCommandDelegate<TParameter, TResult> AsyncCommandDelegate { get; }

    internal AsyncCommand(IAsyncCommandDelegate<TParameter, TResult> asyncCommandDelegate)
    {
        AsyncCommandDelegate = asyncCommandDelegate;
    }

    internal AsyncCommand(
        Func<TParameter?, Task<TResult>> executeCallback,
        Func<TParameter?, bool> canExecuteCallback)
        : this(new DefaultAsyncCommandDelegate<TParameter, TResult>(executeCallback, canExecuteCallback))
    {
    }

    internal AsyncCommand(
        Func<TParameter?, TResult> executeCallback,
        Func<TParameter?, bool> canExecuteCallback)
        : this(ToCallbackAsync(executeCallback), canExecuteCallback)
    {
    }

    public event EventHandler? CanExecuteChanged;

    public TResult? Result
    {
        get
        {
            return _result;
        }
    }

    public Exception? Error
    {
        get
        {
            return _error;
        }
    }

    public bool IsExecutable
    {
        get
        {
            return _executeTask is null && _canExecute;
        }
    }

    public bool IsExecuting
    {
        get
        {
            return _executeTask is not null;
        }
    }

    public void FireCanExecuteChanged()
    {
        CanExecuteChanged?.Invoke(this, EventArgs.Empty);
    }

    public bool CanExecute(object? parameter)
    {
        var canExecuteResult = CanExecuteInternal(parameter);
        if (canExecuteResult != _canExecute)
        {
            _canExecute = canExecuteResult;
            OnPropertyChanged(nameof(IsExecutable));
        }
        return canExecuteResult;
    }

    public void Execute(object? parameter)
    {
        if (_executeTask is not null)
        {
            throw new InvalidOperationException("Execute task != null");
        }

        if (!AsyncCommandDelegate.CanExecute(CastParameter(parameter)))
        {
            throw new InvalidOperationException("CanExecute == false");
        }

        _executeTask = Task.Run(async () =>
        {
            _result = default;
            _error = null;

            DispatcherQueue.TryEnqueue(() =>
            {
                FirePropertiesChanged();
            });

            try
            {
                _result = await AsyncCommandDelegate.Execute(CastParameter(parameter));
                _error = null;
            }
            catch (Exception ex)
            {
                _result = default;
                _error = ex;
            }

            DispatcherQueue.TryEnqueue(() =>
            {
                _executeTask = null;
                FirePropertiesChanged();
            });
        });
    }

    private bool CanExecuteInternal(object? parameter)
    {
        if (_executeTask is not null)
        {
            return false;
        }

        return AsyncCommandDelegate.CanExecute(CastParameter(parameter));
    }

    private static Func<TParameter?, Task<TResult>> ToCallbackAsync(Func<TParameter?, TResult> executeCallback)
    {
        return parameter =>
        {
            return Task.FromResult(executeCallback(parameter));
        };
    }

    private void FirePropertiesChanged()
    {
        OnPropertyChanged(nameof(Result));
        OnPropertyChanged(nameof(Error));
        OnPropertyChanged(nameof(IsExecuting));
        OnPropertyChanged(nameof(IsExecutable));
        FireCanExecuteChanged();
    }

    private static TParameter? CastParameter(object? parameter)
    {
        if (parameter is null)
        {
            return default;
        }

        return (TParameter)parameter;
    }
}
