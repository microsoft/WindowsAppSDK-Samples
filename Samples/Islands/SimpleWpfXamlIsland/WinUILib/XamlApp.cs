using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Hosting;
using Microsoft.UI.Xaml.Markup;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace WinUILib
{
    public class XamlApp : Microsoft.UI.Xaml.Application, IXamlMetadataProvider
    {
         
        public XamlApp(IXamlMetadataProvider provider)
        {
            ResourceManagerRequested += App_ResourceManagerRequested;
            _xamlMetaDataProvider = provider;
            _windowsXamlManager = WindowsXamlManager.InitializeForCurrentThread();
        }

        private void App_ResourceManagerRequested(object sender, ResourceManagerRequestedEventArgs args)
        {
            args.CustomResourceManager = new Microsoft.Windows.ApplicationModel.Resources.ResourceManager("WinUILib.pri");
        }

        override protected void OnLaunched(Microsoft.UI.Xaml.LaunchActivatedEventArgs args)
        {
            this.Resources.MergedDictionaries.Add(new Microsoft.UI.Xaml.Controls.XamlControlsResources());
            this.Resources.MergedDictionaries.Add(new Microsoft.UI.Xaml.ResourceDictionary {
                Source = new Uri("ms-appx:///WinUILib/ListViewStyles.xbf")
            });
        }

        IXamlType IXamlMetadataProvider.GetXamlType(string fullName)
        {
            var xamlType = _xamlMetaDataProvider.GetXamlType(fullName);
            return xamlType;
        }

        IXamlType IXamlMetadataProvider.GetXamlType(System.Type type)
        {
            var xamlType = _xamlMetaDataProvider.GetXamlType(type);
            return xamlType;
        }

        XmlnsDefinition[] IXamlMetadataProvider.GetXmlnsDefinitions()
        {
            return _xamlMetaDataProvider.GetXmlnsDefinitions();
        }

        WindowsXamlManager _windowsXamlManager;
        IXamlMetadataProvider _xamlMetaDataProvider;
    }
}
