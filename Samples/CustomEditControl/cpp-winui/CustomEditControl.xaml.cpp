// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "CustomEditControl.xaml.h"
#if __has_include("CustomEditControl.g.cpp")
#include "CustomEditControl.g.cpp"
#endif

#include "InputPaneInterop.h"

namespace winrt
{
    using namespace ::winrt::Microsoft::UI::Input;
    using namespace ::winrt::Microsoft::UI::Windowing;
    using namespace ::winrt::Microsoft::UI::Xaml;
    using namespace ::winrt::Microsoft::UI::Xaml::Documents;
    using namespace ::winrt::Microsoft::UI::Xaml::Input;
    using namespace ::winrt::Microsoft::UI::Xaml::Media;
    using namespace ::winrt::Windows::Foundation;
    using namespace ::winrt::Windows::Graphics::Display;
    using namespace ::winrt::Windows::System;
    using namespace ::winrt::Windows::UI;
    using namespace ::winrt::Windows::UI::Core;
    using namespace ::winrt::Windows::UI::Text::Core;
    using namespace ::winrt::Windows::UI::ViewManagement;
}

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

// https://docs.microsoft.com/en-us/windows/apps/desktop/modernize/advanced-scenarios-xaml-islands-cpp

namespace winrt::CustomEditControlWinAppSDK::implementation
{
    CustomEditControl::CustomEditControl()
    {
        InitializeComponent();

        // The CoreTextEditContext processes text input, but other keys are
        // the app's responsibility.
        // Create a CoreTextEditContext for our custom edit control.
        winrt::CoreTextServicesManager manager = winrt::CoreTextServicesManager::GetForCurrentView();
        _editContext = manager.CreateEditContext();

        // For demonstration purposes, this sample sets the Input Pane display policy to Manual
        // so that it can manually show the software keyboard when the control gains focus and
        // dismiss it when the control loses focus. If you leave the policy as Automatic, then
        // the system will hide and show the Input Pane for you. Note that on Desktop, you will
        // need to implement the UIA text pattern to get expected automatic behavior.
        _editContext.InputPaneDisplayPolicy(winrt::CoreTextInputPaneDisplayPolicy::Manual);

        // Set the input scope to Text because this text box is for any text.
        // This also informs software keyboards to show their regular
        // text entry layout.  There are many other input scopes and each will
        // inform a keyboard layout and text behavior.
        _editContext.InputScope(winrt::CoreTextInputScope::Text);

        // The system raises this event to request a specific range of text.
        _tokenTextRequested = _editContext.TextRequested({ this, &CustomEditControl::EditContext_TextRequested });

        // The system raises this event to request the current selection.
        _tokenSelectionRequested = _editContext.SelectionRequested({ this, &CustomEditControl::EditContext_SelectionRequested });

        // The system raises this event when it wants the edit control to remove focus.
        _tokenFocusRemoved = _editContext.FocusRemoved({ this, &CustomEditControl::EditContext_FocusRemoved });

        // The system raises this event to update text in the edit control.
        _tokenTextUpdating = _editContext.TextUpdating({ this, &CustomEditControl::EditContext_TextUpdating });

        // The system raises this event to change the selection in the edit control.
        _tokenSelectionUpdating = _editContext.SelectionUpdating({ this, &CustomEditControl::EditContext_SelectionUpdating });

        // The system raises this event when it wants the edit control
        // to apply formatting on a range of text.
        _tokenFormatUpdating = _editContext.FormatUpdating({ this, &CustomEditControl::EditContext_FormatUpdating });

        // The system raises this event to request layout information.
        // This is used to help choose a position for the IME candidate window.
        _tokenLayoutRequested = _editContext.LayoutRequested({ this, &CustomEditControl::EditContext_LayoutRequested });

        // The system raises this event to notify the edit control
        // that the string composition has started.
        _tokenCompositionStarted = _editContext.CompositionStarted({ this, &CustomEditControl::EditContext_CompositionStarted });

        // The system raises this event to notify the edit control
        // that the string composition is finished.
        _tokenCompositionCompleted = _editContext.CompositionCompleted({ this, &CustomEditControl::EditContext_CompositionCompleted });

        // The system raises this event when the NotifyFocusLeave operation has
        // completed. Our sample does not use this event.
        // _editContext.NotifyFocusLeaveCompleted += EditContext_NotifyFocusLeaveCompleted;

        // Set our initial UI.
        UpdateTextUI();
        UpdateFocusUI();
    }

