using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CsWinUiDesktopInstancing
{
    internal static class ActivationTrackerHelper
    {
        private const string c_redirectionKeyName = @"SOFTWARE\AppLifecycleExample";
        private const string c_redirectionValueName = "NumberOfActivations";

        private static RegistryKey GetRegistryKey()
        {
            return Registry.CurrentUser.CreateSubKey(c_redirectionKeyName, true);
        }

        private static void SetNumberOfActivations(int newNumberOfActivations)
        {
            RegistryKey instanceTrackerKey = GetRegistryKey();
            instanceTrackerKey.SetValue(c_redirectionValueName, newNumberOfActivations);
        }

        internal static int IncrementAndSetNumberOfRedirectionsReturnNewNumber()
        {
            int numberOfActivations = GetNumberOfActivations();
            numberOfActivations++;
            SetNumberOfActivations(numberOfActivations);
            return numberOfActivations;
        }

        internal static int GetNumberOfActivations()
        {
            int numberOfActivations = 0;
            RegistryKey instanceTrackerKey = GetRegistryKey();

            object numberOfActivationsAsObject = instanceTrackerKey.GetValue(c_redirectionValueName);
            if (numberOfActivationsAsObject == null)
            {
                instanceTrackerKey.SetValue(c_redirectionValueName, numberOfActivations);
            }
            else
            {
                numberOfActivations = Convert.ToInt32(numberOfActivationsAsObject);
            }

            return numberOfActivations;
        }
    }
}
