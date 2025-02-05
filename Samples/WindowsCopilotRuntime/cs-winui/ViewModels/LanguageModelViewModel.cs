﻿// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
using WindowsCopilotRuntimeSample.Models.Contracts;
using WindowsCopilotRuntimeSample.Util;
using Microsoft.Windows.SemanticSearch;
using System.Collections.Generic;
using System.Text;
using System.Windows.Input;
using Microsoft.Windows.AI.ContentModeration;
using Microsoft.Windows.AI.Generative;
using Microsoft.VisualBasic;
using System.Collections.ObjectModel;
using Microsoft.UI.Xaml.Data;
using System.Globalization;
using System;
using WindowsCopilotRuntimeSample.Models;

namespace WindowsCopilotRuntimeSample.ViewModels;

/// <summary>
/// A model to test the AI fabric Language Model API
/// </summary>
internal partial class LanguageModelViewModel : CopilotModelBase<LanguageModelModel>
{
    private string? _prompt;
    private string? _promptTextIntelligence;
    private string? _embeddingPrompt;
    private string? _context;

    // LanguageModelOptions
    private LanguageModelSkill _languageModelOptionsSkill = LanguageModelSkill.General;
    private string? _languageModelOptionsTemp = "0.9";
    private string? _languageModelOptionsTopP = "0.9";
    private string? _languageModelOptionsTopK = "40";

    // ContentFilterOptions - Prompt min severity to block
    private SeverityLevel _hateContentSevToBlockForPrompt = SeverityLevel.Medium;
    private SeverityLevel _sexualContentSevToBlockForPrompt = SeverityLevel.Medium;
    private SeverityLevel _violentContentSevToBlockForPrompt = SeverityLevel.Medium;
    private SeverityLevel _selfHarmContentSevToBlockForPrompt = SeverityLevel.Medium;

    // ContentFilterOptions - Response min severity to block
    private SeverityLevel _hateContentSevToBlockForResponse = SeverityLevel.Medium;
    private SeverityLevel _sexualContentSevToBlockForResponse = SeverityLevel.Medium;
    private SeverityLevel _violentContentSevToBlockForResponse = SeverityLevel.Medium;
    private SeverityLevel _selfHarmContentSevToBlockForResponse = SeverityLevel.Medium;

    private readonly AsyncCommand<string, string> _generateResponseCommand;
    private readonly AsyncCommandWithProgress<string, LanguageModelResponse, string> _generateResponseWithProgressCommand;
    private readonly AsyncCommandWithProgress<string, LanguageModelResponse, string> _generateResponseWithOptionsAndProgressCommand;
    private readonly AsyncCommandWithProgress<string, LanguageModelResponse, string> _generateResponseWithContextProgressCommand;
    private readonly AsyncCommandWithProgress<string, LanguageModelResponse, string> _generateResponseWithTextIntelligenceSkillsCommand;

    private readonly AsyncCommand<string, IReadOnlyList<EmbeddingVector>> _generateEmbeddingVectorCommand;

    private readonly StringBuilder _responseProgress = new();
    private readonly StringBuilder _responseProgressTextIntelligence = new();

    public ObservableCollection<SeverityLevel> ContentFilterSeverityLevels { get; } = new ObservableCollection<SeverityLevel> {
        SeverityLevel.Low,
        SeverityLevel.Medium
    };

    public ObservableCollection<LanguageModelSkill> LanguageModelSkills { get; } = new ObservableCollection<LanguageModelSkill> {
        LanguageModelSkill.General,
        LanguageModelSkill.TextToTable,
        LanguageModelSkill.Summarize,
        LanguageModelSkill.Rewrite
    };

    public LanguageModelViewModel(LanguageModelModel languageModelSession)
        : base(languageModelSession)
    {
        // GenerateResponse
        _generateResponseCommand = new(
            async prompt =>
            {
                return await Session.GenerateResponseAsync(prompt!);
            },
            (prompt) => IsAvailable && !string.IsNullOrEmpty(prompt));

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
                    HateContentSeverity = HateContentSevToBlockForPrompt,
                    SexualContentSeverity = SexualContentSevToBlockForPrompt,
                    ViolentContentSeverity = ViolentContentSevToBlockForPrompt,
                    SelfHarmContentSeverity = SelfHarmContentSevToBlockForPrompt
                };
                
                var responseMinSeverityLevelToBlock = new TextContentFilterSeverity {
                    HateContentSeverity = HateContentSevToBlockForResponse,
                    SexualContentSeverity = SexualContentSevToBlockForResponse,
                    ViolentContentSeverity = ViolentContentSevToBlockForResponse,
                    SelfHarmContentSeverity = SelfHarmContentSevToBlockForResponse
                };

