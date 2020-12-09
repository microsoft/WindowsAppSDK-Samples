<#
The purpose of the sample is to demonstrate loading the Project Reunion dll from the Project
Reunion framework package. WinUI3 currently also includes a copy of the Project Reunion dll.
To ensure that the correct one is loaded we remove the entry from the appxrecipe so that it
is not included in the final package.
#>
[CmdLetBinding()]
Param(
    [string]$appxRecipePath
)

if ([System.IO.File]::Exists($appxRecipePath))
{
    $appxRecipeXml = [xml](get-content $appxRecipePath)
    $appxRecipeXmlNamespace = new-object Xml.XmlNamespaceManager $appxRecipeXml.NameTable
    $appxRecipeXmlNamespace.AddNamespace("docNamespace", $appxRecipeXml.DocumentElement.NamespaceURI)
    $reunionDllAppxPackagedFileElement = $appxRecipeXml.DocumentElement.SelectNodes("//docNamespace:AppxPackagedFile/docNamespace:PackagePath[.='Microsoft.ProjectReunion.dll']", $appxRecipeXmlNamespace)
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

        $appxRecipeXml.save($appxRecipePath)
    }
}
else 
{
    Write-Host "file could not be found: $appxRecipePath"
}