# GitHub Copilot Instructions (WindowsAppSDK-Samples)

Read scripts first; improve their headers instead of duplicating detail here.

## Build
Use only `build.ps1` / `build.cmd`. Read `build.ps1` before invoking. It auto: detects platform, picks solutions (current dir > selected sample > all), initializes VS DevShell, restores via local nuget, emits `.binlog` per solution. Do NOT hand-roll msbuild/nuget restore logic.

## Versions
Run `UpdateVersions.ps1` only after reading it. Get the WinAppSDK version string from: https://www.nuget.org/packages/Microsoft.WindowsAppSdk/ (stable/preview/servicing) and pass as `-WinAppSDKVersion`.

## Coding Guidelines
- Small, focused diffs; no mass reformatting; ensure SOLID principles.
- Preserve APIs, encoding, line endings, relative paths.
- PowerShell: approved verbs; each new script has Synopsis/Description/Parameters/Examples header.
- C#/C++: follow existing style; lean headers; forward declare where practical;  keep samples illustrative.
- Minimal necessary comments; avoid noise. Centralize user strings for localization.
- Never log secrets or absolute external paths.

## Language style rules
- Always enforce repo analyzers: root `.editorconfig` plus any `stylecop.json`.
- C# code follows StyleCop.Analyzers and Microsoft.CodeAnalysis.NetAnalyzers.
- C++ code honors `.clang-format` plus `.clang-tidy` (modernize/cppcoreguidelines/readability).
- Markdown files wrap at 80 characters and use ATX headers with fenced code blocks that include language tags.
- YAML files indent two spaces and add comments for complex settings while keeping keys clear.
- PowerShell scripts use Verb-Noun names and prefer single-quoted literals while documenting parameters and satisfying PSScriptAnalyzer.

## PR Guidance
- One intent per PR. Update script README/header if behavior changes.
- Provide summary: what / why / validation.
- Run a targeted build (e.g. `pwsh -File build.ps1 -Sample AppLifecycle`).
- For version bumps: inspect at least one changed project file.
- No new warnings/errors or large cosmetic churn.

## Design Docs (Large / Cross-Sample)
Before broad edits create `DESIGN-<topic>.md`:
- Single sample: `Samples/<Sample>/DESIGN-<topic>.md`
- Multi-sample/shared: repo root.
Include: Problem, Goals/Non-Goals, Affected Areas, Approach, Risks, Validation Plan. Reference doc in PR.

## When Unsure
Draft a design doc or WIP PR summarizing assumptions—don't guess.

---
Keep this file lean; source-of-truth for behavior lives in script headers.