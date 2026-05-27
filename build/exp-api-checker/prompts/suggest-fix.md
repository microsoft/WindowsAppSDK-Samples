# Exp API Fix Suggestion Prompt

You are analyzing Windows App SDK sample code that uses experimental APIs. Your task is to suggest how to properly wrap the experimental code.

## Context

The following file uses experimental-only APIs that are not available in the stable WinAppSDK package:

**File:** {{FILE_PATH}}
**Exp Namespaces Used:** {{NAMESPACES}}

## Detection Results

{{DETECTION_RESULTS}}

## File Content

```{{LANGUAGE}}
{{FILE_CONTENT}}
```

## Your Task

Analyze the code and determine:

1. **Classification**: Is this a HYBRID sample (some features use exp API) or EXP-ONLY sample (entire sample depends on exp API)?

2. **Suggested Fix**: Provide specific code changes.

### Repo Convention (IMPORTANT)

- **Macro name is `WASDK_EXPERIMENTAL`** (NOT `WINAPPSDK_EXPERIMENTAL`).
- The macro is defined centrally in `Samples/Directory.Build.props` when `WindowsAppSDKExperimental=true`:
  - C# via `DefineConstants`
  - C++ via `PreprocessorDefinitions`
- Per-sample `Directory.Build.props` opts in by setting `<WindowsAppSDKExperimental>true</WindowsAppSDKExperimental>` and chaining to the root via `Import Project="$([MSBuild]::GetPathOfFileAbove('Directory.Build.props', '$(MSBuildThisFileDirectory)../'))"`.
- The root also adds `<WarningsAsErrors>$(WarningsAsErrors);CS8305</WarningsAsErrors>` so any unguarded exp API usage fails the stable build.

### For HYBRID samples

- Wrap exp-only `using` statements with `#if WASDK_EXPERIMENTAL ... #endif`
- Wrap exp-only fields / properties with `#if WASDK_EXPERIMENTAL ... #endif`
- For **public methods that callers reference**, use `#if WASDK_EXPERIMENTAL ... #else <stub> ... #endif` so the method signature is preserved in the stable build (callers do not need to change). The `#else` branch should throw `NotSupportedException("... requires the experimental Windows App SDK.")`.
- For **internal initialization code** (e.g., setting `_experimentalSession = new(...)`) just use `#if WASDK_EXPERIMENTAL ... #endif` — no `#else` needed.
- Minimize the scope of `#if` blocks — do not blanket-wrap an entire file unless every member is exp-only.

### For EXP-ONLY samples

- The entire sample's `Directory.Build.props` sets `<WindowsAppSDKExperimental>true</WindowsAppSDKExperimental>` permanently
- Source can still use `#if WASDK_EXPERIMENTAL` guards for clarity, but they will always be true

## Output Format

```markdown
## Classification
[HYBRID / EXP-ONLY]

## Reasoning
[Brief explanation of why you classified it this way]

## Suggested Changes

### File: [filename]
```diff
[Show the diff with + and - lines]
```

### Additional Files (if needed)
[e.g., .csproj changes]
```

## Important Notes

- Keep the code working for both stable and experimental builds
- Minimize the scope of `#if` blocks
- Preserve code readability
- Do not change unrelated code
