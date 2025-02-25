using Microsoft.UI.Xaml.Hosting;
using Microsoft.UI.Xaml.Markup;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace WinFormsWithIsland
{
    /// <summary>
    /// The main Xaml application class.  This is required for Xaml controls to work correctly.
    /// </summary>
    internal class XamlApp : Microsoft.UI.Xaml.Application, IXamlMetadataProvider
    {
        public XamlApp()
        {
            _xamlMetaDataProvider = new Microsoft.UI.Xaml.XamlTypeInfo.XamlControlsXamlMetaDataProvider();
            _windowsXamlManager = WindowsXamlManager.InitializeForCurrentThread();
        }

        override protected void OnLaunched(Microsoft.UI.Xaml.LaunchActivatedEventArgs args)
        {
            this.Resources.MergedDictionaries.Add(new Microsoft.UI.Xaml.Controls.XamlControlsResources());
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

        WindowsXamlManager _windowsXamlManager;
        IXamlMetadataProvider _xamlMetaDataProvider;
    }
}