    CustomEditControl::~CustomEditControl()
    {
        RemoveInternalFocus();
    }

    static winrt::Rect GetElementRect(winrt::FrameworkElement element)
    {
        winrt::GeneralTransform transform = element.TransformToVisual(nullptr);
        winrt::Point point = transform.TransformPoint(winrt::Point());
        return winrt::Rect(point, winrt::Size((float)element.ActualWidth(), (float)element.ActualHeight()));
    }

#pragma region Focus_management

    void CustomEditControl::OnGotFocus(winrt::RoutedEventArgs args)
    {
        SetInternalFocus();
    }

    void CustomEditControl::OnLostFocus(winrt::RoutedEventArgs args)
    {
        RemoveInternalFocus();
    }

    void CustomEditControl::SetAppWindow(winrt::Microsoft::UI::WindowId windowId)
    {
        _appWindowId = windowId;

        HWND hwnd = GetWindowFromWindowId(windowId);
        if (hwnd)
        {
            // Get the Input Pane so we can programmatically hide and show it.
            _inputPane = wil::capture_interop<winrt::InputPane>(&IInputPaneInterop::GetForWindow, hwnd);
        }
    }

    void CustomEditControl::SetInternalFocus()
    {
        if (!_internalFocus)
        {
            // Update internal notion of focus.
            _internalFocus = true;

            // Update the UI.
            UpdateTextUI();
            UpdateFocusUI();

            // Notify the CoreTextEditContext that the edit context has focus,
            // so it should start processing text input.
            _editContext.NotifyFocusEnter();
        }

        // Ask the software keyboard to show.  The system will ultimately decide if it will show.
        // For example, it will not show if there is a keyboard attached.
        _inputPane.TryShow();
    }

    void CustomEditControl::RemoveInternalFocus()
    {
        if (_internalFocus)
        {
            //Notify the system that this edit context is no longer in focus
            _editContext.NotifyFocusLeave();

            RemoveInternalFocusWorker();
        }
    }

    void CustomEditControl::RemoveInternalFocusWorker()
    {
        // Update the internal notion of focus
        _internalFocus = false;

        // Ask the software keyboard to dismiss.
        _inputPane.TryHide();

        // Update our UI.
        UpdateTextUI();
        UpdateFocusUI();
    }

    void CustomEditControl::EditContext_FocusRemoved(CoreTextEditContext sender, winrt::IInspectable args)
    {
        RemoveInternalFocusWorker();
    }

#pragma endregion Focus_management

#pragma region Text_management

    // Replace the text in the specified range.
    void CustomEditControl::ReplaceText(winrt::CoreTextRange modifiedRange, winrt::hstring text)
    {
        // Modify the internal text store.
        _text = _text.substr(0, modifiedRange.StartCaretPosition) +
            text +
            _text.substr(modifiedRange.EndCaretPosition);

        // Move the caret to the end of the replacement text.
        _selection.StartCaretPosition = modifiedRange.StartCaretPosition + text.size();
        _selection.EndCaretPosition = _selection.StartCaretPosition;

        // Update the selection of the edit context.  There is no need to notify the system
        // of the selection change because we are going to call NotifyTextChanged soon.
        SetSelection(_selection);

        // Let the CoreTextEditContext know what changed.
        _editContext.NotifyTextChanged(modifiedRange, text.size(), _selection);
    }

    bool CustomEditControl::HasSelection()
    {
        return _selection.StartCaretPosition != _selection.EndCaretPosition;
    }

    // Change the selection without notifying CoreTextEditContext of the new selection.
    void CustomEditControl::SetSelection(winrt::CoreTextRange selection)
    {
        // Modify the internal selection.
        _selection = selection;

        // Update the UI to show the new selection.
        UpdateTextUI();
    }

