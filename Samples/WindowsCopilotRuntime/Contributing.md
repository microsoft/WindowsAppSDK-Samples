# Contributing

This shows the project structure and how one can get started contributing to Windows Copilot Runtime (WCR) sample.

## Project Structure

The project follows the MVVM (Model-View-ViewModel) pattern. Here's how the code is organized:

- **Models**: Contains the data models used in the application.
  - `cs-winui/Models/ImageDescriptionModel.cs`
  - `cs-winui/Models/ImageObjectExtractorModel.cs`
- **ViewModels**: Contains the view models that handle the business logic and data binding for the views.
  - `cs-winui/ViewModels/ImageDescriptionViewModel.cs`
  - `cs-winui/ViewModels/mageObjectExtractorViewModel.cs`
- **Pages**: Contains the XAML pages and their code-behind files. They represent the `Views` part of MVVM.
  - `cs-winui/Pages/ImageDescriptionPage.xaml`
  - `cs-winui/Pages/ImageDescriptionPage.xaml.cs`
  - `cs-winui/Pages/ImageObjectExtractorPage.xaml`
  - `cs-winui/Pages/ImageObjectExtractorPage.xaml.cs`
- **Helpers**: The Util, Converters and Controls folders contain common shared code used by all the pages
  - The `Controls` folder contains user controls for showing creating Session and example source codes
  - `CodeBlockControl.xaml` and `CodeBlockControl.xaml.cs` are parts of user control which shows example source code for each API. The example code is fetched from the `Examples` folder.
  - `ModelInitializationControl.xaml` and `ModelInitializationControl.xaml.cs` have user control to create session for every page. Each page sets `CreateModelSessionWithProgress` as `ICommand` to button in this control. 
  
## Adding a New Page
Any of the examples like `ImageScaler` or `ImageDescriptionModel` can be as reference for adding newer pages. `LanguageModel` is slightly different as it has multiple effects in the sample page. 


## AsyncCommand and Its Inherited Class

In this project, we use `AsyncCommand` to handle asynchronous operations in the view models. `AsyncCommand` is a custom implementation of `ICommand` that supports asynchronous execution.

### AsyncCommand

Here's an example of how to use `AsyncCommand`:

```csharp
...
    public ICommand ScaleCommand => _scaleSoftwareBitmapCommand;
    private readonly AsyncCommand<(SoftwareBitmap, int, int), SoftwareBitmapSource> _scaleSoftwareBitmapCommand;

    public ImageScalerViewModel(ImageScalerModel imageScalerSession)
    : base(imageScalerSession)
    {
        _scaleSoftwareBitmapCommand = new(
            async _ =>
            {
                var height = (int)(Input!.PixelHeight * Factor);
                var width = (int)(Input!.PixelWidth * Factor);
                var outputBitmap = Session.ScaleSoftwareBitmap(Input!, width, height);
                ...
            },
            (_) => IsAvailable && Input is not null);
    }

...

```
Here, our code needs to call `ImageScalerModel.ScaleSoftwareBitmap(SoftwareBitmap inputImage, int, int)` from the view model's Scale Image button (refer to [`ImageScalerPage.xaml`](./cs-winui/Pages/ImageScalerPage.xaml)). We set the `ScaleCommand` property to an `AsyncCommand` that calls `ScaleSoftwareBitmap`. `AsyncCommand` implements `ICommand` so this assignment works.
The `ICommand` is enabled only when the Input image is loaded and the session is created, denoted by Image and IsAvailable respectively.

### AsyncCommandWithProgress

`AsyncCommandWithProgress` is an inherited class from `AsyncCommand` that supports receiving response in a progressive manner, few tokens at a time.

Here's an example of how to use `AsyncCommandWithProgress`:

```csharp
...
    private readonly AsyncCommandWithProgress<string, LanguageModelResponse, string> _generateResponseWithProgressCommand;
    private readonly StringBuilder _responseProgress = new();

    // GenerateResponseWithProgress
    _generateResponseWithProgressCommand = new(
        prompt =>
        {
            _responseProgress.Clear();
            DispatchPropertyChanged(nameof(ResponseProgress));

            return Session.GenerateResponseWithProgressAsync(prompt!);
        },
        (prompt) => IsAvailable && !string.IsNullOrEmpty(prompt));

    _generateResponseWithProgressCommand.ResultProgressHandler += OnResultProgress;
    _generateResponseWithProgressCommand.ResultHandler += OnResult;
...
```
Similar to `AsyncCommand`, `AsyncCommandWithProgress` implements `ICommand` and calls model APIs. It provides two event handlers, `ResultProgressHandler` and `ResultHandler`.
`ResultProgressHandler` is used to update the result (`ResponseProgress`) as newer tokens arrive.
`ResultHandler` is called when final response token has arrived and there are no more pending response tokens.

Refer to [`LanguageModelViewModel.cs`](./cs-winui/ViewModels/LanguageModelViewModel.cs) for detailed implementation.
