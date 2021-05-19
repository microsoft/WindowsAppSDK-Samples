using System.Text;
using Microsoft.ApplicationModel.Resources;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace ClassLibrary
{
    public class Class
    {
        public string GetClassLibrarySampleString()
        {
            var resourceManager = new ResourceManager();
            return resourceManager.MainResourceMap.GetValue("ClassLibrary/ClassLibraryResources/ClassLibrarySampleString").ValueAsString;
        }
    }
}
