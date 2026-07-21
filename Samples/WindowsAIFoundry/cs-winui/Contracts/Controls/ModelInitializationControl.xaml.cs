// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
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

    public static readonly DependencyProperty AssetAssemblyNameProperty =
            DependencyProperty.Register(nameof(AssetAssemblyName), typeof(string), typeof(ModelInitializationControl), new PropertyMetadata(default(string)));

    public string SourceFile
    {
        get { return (string)GetValue(InitializationSourceFile); }
        set { SetValue(InitializationSourceFile, value); }
    }

    /// <summary>
    /// Assembly name to resolve the example markdown against when the asset
    /// is packaged inside an extension class library.
    /// </summary>
    public string AssetAssemblyName
    {
        get { return (string)GetValue(AssetAssemblyNameProperty); }
        set { SetValue(AssetAssemblyNameProperty, value); }
    }
}
