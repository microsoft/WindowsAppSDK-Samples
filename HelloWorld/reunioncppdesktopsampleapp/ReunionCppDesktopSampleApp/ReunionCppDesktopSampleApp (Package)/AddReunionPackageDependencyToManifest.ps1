<#
This script's purpose is to jump in during the build and edit the package project appxmanifest to
include the Project Reunion <PackageDependency> entry.

Some background:
The sample Project Reunion app demonstrates the scenario where a user wants to use WinUI 3 with
Project Reunion. The WinUI 3 template has a Win32 app that is using WinUI 3 and is being referenced
by a packaging project so that the Win32 app can be distributed through the Microsoft app store.

A goal of Project Reunion is that it can update seamlessly. To achieve this, Project Reunion is
distributed as a framework package. Unlike a regular Nuget package, the advantage of a framework
package backed Nuget is that an app can reference the Nuget but not have to include and ship the
Nuget binaries along with the app. Instead, at runtime when the app needs the binaries, they are
pulled in from a Microsoft app store managed package. 

For a store app to reference a framework package it needs a <PackageDependency> entry in its
manifest that corresponds with the framework package. For a project to take a dependency on a
package that is backed by a framework package, a UWP project would pull in the Nuget and the Nuget
would then under the covers cause the UWP project to add <PackageDependency> entry to its
AppxManifest.

For something distributed through an app packge the package manifest must have the
<PackageDependency> entry. This works automatically for UWP projects referenced by packaging
projects, because the packaging project knows to look for certain things defined in the referenced
UWP project's AppxManifest. Unforunately for a Win32 project there is no AppxManifest and therefore
there is currently no way for the package project AppxManifest to know that the Win32 referenced
project has properties pulled in from a Nuget that it needs to inherit.

This script has been thrown together as a stop gap for this scenario. In the future something like
Dynamic Dependencies(https://github.com/microsoft/ProjectReunion/tree/main/specs/dynamicdependencies)
might be a solution to this scenario.
#>
[CmdLetBinding()]
Param(
    [string]$packagesDirectory,
    [string]$appxManifestPath
)

function Get-ProjectReunion-Package-Path 
{
    Param([string]$packagesDirectory)

    $projectReunionPackageDirectory = ""
    if (Test-Path $packagesDirectory)
    {
        $packageDirectories = Get-ChildItem $packagesDirectory | Where-Object { $_.PSIsContainer } | Where-Object {$_.PSChildName -match "Microsoft.ProjectReunion.[\d+].*.*"}
        if ($packageDirectories.Count -eq 1)
        {
            $projectReunionPackageDirectory = $packageDirectories[0].FullName
        }
        elseif ($packageDirectories.Count -gt 1)
        {
            Write-Host "Found more than one Project Reunion metapackage. There should only be one."
        }
        else 
        {
            Write-Host "Project Reunion metapackage was not found."
        }
    }
    else 
    {
        Write-Host "$packagesDirectory was not found."
    }

    return $projectReunionPackageDirectory
}

$projectReunionPackageDirectory = Get-ProjectReunion-Package-Path -packagesDirectory $packagesDirectory

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
    $fwpName = $identityElements[0].Attributes.GetNamedItem("Name").Value
    $fwpPublisher = $identityElements[0].Attributes.GetNamedItem("Publisher").Value
    $fwpVersion = $identityElements[0].Attributes.GetNamedItem("Version").Value

    # Create a new PackageDependency entry for the Project Reunion framework package. The PackageDependency entry that will be added will look something like:
    # <PackageDependency Name="Microsoft.ProjectReunion.0.1" MinVersion="0.12011.3010.0" Publisher="CN=Microsoft Corporation, O=Microsoft Corporation, L=Redmond, S=Washington, C=US" />
    $appxManifestXml = [xml](get-content $appxManifestPath)
    $appxManifestXmlNamespace = new-object Xml.XmlNamespaceManager $appxManifestXml.NameTable
    $appxManifestXmlNamespace.AddNamespace("docNamespace", $appxManifestXml.DocumentElement.NamespaceURI)
    $dependencies = $appxManifestXml.DocumentElement.SelectNodes("//docNamespace:Dependencies", $appxManifestXmlNamespace)

    # Check to see if an entry for this exact package is already in the AppxManifest and if it is then don't add another one.
    $reunionPackageDependencyElement = $dependencies.SelectNodes("//docNamespace:PackageDependency[@Name=`"$fwpName`"]", $appxManifestXmlNamespace)
    if (-not $reunionPackageDependencyElement)
    {
        $reunionPackageDependencyElement = $appxManifestXml.CreateElement("PackageDependency", $appxManifestXml.DocumentElement.NamespaceURI)
        $dependencies[0].AppendChild($reunionPackageDependencyElement);

        $reunionPackageDependencyElement.SetAttribute("Name", $fwpName)
        $reunionPackageDependencyElement.SetAttribute("MinVersion", $fwpVersion)
        $reunionPackageDependencyElement.SetAttribute("Publisher", $fwpPublisher)

        $appxManifestXml.save($appxManifestPath)
    }
}
