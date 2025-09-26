#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

#include <algorithm>
#include <chrono>
#include <cwchar>
#include <cwctype>
#include <ctime>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <string_view>

namespace
{
    bool IsChecked(Microsoft::UI::Xaml::Controls::CheckBox const& checkbox)
    {
        if (auto isChecked = checkbox.IsChecked())
        {
            return isChecked.Value();
        }

        return false;
    }
}

namespace winrt::FilePickersAppSinglePackaged::implementation
{
    using Microsoft::UI::Xaml::Controls::CheckBox;
    using Microsoft::Windows::Storage::Pickers::FileOpenPicker;
    using Microsoft::Windows::Storage::Pickers::FileSavePicker;
    using Microsoft::Windows::Storage::Pickers::FolderPicker;
    using Microsoft::Windows::Storage::Pickers::PickerLocationId;
    using Microsoft::Windows::Storage::Pickers::PickerViewMode;
    using Windows::Data::Json::JsonObject;
    using Windows::Data::Json::JsonValueType;

    MainWindow::MainWindow()
    {
        // Xaml objects should not call InitializeComponent during construction.
        // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent
        InitializeComponent();

        Title(L"File Pickers Sample App");

        if (auto appWindow = AppWindow())
        {
            Windows::Graphics::SizeInt32 newSize{ 1800, 1100 };
            try
            {
                appWindow.Resize(newSize);
            }
            catch (winrt::hresult_error const&)
            {
                // Ignore if resize is not supported.
            }
        }
    }

    void MainWindow::LogResult(winrt::hstring const& message)
    {
        auto now = std::chrono::system_clock::now();
        std::time_t nowTime = std::chrono::system_clock::to_time_t(now);
        std::tm localTime{};
        localtime_s(&localTime, &nowTime);

    wchar_t buffer[9]{};
    wcsftime(buffer, sizeof(buffer) / sizeof(wchar_t), L"%H:%M:%S", &localTime);

        std::wstringstream stream;
        stream << L"[" << buffer << L"] " << message.c_str();

        std::wstring existingText{ ResultsTextBlock().Text().c_str() };
        stream << L"\n" << existingText;

    auto combined = stream.str();
    ResultsTextBlock().Text(winrt::hstring{ combined });
    }

    PickerLocationId MainWindow::GetSelectedNewLocationId()
    {
        switch (StartLocationComboBox().SelectedIndex())
        {
        case 0: return PickerLocationId::DocumentsLibrary;
        case 1: return PickerLocationId::ComputerFolder;
        case 2: return PickerLocationId::Desktop;
        case 3: return PickerLocationId::Downloads;
        case 4: return static_cast<PickerLocationId>(4);
        case 5: return PickerLocationId::MusicLibrary;
        case 6: return PickerLocationId::PicturesLibrary;
        case 7: return PickerLocationId::VideosLibrary;
        case 8: return PickerLocationId::Objects3D;
        case 9: return PickerLocationId::Unspecified;
        case 10: return static_cast<PickerLocationId>(10);
        default: throw hresult_invalid_argument(L"Invalid location selected");
        }
    }

    PickerViewMode MainWindow::GetSelectedNewViewMode()
    {
        switch (ViewModeComboBox().SelectedIndex())
        {
        case 0: return PickerViewMode::List;
        case 1: return PickerViewMode::Thumbnail;
        case 2: return static_cast<PickerViewMode>(2);
        default: throw hresult_invalid_argument(L"Invalid view mode selected");
        }
    }