    // Change the selection and notify CoreTextEditContext of the new selection.
    void CustomEditControl::SetSelectionAndNotify(winrt::CoreTextRange selection)
    {
        SetSelection(selection);
        _editContext.NotifySelectionChanged(_selection);
    }

    // Return the specified range of text. Note that the system may ask for more text
    // than exists in the text buffer.
    void CustomEditControl::EditContext_TextRequested(winrt::CoreTextEditContext sender, winrt::CoreTextTextRequestedEventArgs args)
    {
        winrt::CoreTextTextRequest request = args.Request();
        request.Text(winrt::hstring(_text.substr(
            request.Range().StartCaretPosition,
            min(request.Range().EndCaretPosition, static_cast<int32_t>(_text.size())) - request.Range().StartCaretPosition)));
    }

    // Return the current selection.
    void CustomEditControl::EditContext_SelectionRequested(winrt::CoreTextEditContext sender, winrt::CoreTextSelectionRequestedEventArgs args)
    {
        winrt::CoreTextSelectionRequest request = args.Request();
        request.Selection(_selection);
    }

    void CustomEditControl::EditContext_TextUpdating(winrt::CoreTextEditContext sender, winrt::CoreTextTextUpdatingEventArgs args)
    {
        winrt::CoreTextRange range = args.Range();
        winrt::hstring newText = args.Text();
        winrt::CoreTextRange newSelection = args.NewSelection();

        // Modify the internal text store.
        _text = _text.substr(0, range.StartCaretPosition) +
            newText +
            _text.substr(min(static_cast<int32_t>(_text.size()), range.EndCaretPosition));

        // You can set the proper font or direction for the updated text based on the language by checking
        // args.InputLanguage. We will not do that in this sample.

        // Modify the current selection.
        newSelection.EndCaretPosition = newSelection.StartCaretPosition;

        // Update the selection of the edit context. There is no need to notify the system
        // because the system itself changed the selection.
        SetSelection(newSelection);
    }

    void CustomEditControl::EditContext_SelectionUpdating(winrt::CoreTextEditContext sender, winrt::CoreTextSelectionUpdatingEventArgs args)
    {
        // Set the new selection to the value specified by the system.
        CoreTextRange range = args.Selection();

        // Update the selection of the edit context. There is no need to notify the system
        // because the system itself changed the selection.
        SetSelection(range);
    }

#pragma endregion Text_management

#pragma region Formatting_and_layout

    void CustomEditControl::EditContext_FormatUpdating(winrt::CoreTextEditContext sender, winrt::CoreTextFormatUpdatingEventArgs args)
    {
        // The following code specifies how you would apply any formatting to the specified range of text
        // For this sample, we do not make any changes to the format.

        // Apply text color if specified.
        // A null value indicates that the default should be used.
        if (args.TextColor() != nullptr)
        {
            // InternalSetTextColor(args.Range(), args.TextColor().Value());
        }
        else
        {
            // InternalSetDefaultTextColor(args.Range());
        }

        // Apply background color if specified.
        // A null value indicates that the default should be used.
        if (args.BackgroundColor() != nullptr)
        {
            // InternalSetBackgroundColor(args.Range(), args.BackgroundColor().Value());
        }
        else
        {
            // InternalSetDefaultBackgroundColor(args.Range());
        }

        // Apply underline if specified.
        // A null value indicates that the default should be used.
        if (args.UnderlineType() != nullptr)
        {
            // TextDecoration underline = new TextDecoration(args.Range(), args.UnderlineType.Value(), args.UnderlineColor.Value());

            // InternalAddTextDecoration(underline);
        }
        else
        {
            // InternalRemoveTextDecoration(args.Range());
        }
    }

    static Rect ScaleRect(Rect rect, float scale)
    {
        rect.X *= scale;
        rect.Y *= scale;
        rect.Width *= scale;
        rect.Height *= scale;
        return rect;
    }

