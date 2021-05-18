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
            var resourceLoader = new ResourceLoader();
            var uri = new System.Uri("ms-resource://323858f2-34e1-48a5-8244-5c1bc0c1f889/ClassLibrary/ClassLibraryResources/ClassLibrarySampleString");
            return resourceLoader.GetStringForUri(uri);
        }
    }
}
