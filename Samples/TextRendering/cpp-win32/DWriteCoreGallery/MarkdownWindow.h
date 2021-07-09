// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#include "StaticTextWindow.h"

class MarkdownWindow : public StaticTextWindow
{
public:
    MarkdownWindow(HWND parentWindow, TextRenderer* textRenderer, uint32_t resourceId);
};

/// <summary>
/// Block type returned by pseudo-markdown parser.
/// </summary>
enum class MarkdownBlockType
{
    Body,
    Heading,
    Code
};

/// <summary>
/// Styling properties for creating text layouts from pseudo-Markdown.
/// </summary>
struct MarkdownStyle
{
    wchar_t const* headingFamilyName;
    wchar_t const* bodyFamilyName;
    wchar_t const* codeFamilyName;

    std::span<const DWRITE_FONT_AXIS_VALUE> headingAxisValues;
    std::span<const DWRITE_FONT_AXIS_VALUE> bodyAxisValues;
    std::span<const DWRITE_FONT_AXIS_VALUE> boldAxisValues;
    std::span<const DWRITE_FONT_AXIS_VALUE> italicAxisValues;

    float headingFontSize;
    float bodyFontSize;
    float codeFontSize;
};
extern const MarkdownStyle g_defaultStyle;

/// <summary>
/// Loads a binary resource of type ID_MARKDOWN_RESOURCE.
/// </summary>
/// <param name="resourceId">Integer ID of the binary resource.</param>
/// <returns></returns>
std::span<char const> LoadMarkdownResource(uint32_t resourceId);

/// <summary>
/// Parses one block of pseudo-markdown from an array of input characters. Note this only supports a
/// simplified subset of Markdown, hence "pseudo-markdown".
/// </summary>
/// <param name="inputPos">On entry, points to the start of the UTF-8 input text. On return, points to the end of the first block.</param>
/// <param name="inputEnd">Points to the end of the input text.</param>
/// <param name="text">Receives the parsed text, converted from UTF-8 and with markup removed.</param>
/// <param name="boldRanges">Receives the text ranges that should be styled as bold.</param>
/// <param name="italicRanges">Receives the text ranges that should be styled as italic.</param>
/// <param name="codeRanges">Receives the text ranges that should be styled as code.</param>
/// <returns>Returns the block type -- either body or heading.</returns>
MarkdownBlockType ParsePseudoMarkdownBlock(
    _In_reads_(inputEnd - inputPos) char const*& inputPos,
    char const* inputEnd,
    _Out_ std::wstring& text,
    _Out_ std::vector<DWRITE_TEXT_RANGE>& boldRanges,
    _Out_ std::vector<DWRITE_TEXT_RANGE>& italicRanges,
    _Out_ std::vector<DWRITE_TEXT_RANGE>& codeRanges
);

/// <summary>
/// Converts pseudo-markdown text to a vector of text layout objects.
/// </summary>
/// <param name="text">Pseudo-markdown text to parse.</param>
/// <returns>Returns a vector of text layout objects.</returns>
std::vector<wil::com_ptr<IDWriteTextLayout4>> CreateTextLayoutsFromPseudoMarkdown(std::span<char const> text, MarkdownStyle style = g_defaultStyle);
