---
page_type: sample
languages:
- csharp
- cpp
products:
- windows
- windows-app-sdk
name: "Custom Edit Control"
urlFragment: CustomEditControl
description: "Shows how to use the CoreTextEditContext class to create a rudimentary text control."
extendedZipContent:
- path: LICENSE
  target: LICENSE
---

# Custom Edit Control sample

Shows how to use the CoreTextEditContext class in the Windows.UI.Text.Core namespace
to create a rudimentary text control. Note that this text control is not complete;
it glosses over many details that would be necessary for a complete text edit control.

The focus of the sample is in CustomEditControl to show how to implement and manage a text control.
There is much more functionality built in to the CoreTextEditContext that is not covered in this sample.

This sample demonstrates the following:

* Managing the text and current selection of a custom edit control.
  This sample uses a simple string to record the text.
* Rendering the text and current selection in the custom edit control.
  To illustrate that the app is completely responsible for the visual
  presentation of the control, this sample takes the unusual decision
  to use a globe to represent the caret.
* Manually setting focus to and removing focus from the control
  and the CoreTextEditContext based on application-defined criteria.
  The CoreTextEditContext processes text input when it has focus.
* Setting the CoreTextEditContext.InputPaneDisplayPolicy to Manual
  and manually showing the software keyboard when the custom edit control
  gains focus and hiding it when the custom edit control loses focus.
* Responding to system events that request information about the
  custom edit control or request changes to the text and selection of
  the custom edit control.
* Responding to layout information requests so that the IME candidate window
  can be positioned properly.
* Changing the selection and/or moving the caret when the user presses
  an arrow key, and deleting text when the user presses the Backspace key.

**Instructions on using this sample**

* Click or tap on the custom edit control to give it focus,
  and click or tap outside the custom edit control to remove focus.
* Observe that the Input Pane appears (if applicable)
  when the custom edit control gains focus,
  and it disappears when the custom edit control loses focus.
* Use the arrow keys to move the caret (shown as a globe).
* Hold the shift key when pressing the arrow keys to adjust
  the selection.
* Use the Backspace key to delete text.
* To demonstrate support for IME candidates:
  * Install an IME by going to Settings, Time and Language,
    Region and language. Click "Add a language" and select
    Chinese (Simplified) "中文（中华人民共和国）".
  * Set your input language to Chinese by using the language
    selector in the bottom right corner of the taskbar (on Desktop)
    or by swiping the space bar on the software keyboard (on Mobile).
  * Put focus on the custom edit control and start typing.
    IME candidate suggestions will appear as you type.

**Features missing from this sample**

* This sample does not properly handle surrogate pairs
  or grapheme clusters.
* This sample does not support common keyboard shortcuts
  such as Home and End, nor does it support shortcuts such
  as Ctrl+V to paste.
* This sample does not show a context menu if the user right-clicks
  or performs a press-and-hold gesture.
* This sample does not support using the mouse or touch to
  move the caret or adjust the selection.

## Prerequisites

* See [System requirements for Windows app development](https://docs.microsoft.com/windows/apps/windows-app-sdk/system-requirements).
* Make sure that your development environment is set up correctly&mdash;see [Install tools for developing apps for Windows 10 and Windows 11](https://docs.microsoft.com/windows/apps/windows-app-sdk/set-up-your-development-environment).

## Building and running any of the samples

* Open the solution file (`.sln`) from the subfolder of your preferred sample in Visual Studio.
* From Visual Studio, either **Start Without Debugging** (Ctrl+F5) or **Start Debugging** (F5).

## Related Links

- [CoreTextEditContext](https://msdn.microsoft.com/library/windows/apps/windows.ui.text.core.coretexteditcontext.aspx)  
