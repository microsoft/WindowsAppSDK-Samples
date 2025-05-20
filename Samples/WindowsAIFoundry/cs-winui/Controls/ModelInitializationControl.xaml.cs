﻿// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
using WindowsAISample.ViewModels;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;

namespace WindowsAISample.Controls;

public sealed partial class ModelInitializationControl : UserControl
{
    public ModelInitializationControl()
    {
        InitializeComponent();
    }

    public static readonly DependencyProperty InitializationSourceFile =
            DependencyProperty.Register("SourceFile", typeof(string), typeof(ModelInitializationControl), new PropertyMetadata(string.Empty));

    public string SourceFile
    {
        get { return (string)GetValue(InitializationSourceFile); }
        set { SetValue(InitializationSourceFile, value); }
    }
}
