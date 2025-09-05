// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
using Microsoft.Windows.AI.ContentSafety;
using Microsoft.Windows.AI.Text;
using Microsoft.Windows.Storage.Pickers;
using System;
using System.Collections.ObjectModel;
using System.Text;
using System.Windows.Input;
using WindowsAISample.Models;
using WindowsAISample.Util;
using WinRT.Interop;

namespace WindowsAISample.ViewModels;

/// <summary>
/// A model to test the AI fabric Language Model API
/// </summary>
internal partial class LanguageModelViewModel : CopilotModelBase<LanguageModelModel>
{
    private string? _prompt;
    private string? _textIntelligencePrompt;
    private string? _embeddingPrompt;
    private string? _context;
    private string? _adapterFilePath;
    private string? _loraPrompt;

    // LanguageModelOptions
    //private LanguageModelSkill _languageModelOptionsSkill = LanguageModelSkill.General;
    private string? _languageModelOptionsTemp = "0.9";
    private string? _languageModelOptionsTopP = "0.9";
    private string? _languageModelOptionsTopK = "40";

    // ContentFilterOptions - Prompt min severity to block
    private SeverityLevel _hateContentSevToBlockForPrompt = SeverityLevel.Minimum;
    private SeverityLevel _sexualContentSevToBlockForPrompt = SeverityLevel.Minimum;
    private SeverityLevel _violentContentSevToBlockForPrompt = SeverityLevel.Minimum;
    private SeverityLevel _selfHarmContentSevToBlockForPrompt = SeverityLevel.Minimum;

    // ContentFilterOptions - Response min severity to block
    private SeverityLevel _hateContentSevToBlockForResponse = SeverityLevel.Minimum;
    private SeverityLevel _sexualContentSevToBlockForResponse = SeverityLevel.Minimum;
    private SeverityLevel _violentContentSevToBlockForResponse = SeverityLevel.Minimum;
    private SeverityLevel _selfHarmContentSevToBlockForResponse = SeverityLevel.Minimum;

    private readonly AsyncCommandWithProgress<string, LanguageModelResponseResult, string> _generateResponseWithProgressCommand;
    private readonly AsyncCommandWithProgress<string, LanguageModelResponseResult, string> _generateResponseWithOptionsAndProgressCommand;
    private readonly AsyncCommandWithProgress<string, LanguageModelResponseResult, string> _generateResponseWithContextProgressCommand;

    private readonly AsyncCommandWithProgress<string, LanguageModelResponseResult, string> _generateResponseWithTextIntelligenceSummarizeSkills;
    private readonly AsyncCommandWithProgress<string, LanguageModelResponseResult, string> _generateResponseWithTextIntelligenceRewriteSkills;
    private readonly AsyncCommand<string, string> _generateResponseWithTextIntelligenceTextToTableSkills;

    private readonly AsyncCommand<string, LanguageModelEmbeddingVectorResult> _generateEmbeddingVectorCommand;

    private readonly AsyncCommand<object, bool> _pickInputAdapterCommand;
    private readonly AsyncCommandWithProgress<string, LanguageModelResponseResult, string> _generateResponseWithLoRACommand;
    private readonly AsyncCommandWithProgress<string, LanguageModelResponseResult, string> _generateResponseWithoutLoRACommand;

    private readonly StringBuilder _responseProgress = new();
    private readonly StringBuilder _responseProgressTextIntelligence = new();

    private readonly StringBuilder _responseProgressWithLoRA = new();
    private readonly StringBuilder _responseProgressWithoutLoRA = new();

    public ObservableCollection<SeverityLevel> ContentFilterSeverityLevels { get; } = new ObservableCollection<SeverityLevel> {
        SeverityLevel.Minimum,
        SeverityLevel.Low,
        SeverityLevel.Medium,
        SeverityLevel.High
    };

