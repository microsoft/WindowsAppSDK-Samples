---
name: "App Notifications Sample" 
page_type: sample
languages:
- cpp
products: 
- windows-app-sdk
description: "Demonstrates how to use the windows app SDK App Notifications APIs from an unpackaged WinUI app"
urlFragment: Notifications/App
---
# App Notifications Sample

## Code Organization 
**How is the code organized?**

* Philosophy
    * Scenarios
    * non-persistent
* Organization
* Processing
    * Foreground
    * Background
* Activation
    * Background
    * Foreground
* Future
    * Scenarios
    * Roadmap
        * Releases
            * Preview3 - May 3rd
            * Stable - May 24th
        * Fork
            * Stable - May 3rd
    * Background Activation
        * Potential Issues

**Which scenarios will demonstrate background activation?**

*Given the uncertainty around the background activation functionality,*

**What scenarios are being implemented for the first iteration of the sample?**

**

**What are the upcoming scenarios to be implemented before GA?**

**

**Why are foreground and background tags ignored in the sample?**

*The sample sets `activationType = \"foreground\" />\` or `activationType = \"background\" />\` but these tags don't seem to have any effect on how a notifiacation is handled.*

**What is the argument entry in the action?**

*For example, how is this supposed to be interpreted: `arguments = \"action=activateToast&amp;contentId=351\"\`?*

**How are received notifications routed in the sample?**

*The samples contains multiple scenario classes, each able to send notifications and also able to process the notifications it sent. How does the sample knows to which scenario to route a specific notification?*

The sample uses the contentId field part of the s to identify to which scenario a specific toast should be routed.