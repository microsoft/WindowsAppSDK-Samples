// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

namespace CsConsoleWidgetProvider
{
    internal class WeatherWidget : WidgetImplBase
    {
        public static string DefinitionId { get; } = "CSharp_Weather_Widget";
        public WeatherWidget(string widgetId, string initialState) : base(widgetId, initialState) { }

        private static string WidgetTemplate { get; set; } = "";

        private static string GetDefaultTemplate()
        {
            if (string.IsNullOrEmpty(WidgetTemplate))
            {
                WidgetTemplate = ReadPackageFileFromUri("ms-appx:///Templates/WeatherWidgetTemplate.json");
            }

            return WidgetTemplate;
        }

        public override string GetTemplateForWidget()
        {
            return GetDefaultTemplate();
        }

        public override string GetDataForWidget()
        {
            // Return empty JSON since we don't have any data that we want to use.
            return "{}";
        }
    }
}