    std::vector<winrt::hstring> MainWindow::GetFileFilters()
    {
        auto rawInput = FileTypeFilterInput().Text();
        std::wstring input{ rawInput.c_str() };

        auto trim = [](std::wstring_view value)
        {
            size_t start = 0;
            size_t end = value.size();

            while (start < end && std::iswspace(static_cast<wint_t>(value[start])))
            {
                ++start;
            }

            while (end > start && std::iswspace(static_cast<wint_t>(value[end - 1])))
            {
                --end;
            }

            return std::wstring{ value.substr(start, end - start) };
        };

        std::vector<winrt::hstring> filters;
        std::wstring current;
        for (wchar_t ch : input)
        {
            if (ch == L',' || ch == L';')
            {
                auto trimmed = trim(current);
                if (!trimmed.empty())
                {
                    filters.emplace_back(trimmed);
                }
                current.clear();
            }
            else
            {
                current.push_back(ch);
            }
        }

        auto trimmed = trim(current);
        if (!trimmed.empty())
        {
            filters.emplace_back(trimmed);
        }

        if (filters.empty())
        {
            filters.emplace_back(L"*");
        }

        return filters;
    }

    Windows::Foundation::IAsyncAction MainWindow::NewPickSingleFile_Click(winrt::Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&)
    {
        try
        {
            FileOpenPicker picker{ AppWindow().Id() };

            if (IsChecked(CommitButtonCheckBox()))
            {
                picker.CommitButtonText(CommitButtonTextInput().Text());
            }

            if (IsChecked(ViewModeCheckBox()))
            {
                picker.ViewMode(GetSelectedNewViewMode());
            }

            if (IsChecked(SuggestedStartLocationCheckBox()))
            {
                picker.SuggestedStartLocation(GetSelectedNewLocationId());
            }

            picker.FileTypeFilter().Clear();
            if (IsChecked(FileTypeFilterCheckBox()))
            {
                for (auto const& filter : GetFileFilters())
                {
                    picker.FileTypeFilter().Append(filter);
                }
            }

            auto result = co_await picker.PickSingleFileAsync();
            if (result)
            {
                std::filesystem::path path{ result.Path().c_str() };
                auto fileName = path.filename().wstring();

          std::wstringstream stream;
                stream << L"New FileOpenPicker - PickSingleFileAsync:\nFile: " << fileName
              << L"\nPath: " << result.Path().c_str();
          auto message = stream.str();
          LogResult(winrt::hstring{ message });
            }
            else
            {
                LogResult(L"New FileOpenPicker - PickSingleFileAsync: Operation cancelled");
            }
        }
        catch (winrt::hresult_error const& ex)
        {
            LogResult(L"Error in New FileOpenPicker: " + ex.message());
        }

        co_return;
    }

    Windows::Foundation::IAsyncAction MainWindow::NewPickMultipleFiles_Click(winrt::Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&)
    {
        try
        {
            FileOpenPicker picker{ AppWindow().Id() };

            if (IsChecked(CommitButtonCheckBox()))
            {
                picker.CommitButtonText(CommitButtonTextInput().Text());
            }

            if (IsChecked(ViewModeCheckBox()))
            {
                picker.ViewMode(GetSelectedNewViewMode());
            }

            if (IsChecked(SuggestedStartLocationCheckBox()))
            {
                picker.SuggestedStartLocation(GetSelectedNewLocationId());
            }

            picker.FileTypeFilter().Clear();
            if (IsChecked(FileTypeFilterCheckBox()))
            {
                for (auto const& filter : GetFileFilters())
                {
                    picker.FileTypeFilter().Append(filter);
                }
            }

            auto results = co_await picker.PickMultipleFilesAsync();
            if (results && results.Size() > 0)
            {
                std::wstringstream stream;
                stream << L"New FileOpenPicker - PickMultipleFilesAsync: " << results.Size() << L" files\n";
                for (auto const& file : results)
                {
                    std::filesystem::path path{ file.Path().c_str() };
                    stream << L"- " << path.filename().wstring() << L": " << file.Path().c_str() << L"\n";
                }
                auto message = stream.str();
                LogResult(winrt::hstring{ message });
            }
            else
            {
                LogResult(L"New FileOpenPicker - PickMultipleFilesAsync: Operation cancelled or no files selected");
            }
        }
        catch (winrt::hresult_error const& ex)
        {
            LogResult(L"Error in New PickMultipleFilesAsync: " + ex.message());
        }

        co_return;
    }

