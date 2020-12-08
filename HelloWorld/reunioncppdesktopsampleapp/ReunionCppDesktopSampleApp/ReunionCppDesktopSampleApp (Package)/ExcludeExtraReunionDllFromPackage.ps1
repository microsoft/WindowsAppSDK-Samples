<#
The purpose of the sample is to demonstrate loading the Project Reunion dll from the Project
Reunion framework package. WinUI3 currently also includes a copy of the Project Reunion dll.
To ensure that the correct one is loaded we remove the entry from the appxreceipe so that it
is not included in the final package.
#>
[CmdLetBinding()]
Param(
    [string]$appxReceipePath
)

if ([System.IO.File]::Exists($appxReceipePath))
{
    $appxReceipeXml = [xml](get-content $appxReceipePath)
    $appxReceipeXmlNamespace = new-object Xml.XmlNamespaceManager $appxReceipeXml.NameTable
    $appxReceipeXmlNamespace.AddNamespace("docNamespace", $appxReceipeXml.DocumentElement.NamespaceURI)
    $reunionDllAppxPackagedFileElement = $appxReceipeXml.DocumentElement.SelectNodes("//docNamespace:AppxPackagedFile/docNamespace:PackagePath[.='Microsoft.ProjectReunion.dll']", $appxReceipeXmlNamespace)
    if ($reunionDllAppxPackagedFileElement.Count -eq 0)
    {
        Write-Host "AppxPackagedFile for Microsoft.ProjectReunion.dll not found. If this script is no longer needed it can be removed."
    }
    elseif ($reunionDllAppxPackagedFileElement.Count -gt 1)
    {
        Write-Host "Too many entries for AppxPackagedFile with Microsoft.ProjectReunion.dll found."
    }
    else
    {
        $elementToRemove = $reunionDllAppxPackagedFileElement.ParentNode
        $elementToRemove.ParentNode.RemoveChild($elementToRemove)

        $appxReceipeXml.save($appxReceipePath)
    }
}
else 
{
    Write-Host "file could not be found: $appxReceipePath"
}