    public LanguageModelViewModel(LanguageModelModel languageModelSession)
        : base(languageModelSession)
    {
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

        // GenerateResponseWithOptionsAndProgress
        _generateResponseWithOptionsAndProgressCommand = new(
            prompt =>
            {
                var languageModelOptions = new LanguageModelOptions();  // default values

                var promptMinSeverityLevelToBlock = new TextContentFilterSeverity {
                    Hate = HateContentSevToBlockForPrompt,
                    Sexual = SexualContentSevToBlockForPrompt,
                    Violent = ViolentContentSevToBlockForPrompt,
                    SelfHarm = SelfHarmContentSevToBlockForPrompt
                };

                var responseMinSeverityLevelToBlock = new TextContentFilterSeverity {
                    Hate = HateContentSevToBlockForResponse,
                    Sexual = SexualContentSevToBlockForResponse,
                    Violent = ViolentContentSevToBlockForResponse,
                    SelfHarm = SelfHarmContentSevToBlockForResponse
                };

                var contentFilterOptions = new ContentFilterOptions {
                    PromptMaxAllowedSeverityLevel = promptMinSeverityLevelToBlock,
                    ResponseMaxAllowedSeverityLevel = responseMinSeverityLevelToBlock
                };

                _responseProgress.Clear();
                DispatchPropertyChanged(nameof(ResponseProgress));

                return Session.GenerateResponseWithOptionsAndProgressAsync(prompt!, languageModelOptions, contentFilterOptions);
            },
            (prompt) => IsAvailable && !string.IsNullOrEmpty(prompt));

        _generateResponseWithOptionsAndProgressCommand.ResultProgressHandler += OnResultProgress;
        _generateResponseWithOptionsAndProgressCommand.ResultHandler += OnResult;

        // GenerateResponseWithProgress
        _generateResponseWithContextProgressCommand = new(
            prompt =>
            {
                _responseProgress.Clear();
                DispatchPropertyChanged(nameof(ResponseProgress));

                return Session.GenerateResponseWithContextAsync(prompt!, Context!);
            },
            (prompt) => IsAvailable && !string.IsNullOrEmpty(prompt));

        _generateResponseWithContextProgressCommand.ResultProgressHandler += OnResultProgress;
        _generateResponseWithContextProgressCommand.ResultHandler += OnResult;

        // GenerateResponseWithTextIntelligenceSummarizeSkills
        _generateResponseWithTextIntelligenceSummarizeSkills = new(
            prompt =>
            {
                _responseProgressTextIntelligence.Clear();
                DispatchPropertyChanged(nameof(ResponseProgressTextIntelligence));

                return Session.GenerateResponseTextIntelligenceSummarizeWithProgressAsync(prompt!);
            },
            (prompt) => IsAvailable && !string.IsNullOrEmpty(TextIntelligencePrompt));

        _generateResponseWithTextIntelligenceSummarizeSkills.ResultProgressHandler += OnResultProgressTextIntelligence;
        _generateResponseWithTextIntelligenceSummarizeSkills.ResultHandler += OnResultTextIntelligence;

        // GenerateResponseWithTextIntelligenceRewriteSkills
        _generateResponseWithTextIntelligenceRewriteSkills = new(
            prompt =>
            {
                _responseProgressTextIntelligence.Clear();
                DispatchPropertyChanged(nameof(ResponseProgressTextIntelligence));

                return Session.GenerateResponseTextIntelligenceRewriteWithProgressAsync(prompt!);
            },
            (prompt) => IsAvailable && !string.IsNullOrEmpty(TextIntelligencePrompt));

        _generateResponseWithTextIntelligenceRewriteSkills.ResultProgressHandler += OnResultProgressTextIntelligence;
        _generateResponseWithTextIntelligenceRewriteSkills.ResultHandler += OnResultTextIntelligence;

        // GenerateResponseWithTextIntelligenceTextToTableSkills
        _generateResponseWithTextIntelligenceTextToTableSkills = new(
            prompt =>
            {
                return Session.GenerateResponseTextIntelligenceTextToTableAsync(prompt!);
            },
            (prompt) => IsAvailable && !string.IsNullOrEmpty(TextIntelligencePrompt));

        // GenerateEmbedding
        _generateEmbeddingVectorCommand = new(
            prompt =>
            {
                return Session.GenerateEmbeddingVectors(prompt!);
            },
            (prompt) => IsAvailable && !string.IsNullOrEmpty(prompt));

        // GenerateResponseWithLoRA
        _generateResponseWithLoRACommand = new(
            prompt =>
            {
                _responseProgressWithLoRA.Clear();
                DispatchPropertyChanged(nameof(ResponseProgressWithLoRA));

                return Session.GenerateResponseWithLoraAdapterAndContextAsync(Context ?? string.Empty, prompt!, AdapterFilePath!);
            },
            (prompt) => IsAvailable && !string.IsNullOrEmpty(LoRAPrompt));

        _generateResponseWithLoRACommand.ResultProgressHandler += OnResultProgressWithLoRA;
        _generateResponseWithLoRACommand.ResultHandler += OnResultWithLoRA;

        // GenerateResponseWithoutLoRA
        _generateResponseWithoutLoRACommand = new(
            prompt =>
            {
                _responseProgressWithoutLoRA.Clear();
                DispatchPropertyChanged(nameof(ResponseProgressWithoutLoRA));

                if (string.IsNullOrEmpty(Context))
                {
                    return Session.GenerateResponseWithProgressAsync(prompt);
                }
                else
                {
                    return Session.GenerateResponseWithContextAsync(prompt, Context!);
                }
            },
            (prompt) => IsAvailable && !string.IsNullOrEmpty(LoRAPrompt));

        _generateResponseWithoutLoRACommand.ResultProgressHandler += OnResultProgressWithoutLoRA;
        _generateResponseWithoutLoRACommand.ResultHandler += OnResultWithoutLoRA;

        _pickInputAdapterCommand = new(async _ =>
        {
            var picker = new FileOpenPicker(App.Window.Id);

            picker.ViewMode = PickerViewMode.List;
            picker.SuggestedStartLocation = PickerLocationId.DocumentsLibrary;
            picker.FileTypeFilter.Add(".safetensors");

            var result = await picker.PickSingleFileAsync();
            if (result == null)
            {
                return false;
            }

            var file = await StorageFile.CreateFromPathAsync(result.Path);
            if (file != null)
            {
                await DispatcherQueue.EnqueueAsync(() =>
                {
                    AdapterFilePath = file.Path;
                });

                return true;
            }
            return false;

        },
        _ => true);
    }

