using System.Collections.Generic;
using System.Linq;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;

namespace SDKTemplate
{
    public sealed partial class Scenario1_ListDetails : Page
    {
        private const double NarrowThreshold = 720;

        private ItemViewModel _selected;
        private bool _isNarrow;
        private bool _showingDetail;

        public Scenario1_ListDetails()
        {
            this.InitializeComponent();

            var items = ItemsDataSource.GetAllItems()
                .Select(ItemViewModel.FromItem)
                .ToList();
            MasterListView.ItemsSource = items;
        }

        private void LayoutRoot_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            _isNarrow = e.NewSize.Width < NarrowThreshold;
            UpdateLayout();
        }

        private void MasterListView_ItemClick(object sender, ItemClickEventArgs e)
        {
            _selected = e.ClickedItem as ItemViewModel;
            MasterListView.SelectedItem = _selected;
            _showingDetail = true;
            UpdateDetailContent();
            UpdateLayout();
        }

        private void BackButton_Click(object sender, RoutedEventArgs e)
        {
            _showingDetail = false;
            UpdateLayout();
        }

        private void UpdateDetailContent()
        {
            if (_selected == null)
            {
                DetailContent.Visibility = Visibility.Collapsed;
                NoSelectionText.Visibility = Visibility.Visible;
                return;
            }

            DetailTitle.Text = _selected.Title;
            DetailDate.Text = _selected.DateCreatedHourMinute;
            DetailText.Text = _selected.Text;
            DetailContent.Visibility = Visibility.Visible;
            NoSelectionText.Visibility = Visibility.Collapsed;
        }

        private void UpdateLayout()
        {
            if (!_isNarrow)
            {
                // Wide: list and details side by side.
                MasterColumn.Width = new GridLength(320);
                DetailColumn.Width = new GridLength(1, GridUnitType.Star);
                MasterPane.Visibility = Visibility.Visible;
                DetailPane.Visibility = Visibility.Visible;
                BackButton.Visibility = Visibility.Collapsed;
                UpdateDetailContent();
                return;
            }

            // Narrow: show either the list or the details, full width.
            if (_showingDetail && _selected != null)
            {
                MasterColumn.Width = new GridLength(0);
                DetailColumn.Width = new GridLength(1, GridUnitType.Star);
                MasterPane.Visibility = Visibility.Collapsed;
                DetailPane.Visibility = Visibility.Visible;
                BackButton.Visibility = Visibility.Visible;
                UpdateDetailContent();
            }
            else
            {
                MasterColumn.Width = new GridLength(1, GridUnitType.Star);
                DetailColumn.Width = new GridLength(0);
                MasterPane.Visibility = Visibility.Visible;
                DetailPane.Visibility = Visibility.Collapsed;
                BackButton.Visibility = Visibility.Collapsed;
            }
        }
    }
}