    void CustomEditControl::EditContext_LayoutRequested(winrt::CoreTextEditContext sender, winrt::CoreTextLayoutRequestedEventArgs args)
    {
        winrt::CoreTextLayoutRequest request = args.Request();

        // Get the screen coordinates of the entire control and the selected text.
        // This information is used to position the IME candidate window.

        // First, get the coordinates of the edit control and the selection
        // relative to the Window.

        Rect contentRect = GetElementRect(ContentPanel());
        Rect selectionRect = GetElementRect(SelectionText());

        // Next, convert to screen coordinates in view pixels.
        winrt::AppWindow appWindow = winrt::AppWindow::GetFromWindowId(_appWindowId);
        contentRect.X += appWindow.Position().X;
        contentRect.Y += appWindow.Position().Y;
        selectionRect.X += appWindow.Position().X;
        selectionRect.Y += appWindow.Position().Y;

        // Finally, scale up to raw pixels.
        float scaleFactor = static_cast<float>(XamlRoot().RasterizationScale());

        contentRect = ScaleRect(contentRect, scaleFactor);
        selectionRect = ScaleRect(selectionRect, scaleFactor);

        // This is the bounds of the selection.
        // Note: If you return bounds with 0 width and 0 height, candidates will not appear while typing.
        request.LayoutBounds().TextBounds(selectionRect);

        // This is the bounds of the whole control
        request.LayoutBounds().ControlBounds(contentRect);
    }

#pragma endregion Formatting_and_layout

#pragma region Input

    // This indicates that an IME has started composition.  If there is no handler for this event,
    // then composition will not start.
    void CustomEditControl::EditContext_CompositionStarted(winrt::CoreTextEditContext sender, winrt::CoreTextCompositionStartedEventArgs args)
    {
    }

    void CustomEditControl::EditContext_CompositionCompleted(winrt::CoreTextEditContext sender, winrt::CoreTextCompositionCompletedEventArgs args)
    {
    }

    void CustomEditControl::OnCharacterReceived(winrt::Microsoft::UI::Xaml::Input::CharacterReceivedRoutedEventArgs args)
    {
        // Do not process keyboard input if the custom edit control does not
        // have focus.
        if (!_internalFocus)
        {
            return;
        }

        if (isprint(args.Character()))
        {
            ReplaceText(_selection, winrt::to_hstring(args.Character()));
            UpdateTextUI();
        }        
    }

    void CustomEditControl::OnKeyDown(winrt::KeyRoutedEventArgs args)
    {
        // Do not process keyboard input if the custom edit control does not
        // have focus.
        if (!_internalFocus)
        {
            return;
        }

        // This holds the range we intend to operate on, or which we intend
        // to become the new selection. Start with the current selection.
        winrt::CoreTextRange range = _selection;

        // For the purpose of this sample, we will support only the left and right
        // arrow keys and the backspace key. A more complete text edit control
        // would also handle keys like Home, End, and Delete, as well as
        // hotkeys like Ctrl+V to paste.
        //
        // Note that this sample does not properly handle surrogate pairs
        // nor does it handle grapheme clusters.

        switch (args.Key())
        {
            // Backspace
            case winrt::VirtualKey::Back:
                // If there is a selection, then delete the selection.
                if (HasSelection())
                {
                    // Set the text in the selection to nothing.
                    ReplaceText(range, L"");
                }
                else
                {
                    // Delete the character to the left of the caret, if one exists,
                    // by creating a range that encloses the character to the left
                    // of the caret, and setting the contents of that range to nothing.
                    range.StartCaretPosition = max(0, range.StartCaretPosition - 1);
                    ReplaceText(range, L"");
                }
                break;

            // Left arrow
            case winrt::VirtualKey::Left:

                // If the shift key is down, then adjust the size of the selection.
                if (WI_IsFlagSet(winrt::InputKeyboardSource::GetKeyStateForCurrentThread(winrt::VirtualKey::Shift), winrt::CoreVirtualKeyStates::Down))
                {
                    // If this is the start of a selection, then remember which edge we are adjusting.
                    if (!HasSelection())
                    {
                        _extendingLeft = true;
                    }

                    // Adjust the selection and notify CoreTextEditContext.
                    AdjustSelectionEndpoint(-1);
                }
                else
                {
                    // The shift key is not down. If there was a selection, then snap the
                    // caret at the left edge of the selection.
                    if (HasSelection())
                    {
                        range.EndCaretPosition = range.StartCaretPosition;
                        SetSelectionAndNotify(range);
                    }
                    else
                    {
                        // There was no selection. Move the caret left one code unit if possible.
                        range.StartCaretPosition = max(0, range.StartCaretPosition - 1);
                        range.EndCaretPosition = range.StartCaretPosition;
                        SetSelectionAndNotify(range);
                    }
                }
                break;

            // Right arrow
            case winrt::VirtualKey::Right:

                // If the shift key is down, then adjust the size of the selection.
                if (WI_IsFlagSet(winrt::InputKeyboardSource::GetKeyStateForCurrentThread(winrt::VirtualKey::Shift), winrt::CoreVirtualKeyStates::Down))
                {
                    // If this is the start of a selection, then remember which edge we are adjusting.
                    if (!HasSelection())
                    {
                        _extendingLeft = false;
                    }

                    // Adjust the selection and notify CoreTextEditContext.
                    AdjustSelectionEndpoint(1);
                }
                else
                {
                    // The shift key is not down. If there was a selection, then snap the
                    // caret at the right edge of the selection.
                    if (HasSelection())
                    {
                        range.StartCaretPosition = range.EndCaretPosition;
                        SetSelectionAndNotify(range);
                    }
                    else
                    {
                        // There was no selection. Move the caret right one code unit if possible.
                        range.StartCaretPosition = min(static_cast<int32_t>(_text.length()), range.StartCaretPosition + 1);
                        range.EndCaretPosition = range.StartCaretPosition;
                        SetSelectionAndNotify(range);
                    }
                }
                break;
        }
    }

