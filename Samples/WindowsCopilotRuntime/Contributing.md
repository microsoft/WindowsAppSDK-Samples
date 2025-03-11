# Contributing
This shows the project structure and how one can get started contributing to WCR sample.

## Project Structure

The project follows the MVVM (Model-View-ViewModel) pattern. Here's how the code is organized:

- **Models**: Contains the data models used in the application.
  - `Models/ImageDescriptionModel.cs`
  - `Models/ImageObjectExtractorModel.cs`
- **ViewModels**: Contains the view models that handle the business logic and data binding for the views.
  - `ViewModels/ImageDescriptionViewModel.cs`
  - `ViewModels/mageObjectExtractorViewModel.cs`
- **Pages**: Contains the XAML pages and their code-behind files. It is the *View* of MVVM structure.
  - `Pages/ImageDescriptionPage.xaml`
  - `Views/ImageDescriptionPage.xaml.cs`
  - `Views/ImageObjectExtractorPage.xaml`
  - `Views/ImageObjectExtractorPage.xaml.cs`
- **Helpers**: Util, Converters and Control contain common shared code used by all the pages
  - `Control` contains user control for showing creating Session and example source codes
  - `CodeBlockControl.xaml` and `CodeBlockControl.xaml.cs` have user control to show example source code and each page intiailiazes it with code from `Example` folder.
  - `ModelInitializationControl.xaml` and `ModelInitializationControl.xaml.cs` have user control to create session for every page. Each page sets `CreateModelSessionWithProgress` as `ICommand` to button in this control. 
- 
## Adding a New Page
Any of the examples like `ImageScaler`, `ImageDescriptionModel` can be as reference for adding newer pages. `LanguageModel` is slightly different as it has multiple effects in the sample page. 


## AsyncCommand and Its Inherited Class

In this project, we use `AsyncCommand` to handle asynchronous operations in the view models. `AsyncCommand` is a custom implementation of `ICommand` that supports asynchronous execution.

### AsyncCommand

Here's an example of how to use `AsyncCommand`:

```csharp
using System.Threading.Tasks;
using System.Windows.Input;
using Microsoft.Maui.Controls;

namespace YourNamespace.ViewModels
{
    public class AsyncCommandViewModel : BaseViewModel
    {
        public ICommand LoadDataCommand { get; }

        public AsyncCommandViewModel()
        {
            LoadDataCommand = new AsyncCommand(OnLoadDataExecuted);
        }

        private async Task OnLoadDataExecuted()
        {
            // Asynchronous command execution logic
            await Task.Delay(1000); // Simulate a delay
        }
    }
}
```

In this example, `LoadDataCommand` is an instance of `AsyncCommand` that executes the `OnLoadDataExecuted` method asynchronously.

### AsyncCommandWithProgress

`AsyncCommandWithProgress` is an inherited class from `AsyncCommand` that supports progress reporting during the asynchronous operation.

Here's an example of how to use `AsyncCommandWithProgress`:

```csharp
using System;
using System.Threading.Tasks;
using System.Windows.Input;
using Microsoft.Maui.Controls;

namespace YourNamespace.ViewModels
{
    public class AsyncCommandWithProgressViewModel : BaseViewModel
    {
        public ICommand LoadDataWithProgressCommand { get; }
        public int Progress { get; private set; }

        public AsyncCommandWithProgressViewModel()
        {
            LoadDataWithProgressCommand = new AsyncCommandWithProgress<int>(OnLoadDataWithProgressExecuted, OnProgressChanged);
        }

        private async Task OnLoadDataWithProgressExecuted(IProgress<int> progress)
        {
            for (int i = 0; i <= 100; i++)
            {
                await Task.Delay(50); // Simulate work
                progress.Report(i); // Report progress
            }
        }

        private void OnProgressChanged(int progress)
        {
            Progress = progress;
            OnPropertyChanged(nameof(Progress));
        }
    }
}
```

In this example, `LoadDataWithProgressCommand` is an instance of `AsyncCommandWithProgress` that executes the `OnLoadDataWithProgressExecuted` method asynchronously and reports progress updates.
