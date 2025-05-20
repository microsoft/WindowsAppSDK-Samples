// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
using System.Collections.Generic;
using System.ComponentModel;
using System.Runtime.CompilerServices;
using DispatcherQueue = Microsoft.UI.Dispatching.DispatcherQueue;

namespace WindowsAISampleSample.Util;

/// <summary>
/// Lightweight model base class that implements INotifyPropertyChanged
/// </summary>
internal partial class ModelBase : INotifyPropertyChanged
{

    public event PropertyChangedEventHandler? PropertyChanged;

    protected ModelBase()
        : this(DispatcherQueue.GetForCurrentThread())
    {
    }

    protected ModelBase(DispatcherQueue dispatcherQueue)
    {
        DispatcherQueue = dispatcherQueue;
    }

    protected DispatcherQueue DispatcherQueue { get; }

    protected void OnPropertyChanged(string? propertyName)
    {
        PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
    }

    protected void DispatchPropertyChanged(string? propertyName)
    {
        DispatcherQueue.TryEnqueue(() => OnPropertyChanged(propertyName));
    }

    protected bool SetField<T>(ref T field, T value, [CallerMemberName] string? propertyName = null)
    {
        if (EqualityComparer<T>.Default.Equals(field, value))
        {
            return false;
        }

        field = value;
        OnPropertyChanged(propertyName);
        return true;
    }
}
