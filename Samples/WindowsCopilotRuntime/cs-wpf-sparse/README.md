# Sparse Package with WPF

A sparse package allows unpackaged win32 apps to gain package identity without fully
adopting the MSIX packaging format. This is particularly useful for applications that are not yet
ready to have all their content inside an MSIX package but still need to use Windows extensibility
features that require package identity.

# cs-wpf-sparse Sample

This folder contains the cs-wpf-sparse sample which uses sparse package built on top of the WPF
platform using WCR. BuildSparsePackage.ps1 is provided for the user which closely follows the steps
from the link below

https://learn.microsoft.com/en-us/windows/apps/desktop/modernize/grant-identity-to-nonpackaged-apps#add-the-package-identity-metadata-to-your-desktop-application-manifest

BuildSparsePackage.ps1 does the following:
1) Build the solution in the desired platform and configuration provided in the parameters
 
2) Run MakeAppx with the /nv option on the folder containing the AppxManifest
    (cs-wpf-sparse\WCRforWPF\AppxManifest.xml) 
    
    - The /nv flag is required to bypass validation of referenced file paths in the manifest. 
    
    - The output folder is set to the output of the binaries when we build the solution regularly
    (bin\$Platform\$Configuration\net8.0-windows10.0.22621.0) so that the MSIX will be side by side
    to the executable built in step 1. This placement is necessary because the exe expects the external location of
    the MSIX to be the same as it's file location.

3) Run SignTool to sign the MSIX

# Before Running

Please have the WindowsAppSDK runtime installed using the installer for 1.7.250127003-experimental3
at https://learn.microsoft.com/en-us/windows/apps/windows-app-sdk/downloads

Please install the cert in the .user folder to LocalMachine TrustedPeople root. You can either
launch from the start menu after installing the MSIX in the bin directory or you can directly run
the exe. 


# Initialization code

Just doing the steps above does not yet grant the app package identity. The MSIX package still needs to
be registered with the external location. 

The registerSparsePackage() function in cs-wpf-sparse\WCRforWPF\App.xaml.cs does this for us. This
process is described in the documentation linked above. This will give the app process identity if
ran from the start menu. However, directly running the executable in the external location still
does not give the process package identity. This difference in behavior is not noted in the article
from above.

To allow the resulting app process from directly running the executable to gain identity requires
this workaround. 

The Startup() code has two different codepaths for whether the process is a packaged or a unpackaged
process (With or without identity).

If without Identity, registerSparsePackage() will be called which registers the package on the
external path if it hasn't been installed already. Afterwards, RunWithIdentity() will call
ActivateApplication on the package identity it registered earlier using the ActivationManager.

This will spawn a packaged process of itself which will have identity. The non-identity process will
then exit. The new packaged process in the startup will hit the codepath for when IsPackageProcess()
is true and launch MainWindow. 

Note: the first run of the exe run will require a longer startup time because of the time that is
needed to register the package. 