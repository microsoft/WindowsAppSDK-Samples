using System;
using System.Collections.Generic;
using Windows.Globalization.DateTimeFormatting;

namespace SDKTemplate
{
    // A single item of content shown in the list and the details pane.
    public class Item
    {
        public int Id { get; set; }
        public DateTime DateCreated { get; set; }
        public string Title { get; set; }
        public string Text { get; set; }
    }

    // View model wrapper that exposes a formatted timestamp for binding.
    public class ItemViewModel
    {
        public int ItemId { get; private set; }
        public string Title { get; set; }
        public string Text { get; set; }
        public DateTime DateCreated { get; set; }

        public string DateCreatedHourMinute
        {
            get
            {
                var formatter = new DateTimeFormatter("hour minute");
                return formatter.Format(DateCreated);
            }
        }

        public static ItemViewModel FromItem(Item item)
        {
            return new ItemViewModel
            {
                ItemId = item.Id,
                DateCreated = item.DateCreated,
                Title = item.Title,
                Text = item.Text,
            };
        }
    }

    // In-memory sample data. A real app would load this from a service or database.
    public static class ItemsDataSource
    {
        private static readonly List<Item> _items = new()
        {
            new Item
            {
                Id = 0,
                DateCreated = DateTime.Now,
                Title = "Item 1",
                Text = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Integer id facilisis lectus. Cras nec convallis ante, quis pulvinar tellus. Integer dictum accumsan pulvinar. Pellentesque eget enim sodales sapien vestibulum consequat.\n\nMaecenas eu sapien ac urna aliquam dictum.\n\nNullam eget mattis metus. Donec pharetra, tellus in mattis tincidunt, magna ipsum gravida nibh, vitae lobortis ante odio vel quam."
            },
            new Item
            {
                Id = 1,
                DateCreated = DateTime.Now,
                Title = "Item 2",
                Text = "Quisque accumsan pretium ligula in faucibus. Mauris sollicitudin augue vitae lorem cursus condimentum quis ac mauris. Pellentesque quis turpis non nunc pretium sagittis. Nulla facilisi. Maecenas eu lectus ante. Proin eleifend vel lectus non tincidunt. Fusce condimentum luctus nisi, in elementum ante tincidunt nec.\n\nAenean in nisl at elit venenatis blandit ut vitae lectus. Praesent in sollicitudin nunc. Pellentesque justo augue, pretium at sem lacinia, scelerisque semper erat. Ut cursus tortor at metus lacinia dapibus."
            },
            new Item
            {
                Id = 2,
                DateCreated = DateTime.Now,
                Title = "Item 3",
                Text = "Ut consequat magna luctus justo egestas vehicula. Integer pharetra risus libero, et posuere justo mattis et.\n\nProin malesuada, libero vitae aliquam venenatis, diam est faucibus felis, vitae efficitur erat nunc non mauris. Suspendisse at sodales erat.\nAenean vulputate, turpis non tincidunt ornare, metus est sagittis erat, id lobortis orci odio eget quam. Suspendisse ex purus, lobortis quis suscipit a, volutpat vitae turpis."
            },
            new Item
            {
                Id = 3,
                DateCreated = DateTime.Now,
                Title = "Item 4",
                Text = "Duis facilisis, quam ut laoreet commodo, elit ex aliquet massa, non varius tellus lectus et nunc. Donec vitae risus ut ante pretium semper. Phasellus consectetur volutpat orci, eu dapibus turpis. Fusce varius sapien eu mattis pharetra.\n\nNam vulputate eu erat ornare blandit. Proin eget lacinia erat. Praesent nisl lectus, pretium eget leo et, dapibus dapibus velit. Integer at bibendum mi, et fringilla sem."
            }
        };

        public static IList<Item> GetAllItems() => _items;

        public static Item GetItemById(int id) => _items[id];
    }
}
