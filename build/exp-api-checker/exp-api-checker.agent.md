---
name: exp-api-checker
description: |
  Detect and fix experimental API usage in Windows App SDK samples.
  
  Use this skill when:
  - Checking if sample code uses experimental APIs
  - Getting suggestions for wrapping exp code with #if WASDK_EXPERIMENTAL
  - Determining if a sample should be marked as exp-only
  
  Trigger words: exp api, experimental api, winappsdk experimental, exp check
---

# Exp API Checker Skill

Detects experimental API usage in Windows App SDK sample code and provides fix suggestions.

## Quick Start

### 1. Detect exp API usage

Run the detection script:

```powershell
./build/exp-api-checker/scripts/Detect-ExpApi.ps1 -Path <folder-or-file>
```

Example:
```powershell
./build/exp-api-checker/scripts/Detect-ExpApi.ps1 -Path ./Samples/WindowsAIFoundry/cs-winui
```

### 2. Generate fix prompt for LLM

```powershell
./build/exp-api-checker/scripts/Build-FixPrompt.ps1 -Path <folder-or-file> -OutputDir <out>
```

Each affected file produces a complete LLM prompt (combining file content + detection + template).
Paste a prompt into Copilot Chat / Claude / GPT to receive a specific diff suggestion.

## Workflow

```
1. Run Detect-ExpApi.ps1
   ↓
2. Run Build-FixPrompt.ps1 (combines detection + file content + template)
   ↓
3. Send prompt to LLM, receive diff
   ↓
4. Apply diff:
   - HYBRID: Add #if WASDK_EXPERIMENTAL (and #else stub for public methods)
   - EXP-ONLY: Set <WindowsAppSDKExperimental>true</WindowsAppSDKExperimental> in Directory.Build.props
```

## Repo Convention

- **Macro name: `WASDK_EXPERIMENTAL`** (defined in `Samples/Directory.Build.props` when `WindowsAppSDKExperimental=true`)
- Stable build promotes `CS8305` ("experimental API") to an error, so unguarded usage fails the build
- Per-sample opt-in: `<WindowsAppSDKExperimental>true</WindowsAppSDKExperimental>` in its own `Directory.Build.props`

## Exp-Only Namespaces (as of 2.1.3 vs 2.1.4-experimental8)

| Namespace | Description |
|-----------|-------------|
| `Microsoft.Windows.AI.Imaging` | AI image processing |
| `Microsoft.Windows.AI.Text.Experimental` | AI text/language models (LoRA etc.) |
| `Microsoft.Windows.AI.Video` | AI video processing |
| `Microsoft.Windows.Search.AppContentIndex` | App content search |
| `Microsoft.UI.Composition.Experimental` | Experimental composition |
| `Microsoft.UI.Input.Experimental` | Experimental input |

## Fix Patterns

### HYBRID - internal usage (`using`, fields, init code)

```csharp
#if WASDK_EXPERIMENTAL
using Microsoft.Windows.AI.Text.Experimental;
#endif

internal class LanguageModelModel
{
#if WASDK_EXPERIMENTAL
    private LanguageModelExperimental? _languageModelExperimental;
#endif

    public async Task InitAsync()
    {
        // ...stable code...
#if WASDK_EXPERIMENTAL
        _languageModelExperimental = new LanguageModelExperimental(_session);
#endif
    }
}
```

### HYBRID - public method with callers (use #if/#else stub)

Preserve the method signature in stable builds so callers do not need to change:

```csharp
#if WASDK_EXPERIMENTAL
public IAsyncOperationWithProgress<Result, string>
GenerateResponseWithLoraAdapter(string ctx, string prompt, string filePath)
{
    // real exp implementation
}
#else
public IAsyncOperationWithProgress<Result, string>
GenerateResponseWithLoraAdapter(string ctx, string prompt, string filePath)
    => throw new NotSupportedException("LoRA adapter support requires the experimental Windows App SDK.");
#endif
```

### EXP-ONLY Sample (entire sample needs exp)

```xml
<!-- Samples/<MySample>/Directory.Build.props -->
<Project>
  <Import Project="$([MSBuild]::GetPathOfFileAbove('Directory.Build.props', '$(MSBuildThisFileDirectory)../'))" />
  <PropertyGroup>
    <WindowsAppSDKExperimental>true</WindowsAppSDKExperimental>
    <NoWarn>$(NoWarn);CS8305</NoWarn>
  </PropertyGroup>
</Project>
```

## Updating the API List

When a new WinAppSDK version is released:

```powershell
cd build/winmd-tools
./Get-LatestVersions.ps1
./Compare-WinAppSdkApis.ps1 -StableVersion "X.Y.Z" -ExperimentalVersion "X.Y.Z-ExperimentalN"

# Copy updated list to checker
Copy-Item output/exp-only-apis.json ../exp-api-checker/scripts/
```

