using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Hosting;
using Microsoft.UI.Xaml.Markup;
using Microsoft.UI.Xaml.XamlTypeInfo;
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
            AddProvider(new XamlControlsXamlMetaDataProvider());
            _windowsXamlManager = WindowsXamlManager.InitializeForCurrentThread();
        }

        public void AddProvider(IXamlMetadataProvider provider)
        {
            _providers.Add(provider);
        }

        override protected void OnLaunched(Microsoft.UI.Xaml.LaunchActivatedEventArgs args)
        {
            this.Resources.MergedDictionaries.Add(new Microsoft.UI.Xaml.Controls.XamlControlsResources());
        }

        IXamlType? IXamlMetadataProvider.GetXamlType(string fullName)
        {
            foreach (var provider in _providers)
            {
                var xamlType = provider.GetXamlType(fullName);
                if (xamlType != null)
                {
                    return xamlType;
                }
            }
            return null;
        }

        IXamlType? IXamlMetadataProvider.GetXamlType(System.Type type)
        {
            foreach (var provider in _providers)
            {
                var xamlType = provider.GetXamlType(type);
                if (xamlType != null)
                {
                    return xamlType;
                }
            }
            return null;
        }

        XmlnsDefinition[] IXamlMetadataProvider.GetXmlnsDefinitions()
        {
            var definitions = new List<XmlnsDefinition>();
            foreach (var provider in _providers)
            {
                var providerDefinitions = provider.GetXmlnsDefinitions();
                definitions.AddRange(providerDefinitions);
            }
            return definitions.ToArray();
        }

        WindowsXamlManager _windowsXamlManager;
        List<IXamlMetadataProvider> _providers = new List<IXamlMetadataProvider>();
    }
}