    public string ResponseProgress => _responseProgress.ToString();
    public string ResponseProgressTextIntelligence => _responseProgressTextIntelligence.ToString();
    public string ResponseProgressWithLoRA => _responseProgressWithLoRA.ToString();
    public string ResponseProgressWithoutLoRA => _responseProgressWithoutLoRA.ToString();


    public string? Prompt
    {
        get => _prompt;
        set
        {
            SetField(ref _prompt, value);
        }
    }

    public string? TextIntelligencePrompt
    {
        get => _textIntelligencePrompt;
        set => SetField(ref _textIntelligencePrompt, value);
    }

    public string? EmbeddingPrompt
    {
        get => _embeddingPrompt;
        set
        {
            SetField(ref _embeddingPrompt, value);
            //_generateResponseCommand.FireCanExecuteChanged(); //TODO
        }
    }

    public string? LoRAPrompt
    {
        get => _loraPrompt;
        set => SetField(ref _loraPrompt, value);
    }

    public string? Context
    {
        get => _context;
        set => SetField(ref _context, value);
    }

    public string? AdapterFilePath
    {
        get => _adapterFilePath;
        set => SetField(ref _adapterFilePath, value);
    }

    //public LanguageModelSkill LanguageModelOptionsSkill
    //{
    //    get => _languageModelOptionsSkill;
    //    set => SetField(ref _languageModelOptionsSkill, value);
    //}

    public string? LanguageModelOptionsTemp
    {
        get => _languageModelOptionsTemp;
        set => SetField(ref _languageModelOptionsTemp, value);
    }

    public string? LanguageModelOptionsTopP
    {
        get => _languageModelOptionsTopP;
        set => SetField(ref _languageModelOptionsTopP, value);
    }

    public string? LanguageModelOptionsTopK
    {
        get => _languageModelOptionsTopK;
        set => SetField(ref _languageModelOptionsTopK, value);
    }

    public SeverityLevel HateContentSevToBlockForPrompt
    {
        get => _hateContentSevToBlockForPrompt;
        set => SetField(ref _hateContentSevToBlockForPrompt, value);
    }

    public SeverityLevel SexualContentSevToBlockForPrompt
    {
        get => _sexualContentSevToBlockForPrompt;
        set => SetField(ref _sexualContentSevToBlockForPrompt, value);
    }

    public SeverityLevel ViolentContentSevToBlockForPrompt
    {
        get => _violentContentSevToBlockForPrompt;
        set => SetField(ref _violentContentSevToBlockForPrompt, value);
    }

    public SeverityLevel SelfHarmContentSevToBlockForPrompt
    {
        get => _selfHarmContentSevToBlockForPrompt;
        set => SetField(ref _selfHarmContentSevToBlockForPrompt, value);
    }

    public SeverityLevel HateContentSevToBlockForResponse
    {
        get => _hateContentSevToBlockForResponse;
        set => SetField(ref _hateContentSevToBlockForResponse, value);
    }

    public SeverityLevel SexualContentSevToBlockForResponse
    {
        get => _sexualContentSevToBlockForResponse;
        set => SetField(ref _sexualContentSevToBlockForResponse, value);
    }

    public SeverityLevel ViolentContentSevToBlockForResponse
    {
        get => _violentContentSevToBlockForResponse;
        set => SetField(ref _violentContentSevToBlockForResponse, value);
    }

    public SeverityLevel SelfHarmContentSevToBlockForResponse
    {
        get => _selfHarmContentSevToBlockForResponse;
        set => SetField(ref _selfHarmContentSevToBlockForResponse, value);
    }

    /// <summary>
    /// Exercise the GenerateResponse method API for a language model session
    /// </summary>

    public ICommand GenerateResponseWithProgressCommand => _generateResponseWithProgressCommand;

