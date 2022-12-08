// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.Windows.Widgets.Providers;
using System;
using System.Text.Json.Nodes;

namespace CsConsoleWidgetProvider
{
    internal class CountingWidget : WidgetImplBase
    {
        public static string DefinitionId { get; } = "CSharp_Counting_Widget";
        public CountingWidget(string widgetId, string startingState) : base(widgetId, startingState)
        {
            if (state == string.Empty)
            {
                state = "0";
            }
            else
            {
                // This particular widget stores its clickCount in the state as integer.
                // Attempt to parse the saved state and convert it to integer.
                uint parsedClickCount;
                if (uint.TryParse(state, out parsedClickCount))
                {
                    ClickCount = parsedClickCount;
                }
            }
        }

        public override void OnActionInvoked(WidgetActionInvokedArgs actionInvokedArgs)
        {
            if (actionInvokedArgs.Verb == "inc")
            {
                ClickCount++;
                state = ClickCount.ToString();

                var updateOptions = new WidgetUpdateRequestOptions(Id);
                updateOptions.Data = GetDataForWidget();
                updateOptions.CustomState = State;
                WidgetManager.GetDefault().UpdateWidget(updateOptions);
            }
        }

        public override void OnWidgetContextChanged(WidgetContextChangedArgs contextChangedArgs)
        {
            // (Optional) There a several things that can be done here:
            // 1. If you need to adjust template/data for the new context (i.e. widget size has chaned) - you can do it here.
            // 2. Log this call for telemetry to monitor what size users choose the most.
        }

        public override void Activate(WidgetContext widgetContext)
        {
            // Since this widget doesn't update data for any reason
            // except when 'Increment' button was clicked - 
            // there's nothing to do here. However, for widgets that
            // constantly push updates this is the signal to start
            // pushing those updates since widget is now visible.
            isActivated = true;
        }

        public override void Deactivate()
        {
            isActivated = false;
        }

        private static string GetDefaultTemplate()
        {
            if (string.IsNullOrEmpty(WidgetTemplate))
            {
                // This widget has the same template for all the sizes/themes so we load it only once.
                WidgetTemplate = ReadPackageFileFromUri("ms-appx:///Templates/CountingWidgetTemplate.json");
            }

            return WidgetTemplate;
        }

        public override string GetTemplateForWidget()
        {
            return GetDefaultTemplate();
        }

        public override string GetDataForWidget()
        {
            var stateNode = new JsonObject {
                ["count"] = State
            };
            Console.WriteLine(stateNode.ToString());
            return stateNode.ToJsonString();
        }

        private static string WidgetTemplate { get; set; } = "";

        private uint ClickCount { get; set; }
    }
}
