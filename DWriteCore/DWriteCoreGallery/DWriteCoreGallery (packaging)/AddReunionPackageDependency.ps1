<#
This script's purpose is to jump in during the build and do what having AppxPackageRegistration
in the project file would do. It edits the package project appxmanifest to include the Project
Reunion <PackageDependency> entry and adds a <ResolvedSDKReference> entry to the appx recipe.

Some background:
A goal of Project Reunion is that it can update seamlessly. To achieve this, Project Reunion is
distributed as a framework package. Unlike a regular Nuget package, the advantage of a framework
package backed Nuget is that an app can reference the Nuget but not have to include and ship the
Nuget binaries along with the app. Instead, at runtime when the app needs the binaries, they are
pulled in from a Microsoft app store managed package.

The current flow for a UWP to get the dependency for the reunion package is as follows:
The UWP project includes the Reunion Nuget package. In the Reunion Nuget there is a props file that
has <AppxPackageRegistration> entry. This AppxPackageRegistration points at a local copy of the
framework package appx. When AppxPackageRegistration is encountered by the build, it does two things:

1) It looks in the framework appx at the package information and saves it off to be added as a
<PackageDependency> in the final AppxManifest. Having <PackageDependency> in the AppxManifest allows
binaries from that package to be loaded when needed at runtime.

2) It adds a <ResolvedSDKReference> entry to the appxrecipe. This <ResolvedSDKReference> tells
Visual Studio to make sure that the package reference by this entry is installed. Having this entry
ensures that when running a project from Visual Studio the framework package will get installed and
the developer will not need to install it manually or rely on the store.

The AppxPackageRegistration entry works automatically for UWP projects. When a UWP projects are
referenced by package project, the package project knows to look at relevant appx metadata from
the referenced UWP and include it in the package project final appx metadata. Unfortunately this
will not work for a Win32 app that is referenced by a package project because the Win32 app does
not have associated appx data and thus the package project does not have anything to "inherit".