    public ICommand GenerateResponseWithOptionsAndProgressCommand => _generateResponseWithOptionsAndProgressCommand;
    public ICommand GenerateResponseWithTextIntelligenceSummarizeSkills => _generateResponseWithTextIntelligenceSummarizeSkills;
    public ICommand GenerateResponseWithTextIntelligenceRewriteSkills => _generateResponseWithTextIntelligenceRewriteSkills;
    public ICommand GenerateResponseWithTextIntelligenceTextToTableSkills => _generateResponseWithTextIntelligenceTextToTableSkills;

    public ICommand GenerateResponseWithContextProgressCommand => _generateResponseWithContextProgressCommand;

    public ICommand GenerateResponseWithLoRACommand => _generateResponseWithLoRACommand;
    public ICommand GenerateResponseWithoutLoRACommand => _generateResponseWithoutLoRACommand;

    public ICommand PickInputAdapterCommand => _pickInputAdapterCommand;

    /// <summary>
    /// Exercise the GenerateEmbeddingVector method API for a language model session
    /// </summary>
    public ICommand GenerateEmbeddingVectorCommand => _generateEmbeddingVectorCommand;

    protected override void OnIsAvailableChanged()
    {
        _generateResponseWithProgressCommand.FireCanExecuteChanged();
        _generateResponseWithOptionsAndProgressCommand.FireCanExecuteChanged();
        _generateResponseWithTextIntelligenceSummarizeSkills.FireCanExecuteChanged();
        _generateResponseWithTextIntelligenceRewriteSkills.FireCanExecuteChanged();
        _generateResponseWithTextIntelligenceTextToTableSkills.FireCanExecuteChanged();
        _generateResponseWithContextProgressCommand.FireCanExecuteChanged();
    }

    private void OnResultProgress(object? sender, string progress)
    {
        _responseProgress.Append(progress);
        DispatchPropertyChanged(nameof(ResponseProgress));
    }

    private void OnResultProgressTextIntelligence(object? sender, string progress)
    {
        _responseProgressTextIntelligence.Append(progress);
        DispatchPropertyChanged(nameof(ResponseProgressTextIntelligence));
    }

    private void OnResultProgressWithLoRA(object? sender, string progress)
    {
        _responseProgressWithLoRA.Append(progress);
        DispatchPropertyChanged(nameof(ResponseProgressWithLoRA));
    }

    private void OnResultProgressWithoutLoRA(object? sender, string progress)
    {
        _responseProgressWithoutLoRA.Append(progress);
        DispatchPropertyChanged(nameof(ResponseProgressWithoutLoRA));
    }

    private void OnResult(object? sender, LanguageModelResponseResult finalLanguageModelResponse)
    {
        if ((finalLanguageModelResponse.Status != LanguageModelResponseStatus.Complete)
              && (finalLanguageModelResponse.Status != LanguageModelResponseStatus.InProgress))
        {
            _responseProgress.Clear();
            _responseProgress.Append($"LanguageModelResponseStatus: {finalLanguageModelResponse.Status}");
        }

        DispatchPropertyChanged(nameof(ResponseProgress));
    }

    private void OnResultTextIntelligence(object? sender, LanguageModelResponseResult finalLanguageModelResponse)
    {
        if ((finalLanguageModelResponse.Status != LanguageModelResponseStatus.Complete)
              && (finalLanguageModelResponse.Status != LanguageModelResponseStatus.InProgress))
        {
            _responseProgressTextIntelligence.Clear();
            _responseProgressTextIntelligence.Append($"LanguageModelResponseStatus: {finalLanguageModelResponse.Status}");
        }

        DispatchPropertyChanged(nameof(ResponseProgressTextIntelligence));
    }

    private void OnResultWithLoRA(object? sender, LanguageModelResponseResult finalLanguageModelResponse)
    {
        if ((finalLanguageModelResponse.Status != LanguageModelResponseStatus.Complete)
              && (finalLanguageModelResponse.Status != LanguageModelResponseStatus.InProgress))
        {
            _responseProgressWithLoRA.Clear();
            _responseProgressWithLoRA.Append($"LanguageModelResponseStatus: {finalLanguageModelResponse.Status}");
        }

        DispatchPropertyChanged(nameof(ResponseProgressWithLoRA));
    }

    private void OnResultWithoutLoRA(object? sender, LanguageModelResponseResult finalLanguageModelResponse)
    {
        if ((finalLanguageModelResponse.Status != LanguageModelResponseStatus.Complete)
              && (finalLanguageModelResponse.Status != LanguageModelResponseStatus.InProgress))
        {
            _responseProgressWithoutLoRA.Clear();
            _responseProgressWithoutLoRA.Append($"LanguageModelResponseStatus: {finalLanguageModelResponse.Status}");
        }

        DispatchPropertyChanged(nameof(ResponseProgressWithoutLoRA));
    }
}
