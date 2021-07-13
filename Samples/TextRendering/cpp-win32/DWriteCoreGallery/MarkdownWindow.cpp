// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "Main.h"
#include "Helpers.h"
#include "Resource.h"
#include "MarkdownWindow.h"

MarkdownWindow::MarkdownWindow(HWND parentWindow, TextRenderer* textRenderer, uint32_t resourceId) :
    StaticTextWindow{ parentWindow, textRenderer, CreateTextLayoutsFromPseudoMarkdown(LoadMarkdownResource(resourceId)) }
{
}

std::span<char const> LoadMarkdownResource(uint32_t resourceId)
{
    HRSRC info = FindResourceW(g_hInstance, MAKEINTRESOURCE(resourceId), MAKEINTRESOURCE(ID_MARKDOWN_RESOURCE));
    THROW_LAST_ERROR_IF_NULL(info);

    HGLOBAL handle = LoadResource(g_hInstance, info);
    THROW_LAST_ERROR_IF_NULL(handle);

    auto size = SizeofResource(g_hInstance, info);
    THROW_LAST_ERROR_IF(size == 0);

    auto data = static_cast<char const*>(LockResource(handle));
    THROW_LAST_ERROR_IF_NULL(data);

    return { data, size };
}

namespace
{
    _Ret_range_(begin, end) char const* SkipSpaces(_In_reads_(end - begin) char const* begin, char const* end) noexcept
    {
        return std::find_if(begin, end, [](char ch) { return ch != ' '; });
    }

    _Ret_range_(begin, end) char const* FindNewLine(_In_reads_(end - begin) char const* begin, char const* end) noexcept
    {
        return std::find_if(begin, end, [](char ch) { return ch == '\n' || ch == '\r'; });
    }

    _Ret_range_(begin, end) char const* SkipNewLine(_In_reads_(end - begin) char const* begin, char const* end) noexcept
    {
        auto p = begin;
        if (p != end && *p == '\r') { ++p; }
        if (p != end && *p == '\n') { ++p; }
        return p;
    }
}

MarkdownBlockType ParsePseudoMarkdownBlock(
    _In_reads_(inputEnd - inputPos) char const*& inputPos,
    char const* inputEnd,
    _Out_ std::wstring& text,
    _Out_ std::vector<DWRITE_TEXT_RANGE>& boldRanges,
    _Out_ std::vector<DWRITE_TEXT_RANGE>& italicRanges,
    _Out_ std::vector<DWRITE_TEXT_RANGE>& codeRanges
)
{
    // Clear all the outputs.
    text.clear();
    boldRanges.clear();
    italicRanges.clear();
    codeRanges.clear();

    if (inputPos == inputEnd)
    {
        return MarkdownBlockType::Body;
    }

    // Let pos be the current character position in the input.
    char const* pos = inputPos;

    // Determine the block type based on the first character.
    auto blockType = MarkdownBlockType::Body;
    if (*pos == '#')
    {
        blockType = MarkdownBlockType::Heading;
        pos = SkipSpaces(pos + 1, inputEnd);
    }
    else if (*pos == '`')
    {
        blockType = MarkdownBlockType::Code;
        pos = SkipNewLine(FindNewLine(pos, inputEnd), inputEnd);
    }
    else
    {
        pos = SkipSpaces(pos, inputEnd);
    }

    // Pointer to the first character that has not yet been added to the output text.
    char const* outputTextPos = pos;

    // Helper to add output characters between addTextPos and the current position.
    auto AddText = [&]()
    {
        int length = static_cast<int>(pos - outputTextPos);
        if (length > 0)
        {
            uint32_t wideLength = MultiByteToWideChar(CP_UTF8, 0, outputTextPos, length, nullptr, 0);
            if (wideLength != 0)
            {
                size_t destIndex = text.size();
                text.resize(destIndex + wideLength);
                MultiByteToWideChar(CP_UTF8, 0, outputTextPos, length, &text[destIndex], wideLength);
            }
        }
        outputTextPos = pos;
    };

    // Current character properties.
    struct InlineSpan
    {
        std::vector<DWRITE_TEXT_RANGE>& textRanges;
        uint32_t startPos;
        bool isInSpan;

        void ProcessMarkup(std::wstring const& text)
        {
            if (!isInSpan)
            {
                startPos = static_cast<uint32_t>(text.size());
            }
            else
            {
                uint32_t length = static_cast<uint32_t>(text.size() - startPos);
                textRanges.push_back(DWRITE_TEXT_RANGE{ startPos, length });
            }
            isInSpan = !isInSpan;
        }
    };
    InlineSpan boldSpan = { boldRanges };
    InlineSpan italicSpan = { italicRanges };
    InlineSpan codeSpan = { codeRanges };

    // Assume initially that the end of the block is the end of the input.
    auto blockEnd = inputEnd;

    // Iterate until we reach the end of the block.
    while (pos != blockEnd)
    {
        switch (*pos)
        {
        case '\r':
        case '\n':
            // Add any pending text up to the newline.
            AddText();

            // Advance past the newline.
            pos = SkipNewLine(pos + 1, blockEnd);

            if (blockType == MarkdownBlockType::Code)
            {
                // Check for end of code block.
                if (pos < blockEnd && *pos == '`')
                {
                    pos = SkipNewLine(FindNewLine(pos, blockEnd), blockEnd);
                    outputTextPos = pos;
                    blockEnd = pos;
                }
            }
            else
            {
                outputTextPos = pos;

                // Skip blank lines.
                while (pos < blockEnd && (*pos == '\r' || *pos == '\n'))
                {
                    ++pos;
                }

                // Determine whether to end the block.
                if (blockType == MarkdownBlockType::Heading || pos > outputTextPos)
                {
                    // End the current block, either because it's a heading or because there
                    // were multiple newlines.
                    outputTextPos = pos;
                    blockEnd = pos;
                }
                else
                {
                    // Add a single space in place of the newline, and continue the current block.
                    if (!text.empty() && text.back() != ' ')
                    {
                        text += L' ';
                    }
                }
            }
            break;

        case '*':
            if (blockType == MarkdownBlockType::Body)
            {
                // Add any pending text up to the inline markup.
                AddText();

                // Check whether it's **bold** or *italic*.
                if (pos + 1 < blockEnd && pos[1] == '*')
                {
                    pos += 2;
                    boldSpan.ProcessMarkup(text);
                }
                else
                {
                    pos++;
                    italicSpan.ProcessMarkup(text);
                }

                // Do not include the markup in the output text.
                outputTextPos = pos;
            }
            else
            {
                // Treat '*' in non-body text as plain text.
                ++pos;
            }
            break;

        case '`':
            if (blockType == MarkdownBlockType::Body)
            {
                // Add any pending text up to the inline markup.
                AddText();

                // Begin or end the inline code range.
                pos++;
                codeSpan.ProcessMarkup(text);

                // Do not include the markup in the output text.
                outputTextPos = pos;
            }
            else
            {
                // Treat '`' in non-body text as plain text.
                ++pos;
            }
            break;

        default:
            // Advance past plain text character.
            ++pos;
            break;
        }
    }

    AddText();
    inputPos = pos;

    return blockType;
}

