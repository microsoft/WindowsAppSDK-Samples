using System.Configuration;
using System.Data;
using System.Windows;
using Microsoft.Windows.ApplicationModel.DynamicDependency;

namespace WCRforWPF;

/// <summary>
/// Interaction logic for App.xaml
/// </summary>
public partial class App : Application
{
    const int NotSupportedError = unchecked((int)0x80070032);

    public App()
    {
        if (!Bootstrap.TryInitialize(0x00010007, "experimental3", out var result))
        {
            if (result == NotSupportedError)
            {
                Console.WriteLine("Dynamic dependency is not necessary in this kind of app");
            }
            else
            {
                throw new SystemException($"Dynamic dependency initialization failed with error code {result}");
            }
        }
    }
}

