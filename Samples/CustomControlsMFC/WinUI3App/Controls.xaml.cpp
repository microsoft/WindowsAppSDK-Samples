#include "pch.h"
#include "Controls.xaml.h"
#if __has_include("Controls.g.cpp")
#include "Controls.g.cpp"
#endif
#include <winrt/Windows.Storage.Pickers.h>
#include <microsoft.ui.xaml.window.h>
#include <ShObjIdl_core.h>
#include <winrt/Windows.UI.h>
//#include "MainWindow.xaml.h"
#include <map>

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace winrt::Microsoft::UI::Xaml::Controls;
using namespace winrt::Microsoft::UI::Xaml::Media;
// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::WinUI3App::implementation
{
  int buttonClicked = 0;
  Controls::Controls()
  {
    InitializeComponent();
  }

  int32_t Controls::MyProperty()
  {
    throw hresult_not_implemented();
  }

  void Controls::MyProperty(int32_t /* value */)
  {
    throw hresult_not_implemented();
  }

  void Controls::Button_Click(IInspectable const&, RoutedEventArgs const&)
  {
    TextBlock1().Text(L"ButtonClicked " + to_hstring(buttonClicked));
    buttonClicked++;
  }

  void Controls::ComboBox_SelectionChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const& e)
  {
    ComboBox comboBox = sender.as<ComboBox>(); //or ComboBox1()
    auto comboBoxText = comboBox.SelectedValue().as<winrt::hstring>();
    TextBlock2().Text(L"Selected: " + comboBoxText);
  }

  void Controls::CheckBox1_Checked(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
  {
    TextBlock3().Text(L"Checked");
  }


  void Controls::CheckBox1_Unchecked(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
  {
    TextBlock3().Text(L"Unchecked");
  }

  void Controls::RadioButton1_Checked(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
  {
  }


  void Controls::RadioButton2_Checked(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
  {
  }


  void Controls::TextBoxInput_TextChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::TextChangedEventArgs const& e)
  {
    TextBlockOutput().Text(L"Your text: " + TextBoxInput().Text());
  }

  template <typename ancestor_type>
  auto find_ancestor(DependencyObject root) noexcept
  {
    auto ancestor{ root.try_as<ancestor_type>() };
    while (!ancestor && root)
    {
      root = VisualTreeHelper::GetParent(root);
      ancestor = root.try_as<ancestor_type>();
    }
    _ASSERT(ancestor);
    return ancestor;
  }
    

  HWND Controls::GetWindowHandle()
  {
    HWND hwnd;
    auto window = find_ancestor<Window>(*this);
    window.as<IWindowNative>()->get_WindowHandle(&hwnd);
    return hwnd;
  }

  fire_and_forget  Controls::PickFolder()
  {
    //WinUI Issue 2 - FolderPicker on UserControl
    auto picker = winrt::Windows::Storage::Pickers::FolderPicker();
    HWND hwnd = GetWindowHandle();
    winrt::check_hresult(picker.as<IInitializeWithWindow>()->Initialize(hwnd));

    //File and Folder pickers on desktop requires nonempty filters
    picker.FileTypeFilter().Append(L"*");
    auto storageFolder = co_await picker.PickSingleFolderAsync();

    if (!storageFolder)
    {
      co_return;
    }
    //BrowseTextBox().Text(storageFolder.Path());
  }

  void Controls::BrowseButtonClick(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
  {
    PickFolder();
  }

  fire_and_forget UseFileOpenPicker()
  {
    winrt::Windows::Storage::Pickers::FileOpenPicker picker;
    picker.FileTypeFilter().Append(L".png");
    picker.SuggestedStartLocation(winrt::Windows::Storage::Pickers::PickerLocationId::PicturesLibrary);
    auto file = co_await picker.PickSingleFileAsync();
  }

  void Controls::BrowseButtonClickNoHwnd(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
  {
    UseFileOpenPicker();
  }
   

  void Controls::Slider1_ValueChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs const& e)
  {
    try
    {
      auto value = winrt::to_hstring(Slider1().Value());
      TextBlock4().Text(L"Slider: " + value);
    }
    catch (const std::exception&)
    {
    }
  }

  void Controls::ProgressSlider_ValueChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs const& e)
  {
    if (ProgressSlider().Value())
    {
      ProgressRing1().Value(ProgressSlider().Value());
    }
    else
    {
      ProgressSlider().Value(0);
    }
  }

  void Controls::ListBox1_SelectionChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const& e)
  {
    if (ListBox1() != nullptr)
    {
      auto colorName = ListBox1().SelectedIndex();
      switch (colorName)
      {
      case 0:
        Border1().BorderBrush(Media::SolidColorBrush(Windows::UI::Colors::Blue()));
        break;
      case 1:
        Border1().BorderBrush(Media::SolidColorBrush(Windows::UI::Colors::Green()));
        break;
      case 2:
        Border1().BorderBrush(Media::SolidColorBrush(Windows::UI::Colors::Red()));
        break;
      case 3:
        Border1().BorderBrush(Media::SolidColorBrush(Windows::UI::Colors::Yellow()));
        break;
      }
    }

  }


  void Controls::RadioButtons1_SelectionChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const& e)
  {
    if (RadioButtons1() != nullptr)
    {
      auto colorName = RadioButtons1().SelectedIndex();
      switch (colorName)
      {
      case 0:
        Border2().Background(Media::SolidColorBrush(Windows::UI::Colors::Green()));
        break;
      case 1:
        Border2().Background(Media::SolidColorBrush(Windows::UI::Colors::Yellow()));
        break;
      case 2:
        Border2().Background(Media::SolidColorBrush(Windows::UI::Colors::White()));
        break;
      }
    }
  }
}
