# Stable 2.4 Dependency Unification

## Problem

The integration branch inherits package configuration from
`release/experimental`. The root package graph uses Windows App SDK 1.8,
WindowsML and WindowsAIFoundry use local Windows App SDK 2.1 graphs, and two
experimental-only samples use Windows App SDK 2.0 experimental packages.

The final stable branch must not imply that experimental-only samples are
supported by Windows App SDK 2.4. Local package files must also not duplicate
versions already owned by the root package graph.

The repository's public `WinAppSDK-SampleDeps` feed currently contains the
`Microsoft.WindowsAppSDK` 2.4.0 metapackage, but an uncached restore cannot
resolve these required packages:

- `Microsoft.WindowsAppSDK.Foundation` 2.3.9
- `Microsoft.WindowsAppSDK.InteractiveExperiences` 2.1.6
- `Microsoft.WindowsAppSDK.WinUI` 2.3.6
- `Microsoft.WindowsAppSDK.AI` 2.4.4
- `Microsoft.WindowsAppSDK.ML` 2.1.74
- `Microsoft.WindowsAppSDK.Search` 2.4.4
- `Microsoft.WindowsAppSDK.Runtime` 2.4.0

The source migration uses the official NuGet 2.4 closure through the
repository's `localpackages` validation path. Anonymous restore using only the
public sample feed remains a final CI gate.

## Goals

- Use `Microsoft.WindowsAppSDK` 2.4.0 from the root central package file for
  every retained sample.
- Remove local versions that duplicate root-owned packages.
- Keep WindowsML-only and ONNX packages in the WindowsML package file.
- Exclude samples that exist only in `release/experimental` and require
  experimental Windows App SDK APIs.
- Make `UpdateVersions.ps1` update the root package file by default and require
  explicit paths for any additional central package files.
- Validate retained sample families against the official 2.4 package closure.

## Non-Goals

- Move WindowsML-only or ONNX package versions into the root package file.
- Replace workload-managed MAUI package versions.
- Port experimental APIs to different stable APIs as part of the dependency
  update.
- Fix the separately tracked SqueezeNet parallel download race.

## Affected Areas

- `Samples/Directory.Packages.props`
- `Samples/WindowsML/Directory.Packages.props`
- `Samples/WindowsAIFoundry/cs-winui/Directory.Packages.props`
- `UpdateVersions.ps1`
- `Samples/AppContentSearch`
- `Samples/WinUI/ConditionalPredicate`
- Reconciliation design documents and project matrix

## Approach

1. Exclude `AppContentSearch` and `WinUI/ConditionalPredicate`. Neither sample
   exists in `release/2.0-stable`, and both local package files explicitly
   require experimental Windows App SDK packages.
2. Resolve the official Windows App SDK 2.4.0 dependency closure into an
   isolated local package source.
3. Update the root package file to Windows App SDK 2.4.0 and its matching split
   package versions.
4. Delete the WindowsAIFoundry C# WinUI package file so the project inherits
   the root graph.
5. Make the WindowsML package file import the root package file before its
   local items. Remove every local `PackageVersion` already defined by root,
   while retaining WindowsML-only and ONNX versions locally.
6. Keep MAUI `$(MauiVersion)` references workload-managed and document them as
   an exception rather than duplicating them in central package management.
7. Update `UpdateVersions.ps1` so its default scope is only the root package
   file. Additional package files require explicit caller-provided paths.
8. Build each affected retained sample family, then run the repository build.
9. Hydrate the public sample feed and prove anonymous restore before the final
   CI and `main` cutover.

## Risks

- Updating only the metapackage without all split packages makes clean CI
  restore fail even when a developer machine succeeds from its global cache.
- A child `Directory.Packages.props` does not automatically inherit its parent.
  WindowsML must explicitly import the root before duplicate versions are
  removed.
- WindowsML may require source changes for stable 2.4 API behavior even after
  restore succeeds.
- Centralizing a workload-managed package can conflict with installed MAUI
  workloads.

## Validation Plan

- Restore Windows App SDK 2.4.0 from the official package closure through
  `Samples/localpackages`.
- Confirm every retained project resolves `Microsoft.WindowsAppSDK` 2.4.0 and
  has no experimental Windows App SDK package.
- Confirm no deleted experimental project remains in solutions, documentation,
  or the project matrix as retained content.
- Run targeted builds for WindowsAIFoundry and WindowsML.
- Run `build.ps1` for the complete retained sample set.
- Inspect representative generated assets files to confirm root inheritance
  and WindowsML-local package ownership.
- Restore from an empty package cache using only `Samples/nuget.config` after
  feed hydration.

## Status

- Root central package management uses Windows App SDK 2.4.0 and the exact
  split versions resolved from the stable metapackage.
- WindowsAIFoundry C# WinUI inherits root central package management.
- WindowsML imports root and keeps only GenAI WinML and AbiWinRT versions
  locally.
- WindowsML and Widgets pass x64 Release builds.
- WindowsAIFoundry sparse WinForms and WPF pass x64 Release builds. The
  WinForms build required correcting an invalid
  `SplashScreen.scale-200 (1).png` PRI qualifier filename.
- WindowsAIFoundry C# WinUI and Windowing C# WinUI resolve the 2.4 package
  closure, then encounter the previously recorded missing
  `Microsoft.NETCore.App.Crossgen2.win-x64` 8.0.30 package.
- Public sample feed hydration and full retained-sample validation remain.
