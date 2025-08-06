using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Hosting;
using Microsoft.UI.Xaml.Markup;
using System;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace WinUIApp
{
    /// <summary>
    /// Provides application-specific behavior to supplement the default Application class.
    /// </summary>
    public partial class App : Application, IXamlMetadataProvider
    {
        private Window? _window;
        WindowsXamlManager _windowsXamlManager;

        /// <summary>
        /// Initializes the singleton application object.  This is the first line of authored code
        /// executed, and as such is the logical equivalent of main() or WinMain().
        /// </summary>
        public App(IXamlMetadataProvider provider)
        {
            this.ResourceManagerRequested += App_ResourceManagerRequested;

            _xamlMetaDataProvider = provider;

            //_xamlMetaDataProvider = new Microsoft.UI.Xaml.XamlTypeInfo.XamlControlsXamlMetaDataProvider();
            //new Microsoft.UI.Xaml.XamlTypeInfo.XamlControlsXamlMetaDataProvider();



            _windowsXamlManager = WindowsXamlManager.InitializeForCurrentThread();

            //InitializeComponent();
        }

        private void App_ResourceManagerRequested(object sender, ResourceManagerRequestedEventArgs args)
        {
            args.CustomResourceManager = new Microsoft.Windows.ApplicationModel.Resources.ResourceManager("WinUILib.pri");
        }

        /// <summary>
        /// Invoked when the application is launched.
        /// </summary>
        /// <param name="args">Details about the launch request and process.</param>
        protected override void OnLaunched(Microsoft.UI.Xaml.LaunchActivatedEventArgs args)
        {
            this.Resources.MergedDictionaries.Add(new Microsoft.UI.Xaml.Controls.XamlControlsResources());
            this.Resources.MergedDictionaries.Add(new Microsoft.UI.Xaml.ResourceDictionary {
                Source = new Uri("ms-appx:///WinUILib/ListViewStyles.xbf")
            });

            //this.Resources.MergedDictionaries.Add(new Microsoft.UI.Xaml.Controls.XamlControlsResources());
            //this.Resources.MergedDictionaries.Add(new Microsoft.UI.Xaml.ResourceDictionary {
            //    Source = new Uri("ms-appx:///WinUIApp/ListViewStyles.xbf")
            //});
            //_window = new MainWindow();
            //_window.Activate();
        }

        IXamlType IXamlMetadataProvider.GetXamlType(string fullName)
        {
            return _xamlMetaDataProvider.GetXamlType(fullName);
        }

        IXamlType IXamlMetadataProvider.GetXamlType(System.Type type)
        {
            return _xamlMetaDataProvider.GetXamlType(type);
        }

        XmlnsDefinition[] IXamlMetadataProvider.GetXmlnsDefinitions()
        {
            return _xamlMetaDataProvider.GetXmlnsDefinitions();
        }

        IXamlMetadataProvider _xamlMetaDataProvider;

    }
}