                var contentFilterOptions = new ContentFilterOptions {
                    PromptMinSeverityLevelToBlock = promptMinSeverityLevelToBlock,
                    ResponseMinSeverityLevelToBlock = responseMinSeverityLevelToBlock
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

        // GenerateResponseWithTextIntelligenceSkills
        _generateResponseWithTextIntelligenceSkillsCommand = new(
            prompt =>
            {
                var languageModelOptions = new LanguageModelOptions {
                    Skill = LanguageModelOptionsSkill,
                    Temp = (string.IsNullOrEmpty(LanguageModelOptionsTemp) ? 0.9f : float.Parse(LanguageModelOptionsTemp, CultureInfo.InvariantCulture)),
                    Top_p = (string.IsNullOrEmpty(LanguageModelOptionsTopP) ? 0.9f : float.Parse(LanguageModelOptionsTopP, CultureInfo.InvariantCulture)),
                    Top_k = (uint)(string.IsNullOrEmpty(LanguageModelOptionsTopK) ? 40 : int.Parse(LanguageModelOptionsTopK, CultureInfo.InvariantCulture))
                };

                _responseProgressTextIntelligence.Clear();
                DispatchPropertyChanged(nameof(_responseProgressTextIntelligence));

                return Session.GenerateResponseWithOptionsAndProgressAsync(prompt!, languageModelOptions, null /*content filter option*/);
            },
            (prompt) => IsAvailable && !string.IsNullOrEmpty(prompt));

        _generateResponseWithTextIntelligenceSkillsCommand.ResultProgressHandler += OnResultProgressTextIntelligence;
        _generateResponseWithTextIntelligenceSkillsCommand.ResultHandler += OnResultTextIntelligence;

        // GenerateEmbedding
        _generateEmbeddingVectorCommand = new(
            async prompt =>
            {
                return await Session.GenerateEmbeddingVectorAsync(prompt!);
            },
            (prompt) => IsAvailable && !string.IsNullOrEmpty(prompt));
    }

    public string ResponseProgress => _responseProgress.ToString();
    public string ResponseProgressTextIntelligence => _responseProgressTextIntelligence.ToString();

    public string? Prompt
    {
        get => _prompt;
        set
        {
            SetField(ref _prompt, value);
            _generateResponseCommand.FireCanExecuteChanged();
        }
    }

    public string? PromptTextIntelligence
    {
        get => _promptTextIntelligence;
        set
        {
            SetField(ref _promptTextIntelligence, value);
            _generateResponseWithTextIntelligenceSkillsCommand.FireCanExecuteChanged();
        }
    }
    public string? EmbeddingPrompt
    {
        get => _embeddingPrompt;
        set
        {
            SetField(ref _embeddingPrompt, value);
            _generateResponseCommand.FireCanExecuteChanged();
        }
    }

    public string? Context
    {
        get => _context;
        set => SetField(ref _context, value);
    }

    public LanguageModelSkill LanguageModelOptionsSkill
    {
        get => _languageModelOptionsSkill;
        set => SetField(ref _languageModelOptionsSkill, value);
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
    public ICommand GenerateResponseCommand => _generateResponseCommand;

    public ICommand GenerateResponseWithProgressCommand => _generateResponseWithProgressCommand;

    public ICommand GenerateResponseWithOptionsAndProgressCommand => _generateResponseWithOptionsAndProgressCommand;
    public ICommand GenerateResponseWithTextIntelligenceSkillsCommand => _generateResponseWithTextIntelligenceSkillsCommand;
    public ICommand GenerateResponseWithContextProgressCommand  => _generateResponseWithContextProgressCommand;

    /// <summary>
    /// Exercise the GenerateEmbeddingVector method API for a language model session
    /// </summary>
    public ICommand GenerateEmbeddingVectorCommand => _generateEmbeddingVectorCommand;

    protected override void OnIsAvailableChanged()
    {
        _generateResponseCommand.FireCanExecuteChanged();
        _generateResponseWithProgressCommand.FireCanExecuteChanged();
        _generateResponseWithOptionsAndProgressCommand.FireCanExecuteChanged();
        _generateResponseWithTextIntelligenceSkillsCommand.FireCanExecuteChanged();
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

    private void OnResult(object? sender, LanguageModelResponse finalLanguageModelResponse)
    {
        if ((finalLanguageModelResponse.Status != LanguageModelResponseStatus.Complete)
              && (finalLanguageModelResponse.Status != LanguageModelResponseStatus.InProgress))
        {
            _responseProgress.Clear();
            _responseProgress.Append($"LanguageModelResponseStatus: {finalLanguageModelResponse.Status}");
        }

        DispatchPropertyChanged(nameof(ResponseProgress));
    }

    private void OnResultTextIntelligence(object? sender, LanguageModelResponse finalLanguageModelResponse)
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