This script has been thrown together as a stop gap for this scenario. In the future something like
Dynamic Dependencies(https://github.com/microsoft/ProjectReunion/tree/main/specs/dynamicdependencies)
might be a solution to this scenario.
#>
[CmdLetBinding()]
Param(
    [string]$solutionDirectory,
    [string]$appxManifestPath,
    [string]$architecture,
    [string]$appxRecipePath
)

$projectReunionPackageId = "Microsoft.ProjectReunion"

function Get-ProjectReunion-Package-Path
{
    # Get the correct Project Reunion package folder. If the user did something like a Nuget update
    # then there could be other version of the Project Reunion package. We can find the right one by
    # consulting packages.config files.
    $projectReunionVersion = ""
    $packageConfigFiles = Get-ChildItem -Path $solutionDirectory -Recurse | Where-Object { $_.FullName -match "packages.config"}

    # For now, for simplicity, we are assuming there is only 1 Win32 packaged project and thus only
    # 1 packages.config file we need to parse.
    if ($packageConfigFiles.Count -eq 1)
    {
        $xml = [xml](get-content $packageConfigFiles[0].FullName)
        $xmlNamespace = new-object Xml.XmlNamespaceManager $xml.NameTable
        $xmlNamespace.AddNamespace("docNamespace", $xml.DocumentElement.NamespaceURI)
        $projectReunionPackageElement = $xml.DocumentElement.SelectNodes("//docNamespace:package[@id='$projectReunionPackageId']", $xmlNamespace)
        $projectReunionVersion = $projectReunionPackageElement.Version
    }
    elseif ($packageConfigFiles.Count -gt 1)
    {
        Write-Host "Samples are assume to have one Win32 package project if they use this script. If this is no longer true, please update this script."
    }
    else
    {
        Write-Host "No packages.config files were found."
    }

    if ($projectReunionVersion)
    {
        $packagesDirectory = $solutionDirectory + "packages\"
        if (Test-Path $packagesDirectory)
        {
            $projectReunionPackageDirectory = "$packagesDirectory" + "$projectReunionPackageId.$projectReunionVersion"
            if (Test-Path $projectReunionPackageDirectory)
            {
                return $projectReunionPackageDirectory
            }
            else
            {
                Write-Host "Project Reunion metapackage was not found at location: $projectReunionPackageDirectory"
            }
        }
        else
        {
            Write-Host "$packagesDirectory was not found."
        }
    }

    return ""
}

function Add-Framework-Package-Registration-Entry
{
    Param(
        [string]$projectReunionPackageDirectory,
        [string]$fwpName,
        [string]$fwpPublisher,
        [string]$fwpVersion
    )

    if ([System.IO.File]::Exists($appxRecipePath))
    {
        # Assuming that the only appx in this package is the framework package, find the reference to it.
        $propsAndTargets = Get-ChildItem -Path $projectReunionPackageDirectory -Recurse | Where-Object { $_.FullName.EndsWith(".props") -or $_.FullName.EndsWith(".targets")}
        $pathToAppx = ""
        foreach($file in $propsAndTargets)
        {
            $xml = [xml](get-content $file.FullName)
            $xmlNamespace = new-object Xml.XmlNamespaceManager $xml.NameTable
            $xmlNamespace.AddNamespace("docNamespace", $xml.DocumentElement.NamespaceURI)
            $architetureElement = $xml.DocumentElement.SelectNodes("//docNamespace:AppxPackageRegistration/docNamespace:Architecture[text()='$architecture']", $xmlNamespace)
            if ($architetureElement.Count -eq 1)
            {
                $appxPackageRegistrationElement = $architetureElement.ParentNode
                $includeAttribute = $appxPackageRegistrationElement.Include

                # Remove $(MSBuildThisFileDirectory) because this is something we cannot properly
                # interpret in this context but we know it usually translates to the 'the current
                # file directory'
                $relativePath = $includeAttribute.Replace('$(MSBuildThisFileDirectory)','')
                $pathToAppx = $file.Directory.FullName + "\" + $relativePath

                break;
            }
        }

        # Once we have the path to the Reunion framework appx, add it into the package project appx recipe.
        if ($pathToAppx)
        {
            $xml = [xml](get-content $appxRecipePath)
            $xmlNamespace = new-object Xml.XmlNamespaceManager $xml.NameTable
            $xmlNamespace.AddNamespace("docNamespace", $xml.DocumentElement.NamespaceURI)

            # Add a ResolvedSDKReference entry but only if there are no entries that already point to appx of the same name.
            $appxFileName = [System.IO.Path]::GetFileName($pathToAppx)
            $reunionResolvedSDKReferenceElement = $xml.DocumentElement.SelectNodes("//docNamespace:ResolvedSDKReference/docNamespace:AppxLocation[contains(text(),`"$appxFileName`")]", $xmlNamespace)
            if ($reunionResolvedSDKReferenceElement.Count -eq 0)
            {
                # Find the ItemGroup that has the other ResolvedSDKReference entries.
                $parentElement = $xml.CreateElement("ItemGroup", $xml.DocumentElement.NamespaceURI)
                $resolvedSDKReferenceElements = $xml.DocumentElement.SelectNodes("//docNamespace:ItemGroup/docNamespace:ResolvedSDKReference", $xmlNamespace)
                if ($resolvedSDKReferenceElements.Count -gt 0)
                {
                    $parentElement = $resolvedSDKReferenceElements[0].ParentNode
                }
                else
                {
                    $xml.DocumentElement.AppendChild($parentElement)
                }

                <#
                    The following will add a ResolvedSDKReference entry for Reunion that will look like:
                        <ResolvedSDKReference Include="$pathToAppx">
                        <Name>$fwpName</Name>
                        <Version>$fwpVersion</Version>
                        <Architecture>$architecture</Architecture>
                        <FrameworkIdentity>$frameworkIdentity</FrameworkIdentity>
                        <AppxLocation>$pathToAppx</AppxLocation>
                        </ResolvedSDKReference>
                #>
                $reunionResolvedSDKReferenceElement = $xml.CreateElement("ResolvedSDKReference", $xml.DocumentElement.NamespaceURI)
                $parentElement.AppendChild($reunionResolvedSDKReferenceElement)
                $reunionResolvedSDKReferenceElement.SetAttribute("Include", $pathToAppx)

                $nameElement = $xml.CreateElement("Name", $xml.DocumentElement.NamespaceURI)
                $reunionResolvedSDKReferenceElement.AppendChild($nameElement)
                $nameElement.InnerText = $fwpName

                $versionElement = $xml.CreateElement("Version", $xml.DocumentElement.NamespaceURI)
                $reunionResolvedSDKReferenceElement.AppendChild($versionElement)
                $versionElement.InnerText = $fwpVersion

                $architectureElement = $xml.CreateElement("Architecture", $xml.DocumentElement.NamespaceURI)
                $reunionResolvedSDKReferenceElement.AppendChild($architectureElement)
                $architectureElement.InnerText = $architecture

                $frameworkIdentityElement = $xml.CreateElement("FrameworkIdentity", $xml.DocumentElement.NamespaceURI)
                $reunionResolvedSDKReferenceElement.AppendChild($frameworkIdentityElement)
                # Here we are escaping the ' character. It looks like the convention is to have this
                # character around the publisher string.
                $frameworkIdentity = "Name = $fwpName, MinVersion = $fwpVersion, Publisher = %27$fwpPublisher%27"
                $frameworkIdentityElement.InnerText = $frameworkIdentity

                $appxLocationElement = $xml.CreateElement("AppxLocation", $xml.DocumentElement.NamespaceURI)
                $reunionResolvedSDKReferenceElement.AppendChild($appxLocationElement)
                $appxLocationElement.InnerText = $pathToAppx

                $xml.save($appxRecipePath)
            }
        }
        else
        {
            Write-Host "Reunion framework package appx entry was not found."
        }
    }
    else
    {
        Write-Host "file could not be found: $appxRecipePath"
    }
}

$projectReunionPackageDirectory = Get-ProjectReunion-Package-Path

if ($projectReunionPackageDirectory)
{
    Add-Type -AssemblyName System.IO.Compression.FileSystem
    $fwpAppxFiles = Get-ChildItem -Path "$projectReunionPackageDirectory\tools\AppX" -Recurse | Where-Object { $_.FullName.EndsWith(".appx") }

    # We assume that all the appx packages have the same identity attributes for all the platforms so just use the first one.
    $fwpAppxManifest = [System.IO.Compression.ZipFile]::OpenRead($fwpAppxFiles[0].FullName).Entries | Where-Object { $_.FullName -match 'AppxManifest.xml' }

    # Read the AppxManifest from the appx package and get the identity details we need for the PackageDependency entry.
    $stream = $fwpAppxManifest.Open()
    $reader = New-Object IO.StreamReader($stream)
    $text = $reader.ReadToEnd()
    $fwpAppxManifestXml = [xml]($text)
    $fwpAppxManifestXmlNamespace = new-object Xml.XmlNamespaceManager $fwpAppxManifestXml.NameTable
    $fwpAppxManifestXmlNamespace.AddNamespace("docNamespace", $fwpAppxManifestXml.DocumentElement.NamespaceURI)
    $identityElements = $fwpAppxManifestXml.DocumentElement.SelectNodes("//docNamespace:Identity", $fwpAppxManifestXmlNamespace)
    $fwpName = $identityElements[0].Name
    $fwpPublisher = $identityElements[0].Publisher
    $fwpVersion = $identityElements[0].Version

    # Create a new PackageDependency entry for the Project Reunion framework package. The PackageDependency entry that will be added will look something like:
    # <PackageDependency Name="Microsoft.ProjectReunion.0.1" MinVersion="0.12011.3010.0" Publisher="CN=Microsoft Corporation, O=Microsoft Corporation, L=Redmond, S=Washington, C=US" />
    $appxManifestXml = [xml](get-content $appxManifestPath)
    $appxManifestXmlNamespace = new-object Xml.XmlNamespaceManager $appxManifestXml.NameTable
    $appxManifestXmlNamespace.AddNamespace("docNamespace", $appxManifestXml.DocumentElement.NamespaceURI)
    $dependencies = $appxManifestXml.DocumentElement.SelectNodes("//docNamespace:Dependencies", $appxManifestXmlNamespace)

    # Check to see if an entry for this exact package is already in the AppxManifest and if it is then don't add another one.
    $reunionPackageDependencyElement = $dependencies.SelectNodes("//docNamespace:PackageDependency[@Name=`"$fwpName`"]", $appxManifestXmlNamespace)
    if ($reunionPackageDependencyElement.Count -eq 0)
    {
        $reunionPackageDependencyElement = $appxManifestXml.CreateElement("PackageDependency", $appxManifestXml.DocumentElement.NamespaceURI)
        $dependencies[0].AppendChild($reunionPackageDependencyElement);

        $reunionPackageDependencyElement.SetAttribute("Name", $fwpName)
        $reunionPackageDependencyElement.SetAttribute("MinVersion", $fwpVersion)
        $reunionPackageDependencyElement.SetAttribute("Publisher", $fwpPublisher)

        $appxManifestXml.save($appxManifestPath)
    }

    Add-Framework-Package-Registration-Entry -projectReunionPackageDirectory $projectReunionPackageDirectory -fwpName $fwpName -fwpPublisher $fwpPublisher -fwpVersion $fwpVersion
}