namespace
{
    const DWRITE_FONT_AXIS_VALUE g_defaultHeadingAxisValues[] = { { DWRITE_FONT_AXIS_TAG_WEIGHT, 600 }, { DWRITE_FONT_AXIS_TAG_ITALIC, 0 } };
    const DWRITE_FONT_AXIS_VALUE g_defaultBodyAxisValues[] = { { DWRITE_FONT_AXIS_TAG_WEIGHT, 400 }, { DWRITE_FONT_AXIS_TAG_ITALIC, 0 } };
    const DWRITE_FONT_AXIS_VALUE g_defaultBoldAxisValues[] = { { DWRITE_FONT_AXIS_TAG_WEIGHT, 700 }, { DWRITE_FONT_AXIS_TAG_ITALIC, 0 } };
    const DWRITE_FONT_AXIS_VALUE g_defaultItalicAxisValues[] = { { DWRITE_FONT_AXIS_TAG_WEIGHT, 400 }, { DWRITE_FONT_AXIS_TAG_ITALIC, 1 } };
}

const MarkdownStyle g_defaultStyle =
{
    L"Sitka",       // headingFamilyName 
    L"Sitka",       // bodyFamilyName
    L"Consolas",    // codeFamilyName

    g_defaultHeadingAxisValues,
    g_defaultBodyAxisValues,
    g_defaultBoldAxisValues,
    g_defaultItalicAxisValues,

    28.0f,  // headingFontSize
    14.0f,  // bodyFontSize
    13.0f   // codeFontSize
};

std::vector<wil::com_ptr<IDWriteTextLayout4>> CreateTextLayoutsFromPseudoMarkdown(std::span<char const> inputText, MarkdownStyle style)
{
    std::vector<wil::com_ptr<IDWriteTextLayout4>> result;

    auto headingFormat = CreateTextFormat(style.headingFamilyName, style.headingFontSize, style.headingAxisValues);
    auto bodyFormat = CreateTextFormat(style.bodyFamilyName, style.bodyFontSize, style.bodyAxisValues);
    auto codeFormat = CreateTextFormat(style.codeFamilyName, style.codeFontSize, style.bodyAxisValues);

    THROW_IF_FAILED(codeFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP));

    auto GetTextFormat = [&](MarkdownBlockType blockType)
    {
        switch (blockType)
        {
        case MarkdownBlockType::Body:     return bodyFormat.get();
        case MarkdownBlockType::Heading:  return headingFormat.get();
        case MarkdownBlockType::Code:     return codeFormat.get();
        default: THROW_HR(E_INVALIDARG);
        }
    };

    std::wstring text;
    std::vector<DWRITE_TEXT_RANGE> boldRanges;
    std::vector<DWRITE_TEXT_RANGE> italicRanges;
    std::vector<DWRITE_TEXT_RANGE> codeRanges;

    char const* inputPos = inputText.data();
    char const* inputEnd = inputPos + inputText.size();

    while (inputPos < inputEnd)
    {
        auto blockType = ParsePseudoMarkdownBlock(inputPos, inputEnd, text, boldRanges, italicRanges, codeRanges);
        auto textLayout = CreateTextLayout(GetTextFormat(blockType), text);

        for (DWRITE_TEXT_RANGE textRange : boldRanges)
        {
            THROW_IF_FAILED(textLayout->SetFontAxisValues(style.boldAxisValues.data(), static_cast<uint32_t>(style.boldAxisValues.size()), textRange));
        }

        for (DWRITE_TEXT_RANGE textRange : italicRanges)
        {
            THROW_IF_FAILED(textLayout->SetFontAxisValues(style.italicAxisValues.data(), static_cast<uint32_t>(style.italicAxisValues.size()), textRange));
        }

        for (DWRITE_TEXT_RANGE textRange : codeRanges)
        {
            THROW_IF_FAILED(textLayout->SetFontFamilyName(style.codeFamilyName, textRange));
            THROW_IF_FAILED(textLayout->SetFontSize(style.codeFontSize, textRange));
        }

        result.push_back(std::move(textLayout));
    }

    return result;
}

