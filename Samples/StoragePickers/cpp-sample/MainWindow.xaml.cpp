#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include <winrt/Windows.Data.Json.h>

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Windowing;
using namespace Microsoft::Windows::Storage::Pickers;
using namespace Windows::Data::Json;

namespace
{
    std::wstring TrimString(std::wstring_view text)
    {
        auto start = text.find_first_not_of(L" \t\r\n'\"");
        if (start == std::wstring_view::npos)
        {
            return {};
        }

        auto end = text.find_last_not_of(L" \t\r\n'\"");
        return std::wstring{ text.substr(start, end - start + 1) };
    }

    bool IsChecked(Microsoft::UI::Xaml::Controls::CheckBox const& checkbox)
    {
        if (auto value = checkbox.IsChecked())
        {
            return value.Value();
        }

        return false;
    }

    std::wstring FileNameFromPath(std::wstring_view path)
    {
        if (path.empty())
        {
            return {};
        }

        std::filesystem::path fsPath{ path };
        return fsPath.filename().wstring();
    }
}

namespace winrt::FilePickersAppSinglePackaged::implementation
{
    MainWindow::MainWindow()
    {
        InitializeComponent();
        Title(L"File Pickers Sample App");

        if (auto appWindow = this->AppWindow())
        {
            appWindow.Resize({ 1800, 1100 });
        }
    }

    void MainWindow::LogResult(winrt::hstring const& message)
    {
        using namespace std::chrono;

        auto now = system_clock::now();
        auto rawTime = system_clock::to_time_t(now);
        tm localTime{};
        localtime_s(&localTime, &rawTime);

        std::wstringstream timestampStream;
        timestampStream << std::put_time(&localTime, L"%H:%M:%S");

        std::wstring existing{ ResultsTextBlock().Text().c_str() };

        std::wstring newEntry;
        newEntry.reserve(existing.size() + message.size() + 16);
        newEntry.append(L"[");
        newEntry.append(timestampStream.str());
        newEntry.append(L"] ");
        newEntry.append(message.c_str());
        newEntry.append(L"\n");
        newEntry.append(existing);

        ResultsTextBlock().Text(winrt::hstring{ newEntry });
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
        std::wstring text{ FileTypeFilterInput().Text().c_str() };
        std::replace(text.begin(), text.end(), L';', L',');

        std::vector<winrt::hstring> filters;

        std::wstringstream stream{ text };
        std::wstring item;
        while (std::getline(stream, item, L','))
        {
            auto trimmed = TrimString(item);
            if (!trimmed.empty())
            {
                filters.emplace_back(trimmed);
            }
        }

        if (filters.empty())
        {
            filters.emplace_back(L"*");
        }

        return filters;
    }

    winrt::fire_and_forget MainWindow::NewPickSingleFile_Click(winrt::Windows::Foundation::IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        try
        {
            FileOpenPicker picker{ this->AppWindow().Id() };

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
                std::wstring path{ result.Path().c_str() };
                std::wstring fileName = FileNameFromPath(path);

                std::wstringstream message;
                message << L"New FileOpenPicker - PickSingleFileAsync:\nFile: " << fileName << L"\nPath: " << path;
                LogResult(winrt::hstring{ message.str() });
            }
            else
            {
                LogResult(L"New FileOpenPicker - PickSingleFileAsync: Operation cancelled");
            }
        }
        catch (winrt::hresult_error const& ex)
        {
            std::wstring message = L"Error in New FileOpenPicker: ";
            message.append(ex.message().c_str());
            LogResult(winrt::hstring{ message });
        }

        co_return;
    }

    winrt::fire_and_forget MainWindow::NewPickMultipleFiles_Click(winrt::Windows::Foundation::IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        try
        {
            FileOpenPicker picker{ this->AppWindow().Id() };

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
                std::wstringstream message;
                message << L"New FileOpenPicker - PickMultipleFilesAsync: " << results.Size() << L" files\n";

                for (auto const& item : results)
                {
                    std::wstring path{ item.Path().c_str() };
                    message << L"- " << FileNameFromPath(path) << L": " << path << L"\n";
                }

                LogResult(winrt::hstring{ message.str() });
            }
            else
            {
                LogResult(L"New FileOpenPicker - PickMultipleFilesAsync: Operation cancelled or no files selected");
            }
        }
        catch (winrt::hresult_error const& ex)
        {
            std::wstring message = L"Error in New PickMultipleFilesAsync: ";
            message.append(ex.message().c_str());
            LogResult(winrt::hstring{ message });
        }

        co_return;
    }

    winrt::fire_and_forget MainWindow::NewFileTypeChoices_Click(winrt::Windows::Foundation::IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        try
        {
            FileSavePicker picker{ this->AppWindow().Id() };

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
                auto folder = TrimString(SuggestedFolderInput().Text().c_str());
                if (!folder.empty())
                {
                    picker.SuggestedFolder(winrt::hstring{ folder });
                }
            }

            picker.FileTypeChoices().Clear();
            if (IsChecked(FileTypeChoicesCheckBox()))
            {
                auto jsonText = TrimString(FileTypeChoicesInput().Text().c_str());
                if (!jsonText.empty())
                {
                    JsonObject jsonObject;
                    if (JsonObject::TryParse(jsonText, jsonObject))
                    {
                        for (auto const& kvp : jsonObject)
                        {
                            std::vector<winrt::hstring> values;
                            if (kvp.Value().ValueType() == JsonValueType::Array)
                            {
                                for (auto const& value : kvp.Value().GetArray())
                                {
                                    if (value.ValueType() == JsonValueType::String)
                                    {
                                        values.emplace_back(value.GetString());
                                    }
                                }
                            }

                            if (!values.empty())
                            {
                                picker.FileTypeChoices().Insert(kvp.Key(), winrt::single_threaded_vector(std::move(values)));
                            }
                        }
                    }
                    else
                    {
                        LogResult(L"Error in New FileSavePicker: Unable to parse FileTypeChoices JSON");
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
                std::wstring path{ result.Path().c_str() };
                std::wstring fileName = FileNameFromPath(path);

                std::wstringstream message;
                message << L"New FileSavePicker picked file: \n" << fileName << L"\nPath: " << path;
                LogResult(winrt::hstring{ message.str() });
            }
            else
            {
                LogResult(L"New FileSavePicker with FileTypeChoices\nOperation cancelled");
            }
        }
        catch (winrt::hresult_error const& ex)
        {
            std::wstring message = L"Error in New FileTypeChoices: ";
            message.append(ex.message().c_str());
            LogResult(winrt::hstring{ message });
        }

        co_return;
    }

    winrt::fire_and_forget MainWindow::NewPickFolder_Click(winrt::Windows::Foundation::IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        try
        {
            FolderPicker picker{ this->AppWindow().Id() };

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
                std::wstring path{ result.Path().c_str() };
                std::wstring folderName = FileNameFromPath(path);

                std::wstringstream message;
                message << L"New FolderPicker - PickSingleFolderAsync:\nFolder: " << folderName << L"\nPath: " << path;
                LogResult(winrt::hstring{ message.str() });
            }
            else
            {
                LogResult(L"New FolderPicker - PickSingleFolderAsync: Operation cancelled");
            }
        }
        catch (winrt::hresult_error const& ex)
        {
            std::wstring message = L"Error in New FolderPicker: ";
            message.append(ex.message().c_str());
            LogResult(winrt::hstring{ message });
        }

        co_return;
    }
}
