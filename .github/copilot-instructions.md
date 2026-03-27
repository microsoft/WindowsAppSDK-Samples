---
description: 'Windows App SDK Samples AI contributor guidance'
---

# Windows App SDK Samples – Copilot Instructions

Concise guidance for AI contributions to Windows App SDK Samples. For complete details, see [AGENTS.md](../AGENTS.md).

## Quick Reference

- **Build**: `build.ps1` / `build.cmd` (supports `-Sample <name>` for targeted builds)
- **Run**: Test on x64, x86, ARM64 in Debug and Release
- **Verify**: Run WACK on Release builds
- **Exit code 0 = success** – do not proceed if build fails

## Key Rules

- Samples should be **complete but simple** – demonstrate correct API usage
- Follow **scenario-based design** – one scenario per API usage pattern
- Support all platforms: x64, x86, ARM64 (Debug and Release)
- Minimum OS: Windows 10 version 1809 (build 17763)
- Include copyright headers; build clean with no warnings

## Style Enforcement

- **C#**: Follow `.editorconfig` at repo root
- **C++**: Use C++/WinRT; check `Samples/WindowsML/` for `.clang-format`

## When to Ask for Clarification

- Ambiguous sample requirements after reviewing docs
- Cross-feature impact unclear
- API usage patterns not well documented

## Detailed Documentation

- [AGENTS.md](../AGENTS.md) – Full AI contributor guide
- [Samples Guidelines](../docs/samples-guidelines.md) – Complete guidelines and checklist
- [Contributing](../CONTRIBUTING.md) – Contribution requirements
- [PR Template](../docs/pull_request_template.md) – Pull request checklist
- [Windows App SDK Docs](https://docs.microsoft.com/windows/apps/windows-app-sdk/) – Official documentation
