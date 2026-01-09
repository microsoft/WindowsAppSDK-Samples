# Windows App SDK Samples – AI Contributor Guide

Comprehensive guidance for AI contributions to Windows App SDK Samples.

## Quick Reference

- **Build**: Use `build.ps1` or `build.cmd` at repo root (auto-detects platform, supports `-Sample <name>` for targeted builds)
- **Run**: Deploy and test samples on Desktop (x64, x86, ARM64) in Debug and Release configurations
- **Verify**: Run Windows App Certification Kit (WACK) on Release builds
- **Exit code 0 = success** – do not proceed if build fails

### Build Examples
```powershell
./build.ps1                                          # Build all samples (auto-detect platform)
./build.ps1 -Sample AppLifecycle                     # Build only AppLifecycle sample
./build.ps1 -Platform arm64 -Configuration Debug    # Specific platform and config
```

## Key Rules

- Samples should be **complete but simple** – demonstrate correct API usage without excessive abstraction
- Follow **scenario-based design** – each scenario covers one way of using the API
- Support all platforms: x64, x86, ARM64 in both Debug and Release
- Set minimum supported OS version to Windows 10 version 1809 (build 17763)
- Include copyright headers in all source files
- Build clean with no warnings or errors

## Project Structure

Samples follow this organization:
```
\Samples
     \<FeatureName>
          \<Language>-<UI Framework>
```

- **FeatureName**: Use simple English name (e.g., `TextRendering` not `DWriteCore`)
- **Language**: `cs` | `cpp`
- **UI Framework (C#)**: `winui` | `wpf` | `winforms` | `console`
- **UI Framework (C++)**: `winui` | `win32` | `console` | `directx`

## Style Enforcement

- **C#**: Follow `.editorconfig` at repo root (4-space indent, Allman braces, UTF-8 BOM)
- **C++**: Use C++/WinRT for new samples; some samples include `.clang-format` (check `Samples/WindowsML/`)
- **XAML**: Use consistent formatting

## Copyright Headers

For C#/C++/IDL files:
```csharp
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
```

For XAML files:
```xml
<!-- Copyright (c) Microsoft Corporation.
     Licensed under the MIT License. -->
```

## When to Ask for Clarification

- Ambiguous sample requirements after reviewing docs
- Cross-feature impact unclear
- API usage patterns not well documented

## Detailed Documentation

- [Samples Guidelines](docs/samples-guidelines.md) – Complete guidelines and checklist
- [Contributing](CONTRIBUTING.md) – Contribution requirements
- [PR Template](docs/pull_request_template.md) – Pull request checklist
- [Windows App SDK Docs](https://docs.microsoft.com/windows/apps/windows-app-sdk/) – Official documentation
