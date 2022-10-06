#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif
#include <winrt/Windows.Storage.Pickers.h>
#include <microsoft.ui.xaml.window.h>
#include <ShObjIdl_core.h>
#include <winrt/Windows.UI.h>
#include "LabelControl.g.h"
#include "ContainerControl.g.h"
#include <map>

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace winrt::Microsoft::UI::Xaml::Controls;
// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

//BOPT::BOPTTextBlock vv;

namespace winrt::WinUI3App::implementation
{
  MainWindow::MainWindow()
  {
    //RequestedTheme(winrt::Microsoft::UI::Xaml::ApplicationTheme::Dark);
    InitializeComponent();
    
  }

  int32_t MainWindow::MyProperty()
  {
    throw hresult_not_implemented();
  }

  void MainWindow::MyProperty(int32_t /* value */)
  {
    throw hresult_not_implemented();
  }

  HWND MainWindow::GetWindowHandle()
  {
    HWND hwnd;
    Window window = try_as<Window>();
    window.as<IWindowNative>()->get_WindowHandle(&hwnd);
    return hwnd;
  }


  void MainWindow::BrowseForFile(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args)
  {
    PickFile();
  }

  fire_and_forget  MainWindow::PickFile()
  {
    auto picker = winrt::Windows::Storage::Pickers::FileOpenPicker();
    HWND hwnd = GetWindowHandle();
    winrt::check_hresult(picker.as<IInitializeWithWindow>()->Initialize(hwnd));

    //File and Folder pickers on desktop requires nonempty filters
    picker.FileTypeFilter().Append(L"*");
    auto storageFolder = co_await picker.PickSingleFileAsync();

    if (!storageFolder)
    {
      co_return;
    }
    //BrowseTextBox().Text(storageFolder.Path());
  }
  
  void MainWindow::BrowseForFolder(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args)
  {
      PickFolder();
  }
    
  fire_and_forget  MainWindow::PickFolder()
  {
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
}