    // Adjust the active endpoint of the selection in the specified direction.
    void CustomEditControl::AdjustSelectionEndpoint(int direction)
    {
        CoreTextRange range = _selection;
        if (_extendingLeft)
        {
            range.StartCaretPosition = max(0, range.StartCaretPosition + direction);
        }
        else
        {
            range.EndCaretPosition = min(static_cast<int32_t>(_text.length()), range.EndCaretPosition + direction);
        }

        SetSelectionAndNotify(range);
    }

#pragma endregion Input

#pragma region UI

    // Helper function to put a zero-width non-breaking space at the end of a string.
    // This prevents TextBlock from trimming trailing spaces.
    static winrt::hstring PreserveTrailingSpaces(winrt::hstring s)
    {
        return s + L"\ufeff";
    }

    void CustomEditControl::UpdateFocusUI()
    {
        BorderPanel().BorderBrush(_internalFocus ? winrt::SolidColorBrush(winrt::Colors::Green()) : nullptr);
    }

    void CustomEditControl::UpdateTextUI()
    {
        // The raw materials we have are a string (_text) and information about
        // where the caret/selection is (_selection). We can render the control
        // any way we like.
  
        BeforeSelectionText().Text(PreserveTrailingSpaces(winrt::hstring(_text.substr(0, _selection.StartCaretPosition))));

        if (HasSelection())
        {
            // There is a selection. Draw that.
            CaretText().Visibility(winrt::Visibility::Collapsed);
            SelectionText().Text(PreserveTrailingSpaces(
                winrt::hstring(_text.substr(_selection.StartCaretPosition, _selection.EndCaretPosition - _selection.StartCaretPosition))));
        }
        else
        {
            // There is no selection. Remove it.
            SelectionText().Text(L"");

            // Draw the caret if we have focus.
            CaretText().Visibility(_internalFocus ? winrt::Visibility::Visible : winrt::Visibility::Collapsed);
        }

        AfterSelectionText().Text(PreserveTrailingSpaces(winrt::hstring(_text.substr(_selection.EndCaretPosition))));

        // Update statistics for demonstration purposes.
        FullText().Text(_text);
        SelectionStartIndexText().Text(winrt::to_hstring(_selection.StartCaretPosition));
        SelectionEndIndexText().Text(winrt::to_hstring(_selection.EndCaretPosition));
    }

#pragma endregion UI
}
