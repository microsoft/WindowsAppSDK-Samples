using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Navigation;
using Windows.Devices.Radios;

namespace SDKTemplate
{
    public sealed partial class Scenario1_Toggle : Page
    {
        private MainPage rootPage = MainPage.Current;

        public Scenario1_Toggle()
        {
            this.InitializeComponent();
        }

        protected override async void OnNavigatedTo(NavigationEventArgs e)
        {
            // RequestAccessAsync must be called at least once from the UI thread
            var accessLevel = await Radio.RequestAccessAsync();
            if (accessLevel != RadioAccessStatus.Allowed)
            {
                rootPage.NotifyUser("App is not allowed to control radios.", NotifyType.ErrorMessage);
            }
            else
            {
                // An alternative to Radio.GetRadiosAsync is to use the Windows.Devices.Enumeration pattern,
                // passing Radio.GetDeviceSelector as the AQS string
                foreach (var radio in await Radio.GetRadiosAsync())
                {
                    RadioSwitchList.Items.Add(new RadioModel(radio, this.DispatcherQueue));
                }
            }
        }
    }
}
