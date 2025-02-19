// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
using WindowsCopilotRuntimeSample.Util;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Documents;
using Microsoft.UI.Xaml.Media;
using System.Collections.Generic;
using System.Text.RegularExpressions;
using Windows.ApplicationModel.DataTransfer;

namespace WindowsCopilotRuntimeSample.Controls;

public sealed partial class CodeBlockControl : UserControl
{
    public static readonly DependencyProperty SourceFileProperty =
        DependencyProperty.Register("SourceFile", typeof(string), typeof(CodeBlockControl), new PropertyMetadata(default(string), OnSourceFileChanged));
    private string _content = string.Empty;

    public string SourceFile
    {
        get { return (string)GetValue(SourceFileProperty); }
        set { SetValue(SourceFileProperty, value); }
    }

    public CodeBlockControl()
    {
        InitializeComponent();
    }

    private static void OnSourceFileChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
    {
        var control = (CodeBlockControl)d;
        control.ExampleCode_Loaded(control, new RoutedEventArgs());
    }

    public async void ExampleCode_Loaded(object sender, RoutedEventArgs e)
    {
        if (SourceFile != null)
        {
            _content = await SourceFile.ReadTextAsync();
            codeBlock.Blocks.Clear();
            codeBlock.Blocks.Add(HighlightSyntax(_content));
        }
    }

    private void CopyButton_Click(object sender, RoutedEventArgs e)
    {
        var dataPackage = new DataPackage();
        dataPackage.SetText(_content);
        Clipboard.SetContent(dataPackage);
    }

    private static Paragraph HighlightSyntax(string code)
    {
        var paragraph = new Paragraph();
        var keywords = new HashSet<string> { "public", "private", "protected", "class", "void", "async", "await", "if", "else", "for", "while", "return", "new", "string", "int", "bool", "var" };
        var keywordRegex = new Regex(@"\b(" + string.Join("|", keywords) + @")\b");
        var methodRegex = new Regex(@"\b([a-zA-Z_][a-zA-Z0-9_]*)\s*(?=\()");

        // color scheme for one dark https://github.com/joshdick/onedark.vim
        var purple = Windows.UI.Color.FromArgb(0xFF, 0xC6, 0x78, 0xDD);
        var blue = Windows.UI.Color.FromArgb(0xFF, 0x61, 0xAF, 0xEF);
        // green for future highlighting
        _ = Windows.UI.Color.FromArgb(0xFF, 0x98, 0xC3, 0x79);
        var fontFamily = new FontFamily("Consolas");

        var matches = keywordRegex.Matches(code);
        int lastIndex = 0;

        foreach (Match match in matches)
        {
            if (match.Index > lastIndex)
            {
                var nonKeywordText = code[lastIndex..match.Index];
                var methodMatches = methodRegex.Matches(nonKeywordText);
                int nonKeywordLastIndex = 0;

                foreach (Match methodMatch in methodMatches)
                {
                    if (methodMatch.Index > nonKeywordLastIndex)
                    {
                        paragraph.Inlines.Add(new Run { Text = nonKeywordText[nonKeywordLastIndex..methodMatch.Index], FontFamily = fontFamily });
                    }

                    paragraph.Inlines.Add(new Run { Text = methodMatch.Value, Foreground = new SolidColorBrush(blue), FontFamily = fontFamily });
                    nonKeywordLastIndex = methodMatch.Index + methodMatch.Length;
                }

                if (nonKeywordLastIndex < nonKeywordText.Length)
                {
                    paragraph.Inlines.Add(new Run { Text = nonKeywordText[nonKeywordLastIndex..], FontFamily = fontFamily });
                }
            }

            paragraph.Inlines.Add(new Run { Text = match.Value, Foreground = new SolidColorBrush(purple), FontFamily = fontFamily });
            lastIndex = match.Index + match.Length;
        }

        if (lastIndex < code.Length)
        {
            var remainingText = code[lastIndex..];
            var remainingMethodMatches = methodRegex.Matches(remainingText);
            int remainingLastIndex = 0;

            foreach (Match remainingMethodMatch in remainingMethodMatches)
            {
                if (remainingMethodMatch.Index > remainingLastIndex)
                {
                    paragraph.Inlines.Add(new Run { Text = remainingText[remainingLastIndex..remainingMethodMatch.Index], FontFamily = fontFamily });
                }

                paragraph.Inlines.Add(new Run { Text = remainingMethodMatch.Value, Foreground = new SolidColorBrush(blue), FontFamily = fontFamily });
                remainingLastIndex = remainingMethodMatch.Index + remainingMethodMatch.Length;
            }

            if (remainingLastIndex < remainingText.Length)
            {
                paragraph.Inlines.Add(new Run { Text = remainingText[remainingLastIndex..], FontFamily = fontFamily });
            }
        }

        return paragraph;
    }
}