    Windows::Foundation::IAsyncAction MainWindow::NewFileTypeChoices_Click(winrt::Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&)
    {
        try
        {
            FileSavePicker picker{ AppWindow().Id() };

            if (IsChecked(SuggestedFileNameCheckBox()))
            {
                picker.SuggestedFileName(SuggestedFileNameInput().Text());
            }

            if (IsChecked(DefaultFileExtensionCheckBox()))
            {
                picker.DefaultFileExtension(DefaultFileExtensionInput().Text());
            }

            if (IsChecked(SuggestedFolderCheckBox()))
            {
                picker.SuggestedFolder(SuggestedFolderInput().Text());
            }

            picker.FileTypeChoices().Clear();
            if (IsChecked(FileTypeChoicesCheckBox()))
            {
                auto jsonText = FileTypeChoicesInput().Text();
                if (!jsonText.empty())
                {
                    try
                    {
                        auto jsonObject = JsonObject::Parse(jsonText);
                        for (auto const& kvp : jsonObject)
                        {
                            if (kvp.Value().ValueType() == JsonValueType::Array)
                            {
                                auto array = kvp.Value().GetArray();
                                auto extensions = winrt::single_threaded_vector<winrt::hstring>();
                                for (auto const& item : array)
                                {
                                    if (item.ValueType() == JsonValueType::String)
                                    {
                                        extensions.Append(item.GetString());
                                    }
                                }

                                if (extensions.Size() > 0)
                                {
                                    picker.FileTypeChoices().Insert(kvp.Key(), extensions);
                                }
                            }
                        }
                    }
                    catch (winrt::hresult_error const& ex)
                    {
                        LogResult(L"Invalid FileTypeChoices JSON: " + ex.message());
                    }
                }
            }

            if (IsChecked(CommitButtonCheckBox()))
            {
                picker.CommitButtonText(CommitButtonTextInput().Text());
            }

            if (IsChecked(SuggestedStartLocationCheckBox()))
            {
                picker.SuggestedStartLocation(GetSelectedNewLocationId());
            }

            auto result = co_await picker.PickSaveFileAsync();
            if (result)
            {
                std::filesystem::path path{ result.Path().c_str() };
                auto fileName = path.filename().wstring();

          std::wstringstream stream;
                stream << L"New FileSavePicker picked file: \n" << fileName
              << L"\nPath: " << result.Path().c_str();
          auto message = stream.str();
          LogResult(winrt::hstring{ message });
            }
            else
            {
                LogResult(L"New FileSavePicker with FileTypeChoices\nOperation cancelled");
            }
        }
        catch (winrt::hresult_error const& ex)
        {
            LogResult(L"Error in New FileTypeChoices: " + ex.message());
        }

        co_return;
    }

    Windows::Foundation::IAsyncAction MainWindow::NewPickFolder_Click(winrt::Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&)
    {
        try
        {
            FolderPicker picker{ AppWindow().Id() };

            if (IsChecked(CommitButtonCheckBox()))
            {
                picker.CommitButtonText(CommitButtonTextInput().Text());
            }

            if (IsChecked(SuggestedStartLocationCheckBox()))
            {
                picker.SuggestedStartLocation(GetSelectedNewLocationId());
            }

            auto result = co_await picker.PickSingleFolderAsync();
            if (result)
            {
                std::filesystem::path path{ result.Path().c_str() };
                auto folderName = path.filename().wstring();

          std::wstringstream stream;
                stream << L"New FolderPicker - PickSingleFolderAsync:\nFolder: " << folderName
              << L"\nPath: " << result.Path().c_str();
          auto message = stream.str();
          LogResult(winrt::hstring{ message });
            }
            else
            {
                LogResult(L"New FolderPicker - PickSingleFolderAsync: Operation cancelled");
            }
        }
        catch (winrt::hresult_error const& ex)
        {
            LogResult(L"Error in New FolderPicker: " + ex.message());
        }

        co_return;
    }
}
