#define LoadWinAppSDKUsingWin11Api

using System;
using System.Diagnostics;
//using Microsoft.Windows.AIFabricUtils;

namespace Notes
{
    // public class Initialize
    // {
    //     public static void AssemblyInitialize()
    //     {
    //         bool win11 = Environment.OSVersion.Version.Build >= 22000;
    //         if (!win11)
    //         {
    //             throw new Exception("Win11 or better required.");
    //         }

    //         var minVersion = new global::Windows.ApplicationModel.PackageVersion(0, 2, 0, 0);

    //         // Add client API package
    //         Debug.WriteLine("Adding dynamic dependency: " + NameProvider.ClientPkgName + ", version " + minVersion);
    //         AddDynamicDependency(NameProvider.ClientPkgName, minVersion);

    //         // Add mock package
    //         Debug.WriteLine("Adding dynamic dependency: " + NameProvider.ClientPkgName + ", version " + minVersion);
    //         AddDynamicDependency(NameProvider.MockPkgName, minVersion);
    //     }

    //     public static void AddDynamicDependency(string packageFamilyName, global::Windows.ApplicationModel.PackageVersion minVersion)
    //     {
    //         Debug.WriteLine($"Using AI Fabric package family name: {packageFamilyName}");
    //         PackageUtils.AddPackageDependency(packageFamilyName, minVersion);
    //     }
    // }
}
