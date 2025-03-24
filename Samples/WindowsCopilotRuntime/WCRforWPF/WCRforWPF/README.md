# DotNet CLI with WCRforWPF

Because DotNet CLI is incompatible with WAPProj, we must first convert the WAPProj to a
SingleProjectMSIX This project was converted from a WAPProj to SingleProjectMSIX using the steps
below https://learn.microsoft.com/en-us/windows/apps/windows-app-sdk/single-project-msix?tabs=csharp

Summary of the steps done:
- Add EnableMsixTooling and PublishProfile properties to WCRforWPF.csproj
    ```
    <EnableMsixTooling>true</EnableMsixTooling>
    <PublishProfile>Properties\PublishProfiles\win10-$(Platform).pubxml</PublishProfile>
    <GenerateAppxPackageOnBuild>true</GenerateAppxPackageOnBuild>
    ```

- Add WCRforWPF\Properties\launchSettings.json

- Add /Properties/PublishProfiles/win10-$(platform).pubxml and 

- Move Package.appxmanifest and files under images from the WAPProj to WCRforWPF.csproj and remove
  the WAPProj.

- In configuration manager, check the deploy box in each of the configuration

# Building with DotNetCLI

In the command line, cd to the root of the solution and run
```
runDotNetBuild.cmd
```

This will build and generate the MSIX with a self-signed cert which you will need to install to
LocalMachine TrustedPeople Certificate Store
