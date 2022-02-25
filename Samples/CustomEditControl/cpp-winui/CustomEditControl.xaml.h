#pragma once

#include "winrt/Microsoft.UI.Xaml.h"
#include "winrt/Microsoft.UI.Xaml.Markup.h"
#include "winrt/Microsoft.UI.Xaml.Controls.Primitives.h"
#include "CustomEditControl.g.h"

using namespace winrt;
using namespace Microsoft::UI::Xaml::Input;

namespace winrt::CustomEditControlWinAppSDK::implementation
{
    struct CustomEditControl : CustomEditControlT<CustomEditControl>
    {
        CustomEditControl();
        ~CustomEditControl();

        void OnKeyDown(KeyRoutedEventArgs args);
        void OnCharacterReceived(CharacterReceivedRoutedEventArgs args);

        void OnGotFocus(winrt::Microsoft::UI::Xaml::RoutedEventArgs args);
        void OnLostFocus(winrt::Microsoft::UI::Xaml::RoutedEventArgs args);

        void SetAppWindow(winrt::Microsoft::UI::WindowId windowId);

        void EditContext_TextRequested(
            winrt::Windows::UI::Text::Core::CoreTextEditContext sender,
            winrt::Windows::UI::Text::Core::CoreTextTextRequestedEventArgs args);
        void EditContext_SelectionRequested(
            winrt::Windows::UI::Text::Core::CoreTextEditContext sender,
            winrt::Windows::UI::Text::Core::CoreTextSelectionRequestedEventArgs args);
        void EditContext_TextUpdating(
            winrt::Windows::UI::Text::Core::CoreTextEditContext sender,
            winrt::Windows::UI::Text::Core::CoreTextTextUpdatingEventArgs args);
        void EditContext_SelectionUpdating(
            winrt::Windows::UI::Text::Core::CoreTextEditContext sender,
            winrt::Windows::UI::Text::Core::CoreTextSelectionUpdatingEventArgs args);
        void EditContext_FocusRemoved(
            winrt::Windows::UI::Text::Core::CoreTextEditContext sender,
            winrt::Windows::Foundation::IInspectable args);
        void EditContext_FormatUpdating(
            winrt::Windows::UI::Text::Core::CoreTextEditContext sender,
            winrt::Windows::UI::Text::Core::CoreTextFormatUpdatingEventArgs args);
        void EditContext_LayoutRequested(
            winrt::Windows::UI::Text::Core::CoreTextEditContext sender,
            winrt::Windows::UI::Text::Core::CoreTextLayoutRequestedEventArgs args);
        void EditContext_CompositionStarted(
            winrt::Windows::UI::Text::Core::CoreTextEditContext sender,
            winrt::Windows::UI::Text::Core::CoreTextCompositionStartedEventArgs args);
        void EditContext_CompositionCompleted(
            winrt::Windows::UI::Text::Core::CoreTextEditContext sender,
            winrt::Windows::UI::Text::Core::CoreTextCompositionCompletedEventArgs args);

        void UpdateTextUI();
        void UpdateFocusUI();

        void SetInternalFocus();
        void RemoveInternalFocus();
        void RemoveInternalFocusWorker();

        void ReplaceText(winrt::Windows::UI::Text::Core::CoreTextRange modifiedRange, winrt::hstring text);
        bool HasSelection();
        void SetSelection(winrt::Windows::UI::Text::Core::CoreTextRange selection);
        void SetSelectionAndNotify(winrt::Windows::UI::Text::Core::CoreTextRange selection);

        void AdjustSelectionEndpoint(int direction);

        // The _editContext lets us communicate with the input system.
        winrt::Windows::UI::Text::Core::CoreTextEditContext _editContext{ nullptr };

        // We will use a plain text string to represent the
        // content of the custom text edit control.
        std::wstring _text;

        // If the _selection starts and ends at the same point,
        // then it represents the location of the caret (insertion point).
        winrt::Windows::UI::Text::Core::CoreTextRange _selection = {};

        // _internalFocus keeps track of whether our control acts like it has focus.
        bool _internalFocus = false;

        // If there is a nonempty selection, then _extendingLeft is true if the user
        // is using shift+arrow to adjust the starting point of the selection,
        // or false if the user is adjusting the ending point of the selection.
        bool _extendingLeft = false;

        // The input pane object indicates the visibility of the on screen keyboard.
        // Apps can also ask the keyboard to show or hide.
        winrt::Windows::UI::ViewManagement::InputPane _inputPane{ nullptr };

        // The AppWindow that contains our control.
        winrt::Microsoft::UI::WindowId _appWindowId = {};

        winrt::event_token _tokenTextRequested;
        winrt::event_token _tokenSelectionRequested;
        winrt::event_token _tokenFocusRemoved;
        winrt::event_token _tokenTextUpdating;
        winrt::event_token _tokenSelectionUpdating;
        winrt::event_token _tokenFormatUpdating;
        winrt::event_token _tokenLayoutRequested;
        winrt::event_token _tokenCompositionStarted;
        winrt::event_token _tokenCompositionCompleted;
        winrt::event_token _tokenNotifyFocusLeaveCompleted;
    };
}

namespace winrt::CustomEditControlWinAppSDK::factory_implementation
{
    struct CustomEditControl : CustomEditControlT<CustomEditControl, implementation::CustomEditControl>
    {
    };
}
