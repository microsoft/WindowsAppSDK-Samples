// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
using WindowsAISampleSample.Models.Contracts;
using WindowsAISampleSample.Util;
using Microsoft.Windows.Management.Deployment;
using System.Threading.Tasks;
using System.Threading;
using System;
using System.Windows.Input;

namespace WindowsAISampleSample.ViewModels;

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
            (_) => new AsyncOperationWithProgressAdapter<bool, double, bool, double>(
                new AsyncOperationWithProgress<bool, double>(
                    async (progress, ct) =>
                    {
                        await Session.CreateModelSessionWithProgress(progress, ct);
                        return true;
                    }),
                result => result,
                progress => progress),
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

}
