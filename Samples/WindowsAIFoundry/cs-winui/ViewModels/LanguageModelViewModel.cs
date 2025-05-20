// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
using WindowsAISampleSample.Models.Contracts;
using WindowsAISampleSample.Util;
using System.Collections.Generic;
using System.Text;
using System.Windows.Input;
using Microsoft.Windows.AI.ContentSafety;
using Microsoft.Windows.AI.Text;
using Microsoft.VisualBasic;
using System.Collections.ObjectModel;
using Microsoft.UI.Xaml.Data;
using System.Globalization;
using System;
using WindowsAISampleSample.Models;

namespace WindowsAISampleSample.ViewModels;

/// <summary>
/// A model to test the AI fabric Language Model API
/// </summary>
internal partial class LanguageModelViewModel : CopilotModelBase<LanguageModelModel>
{
    private string? _prompt;
    private string? _textIntelligencePrompt;
    private string? _embeddingPrompt;
    private string? _context;

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

    //private readonly AsyncCommandWithProgress<string, LanguageModelResponseResult, string> _generateResponseWithProgressCommand;
    //private readonly AsyncCommandWithProgress<string, LanguageModelResponseResult, string> _generateResponseWithOptionsAndProgressCommand;
    //private readonly AsyncCommandWithProgress<string, LanguageModelResponseResult, string> _generateResponseWithContextProgressCommand;

    private readonly AsyncCommandWithProgress<string, LanguageModelResponseResult, string> _generateResponseWithTextIntelligenceSummarizeSkills;
    private readonly AsyncCommandWithProgress<string, LanguageModelResponseResult, string> _generateResponseWithTextIntelligenceRewriteSkills;
    private readonly AsyncCommand<string, string> _generateResponseWithTextIntelligenceTextToTableSkills;

    //private readonly AsyncCommand<string, LanguageModelEmbeddingVectorResult> _generateEmbeddingVectorCommand;

    private readonly StringBuilder _responseProgress = new();
    private readonly StringBuilder _responseProgressTextIntelligence = new();

    public ObservableCollection<SeverityLevel> ContentFilterSeverityLevels { get; } = new ObservableCollection<SeverityLevel> {
        SeverityLevel.Minimum,
        SeverityLevel.Low,
        SeverityLevel.Medium,
        SeverityLevel.High
    };

    public LanguageModelViewModel(LanguageModelModel languageModelSession)
        : base(languageModelSession)
    {
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
                return  Session.GenerateResponseTextIntelligenceTextToTableAsync(prompt!);
            },
            (prompt) => IsAvailable && !string.IsNullOrEmpty(TextIntelligencePrompt));

    }

    public string ResponseProgress => _responseProgress.ToString();
    public string ResponseProgressTextIntelligence => _responseProgressTextIntelligence.ToString();

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

    public string? Context
    {
        get => _context;
        set => SetField(ref _context, value);
    }

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

    public ICommand GenerateResponseWithTextIntelligenceSummarizeSkills => _generateResponseWithTextIntelligenceSummarizeSkills;
    public ICommand GenerateResponseWithTextIntelligenceRewriteSkills => _generateResponseWithTextIntelligenceRewriteSkills;
    public ICommand GenerateResponseWithTextIntelligenceTextToTableSkills => _generateResponseWithTextIntelligenceTextToTableSkills;

    protected override void OnIsAvailableChanged()
    {
        //_generateResponseWithProgressCommand.FireCanExecuteChanged();
        //_generateResponseWithOptionsAndProgressCommand.FireCanExecuteChanged();
        _generateResponseWithTextIntelligenceSummarizeSkills.FireCanExecuteChanged();
        _generateResponseWithTextIntelligenceRewriteSkills.FireCanExecuteChanged();
        _generateResponseWithTextIntelligenceTextToTableSkills.FireCanExecuteChanged();
        //_generateResponseWithContextProgressCommand.FireCanExecuteChanged();
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
}
