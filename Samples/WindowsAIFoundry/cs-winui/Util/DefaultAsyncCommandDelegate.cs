// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
using System;
using System.Threading.Tasks;

namespace WindowsAISampleSample.Util;

internal sealed class DefaultAsyncCommandDelegate<TParameter, TResult> : IAsyncCommandDelegate<TParameter, TResult>
{
    internal DefaultAsyncCommandDelegate()
    {
    }

    internal DefaultAsyncCommandDelegate(
        Func<TParameter?, Task<TResult>> executeCallback,
        Func<TParameter?, bool> canExecuteCallback)
    {
        ExecuteHandler = executeCallback;
        CanExecuteHandler = canExecuteCallback;
    }

    public Func<TParameter?, Task<TResult>>? ExecuteHandler { get; set; }

    public Func<TParameter?, bool>? CanExecuteHandler { get; set; }

    public bool CanExecute(TParameter? parameter)
    {
        return (CanExecuteHandler ?? throw new InvalidOperationException("CanExecuteHandler == null"))(parameter);
    }

    public Task<TResult> Execute(TParameter? parameter)
    {
        return (ExecuteHandler ?? throw new InvalidOperationException("ExecuteHandler == null"))(parameter);
    }
}
