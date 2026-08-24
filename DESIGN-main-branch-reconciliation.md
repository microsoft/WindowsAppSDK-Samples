# Main Branch Reconciliation

## Problem

Development has continued independently on four branches:

- `main`
- `release/experimental`
- `release/2.0-experimental`
- `release/2.0-stable`

`main` is the default branch, but it does not contain all recent samples.
`release/experimental` has become the effective integration branch, while some
changes continued to land in `main`. The two 2.0 release branches were created
for release validation and also contain changes that are not present in
`release/experimental`.

Merging the branches by commit history would include obsolete version changes,
experimental-only APIs, duplicated cherry-picks, and changes superseded by
later implementations. Reconciliation must therefore be performed by sample
and by final code behavior rather than by commit SHA alone.

## Goals

- Make `release/experimental` the content baseline for reconciliation.
- Preserve all applicable stable samples from `main`,
  `release/experimental`, and `release/2.0-stable`.
- Preserve unique bug fixes and build fixes that remain applicable.
- Use stable package and API surfaces in the resulting `main` branch.
- Identify experimental-only samples explicitly instead of silently including
  or deleting them.
- Validate each migrated sample independently before the final cutover.
- Make `main` the single target for future sample contributions.

## Non-Goals

- Preserve every branch-specific package version or release workaround.
- Cherry-pick every commit that has a unique SHA.
- Move experimental-only samples into `main` without confirming API stability.
- Rewrite or delete the existing branch history.
- Update unrelated code or reformat samples during reconciliation.

## Baseline

The analysis and integration branch were created from these remote commits:

| Branch | Commit | Date |
|---|---|---|
| `main` | `18431c6d` | 2026-08-11 |
| `release/experimental` | `26eddcdf` | 2026-07-01 |
| `release/2.0-experimental` | `63efdc9a` | 2026-07-21 |
| `release/2.0-stable` | `5327f5c4` | 2026-07-02 |

The pinned full commit IDs are:

```text
main=18431c6d0111d6a4bd8d46b9c8a82add47ccb013
release/experimental=26eddcdf3240e6f0c89643ca3b7e303bbf3b6a78
release/2.0-experimental=63efdc9a60858dcb9d527569c371830fe58742cf
release/2.0-stable=5327f5c4e47e8661795fd5c25c120868405bdf22
```

The local integration branch is:

```text
integration/main-reconciliation
```

It is based on `release/experimental`.

If any source branch advances during reconciliation, new commits must be
recorded and classified separately. They must not be included implicitly.

## Branch Roles

| Branch | Reconciliation role |
|---|---|
| `release/experimental` | Content and directory-structure baseline |
| `main` | Source for missing stable samples and newer general fixes |
| `release/2.0-stable` | Stable API reference, especially for AI samples |
| `release/2.0-experimental` | Reference for experimental-only content |

`release/2.0-stable` is not the repository baseline because it intentionally
omits samples and contains older shared build and package settings. Its unique
patches are concentrated in `WindowsAIFoundry` and remove experimental APIs.

## Affected Areas

- Shared build and package configuration
- Sample project and solution files
- `WindowsAIFoundry`
- `WindowsML`
- Samples that exist only in `main`
- Samples that exist only in experimental branches
- CI workflows and sample indexes
- Root and sample documentation

Project boundaries are determined from `.csproj`, `.vcxproj`, `.wapproj`,
`.sln`, `.slnx`, and `CMakeLists.txt` files. Top-level `Samples` directories
alone are not sufficiently precise.

## Inclusion Criteria

A project is eligible for the resulting `main` branch when:

- It uses a released package version, unless an exception is documented.
- It does not require an experimental-only API.
- It has not been intentionally removed from the stable release branch.
- It builds with the repository's existing build scripts.
- Its source, documentation, and package versions describe the same behavior.

Each difference receives one of these actions:

| Classification | Action |
|---|---|
| Shared and equivalent | Keep the baseline version |
| General bug or build fix | Port the effective change |
| Stable sample missing from the baseline | Import and normalize |
| Stable API variant | Use as the sample's stable implementation |
| Version-line-specific change | Do not port |
| Superseded change | Record and omit |
| Experimental-only content | Exclude or isolate pending a decision |
| Conflicting implementation | Reconcile manually and test |

## Approach

### 1. Create the Project Matrix

Create a project-level inventory with:

- Presence on each of the four branches
- Current file-tree differences
- Package and SDK versions
- Stable or experimental API usage
- Unique change provenance
- Recommended action
- Build and smoke-test status

Use rename detection and patch equivalence to avoid treating moved projects or
cherry-picked commits as unrelated changes.

The generated inventory is stored in
`MAIN-BRANCH-RECONCILIATION-PROJECT-MATRIX.csv`. Its initial categories describe
tree relationships only. The `Disposition` and `Validation` columns remain
pending until the corresponding review and build steps are approved.

### 2. Reconcile Shared Infrastructure

Handle shared files before migrating individual samples:

- `build.ps1` and `build.cmd`
- `Directory.Build.props`
- `Directory.Packages.props`
- `Build.Common.Cpp.props`
- `nuget.config`
- CI workflows

Port the applicable C++20, Visual Studio 2026, and compiler-conformance fixes
from `main`. Retain the telemetry suppression and applicable bootstrap settings
from `release/experimental`. Do not copy package downgrades from
`release/2.0-stable` unless a sample requires and documents them.

#### Reconciliation Result

The shared infrastructure review retained the baseline build scripts, central
package management, telemetry suppression, and CI configuration. The Visual
Studio 2026 to `v145` mapping from `main` was already present in the baseline.

The remaining applicable changes from `main` PR #657 were ported:

- Use C++20 by default for C++ projects.
- Update the seven AppLifecycle projects that explicitly selected C++17.
- Fix dependent-base and member-template lookup in UXFrameworksOnIslands.
- Import the shared C++ toolset selection in SampleWidgetProviderApp.

The Widgets x64 Release build passed with no warnings. AppLifecycle and Islands
validation stopped before compilation because this machine does not have the
projects' explicitly selected Visual Studio 2022 `v143` toolset. This
environmental block is recorded in the project matrix and must be resolved
before final validation.

### 3. Import Low-Risk Stable Samples

Evaluate and import the stable projects that are present only in `main`,
including:

- `SecureUI`
- Stable Islands projects
- AppLifecycle packaging projects
- ResourceManagement packaging projects
- Windowing packaging projects

Each sample, or tightly coupled sample family, should be handled in a focused
pull request against the integration branch.

#### Candidate Review Result

The project-level review found two import candidates:

- Import `Islands/cs-winforms-unpackaged` together with
  `Islands/SampleWinUIClassLibrary`. They form one sample through a project
  reference and use the stable Windows App SDK 1.8 package.
- Import the complete `SecureUI` solution as one sample. It uses the stable
  Windows App SDK 1.6 package, but its legacy package restore and ARM64 project
  mappings require review during migration.

The other `main`-only directories are not independent samples:

- The AppLifecycle, ResourceManagement, and Windowing package directories are
  packaging-project splits of applications already present in the baseline.
  Defer them for per-sample packaging review instead of importing duplicates.
- `Islands/SimpleIslandApp/cpp-win32-unpackaged` is primarily a relocation and
  extension of the baseline `Islands/cpp-win32-unpackaged` sample. Reconcile it
  with the existing path instead of adding a second copy.
- The WinML EP Catalog remains in the dedicated WindowsML review.

This review did not import any sample code.

#### Import Status

- The Islands WinForms sample and its WinUI class library have been imported.
  Their x64 Release build passed with no warnings.
- SecureUI remains an approved candidate. Its commit is deferred until it can
  be built with the required Visual Studio 2022 `v143` toolset.

The low-risk sample import step remains incomplete until SecureUI is validated
and committed.

### 4. Stabilize WindowsAIFoundry

Use `release/2.0-stable` as the stable API reference for this sample:

- Remove experimental-only scenarios.
- Preserve applicable later fixes from `release/experimental`.
- Normalize to approved stable package versions.
- Avoid replacing unrelated shared configuration with older release settings.

### 5. Reconcile WindowsML

Treat WindowsML as a manual three-way reconciliation:

- Keep the `release/experimental` directory structure as the starting point.
- Review each unique fix from `main` by behavior.
- Port fixes whose paths changed instead of cherry-picking blindly.
- Evaluate the WinML EP Catalog project for stable API eligibility.
- Verify initialization, CFG, provider matching, logging, and EP fixes.

### 6. Decide Experimental Candidates

Keep these out of the stable integration until their APIs are confirmed stable:

- `AppContentSearch`
- `WinUI/ConditionalPredicate`
- The CMake samples unique to `release/2.0-experimental`

If retained, they must be clearly isolated and documented as experimental.

### 7. Normalize C++ Toolset Selection

Perform repository-wide toolset normalization only after the final sample set
is present, and before final validation:

- Continue to support Visual Studio 2022 with the `v143` toolset.
- Support Visual Studio 2026 with the `v145` toolset.
- Import `Build.Common.Cpp.props` consistently before
  `Microsoft.Cpp.Default.props`.
- Remove hard-coded `v143` selections that have no documented requirement.
- Do not replace hard-coded `v143` selections with hard-coded `v145`.
- Validate with both Visual Studio 2022 and Visual Studio 2026.

During sample migration, retain each imported project's existing toolset
selection. Installing `v143` alongside Visual Studio 2026 may be used to unblock
intermediate validation, but it does not replace validation with Visual Studio
2022.

### 8. Update Documentation and CI

After the sample set is approved:

- Update the root sample index and links.
- Document supported Windows App SDK versions.
- Mark any approved experimental content explicitly.
- Update CI sample lists.
- Remove obsolete descriptions of branch roles.

### 9. Cut Over to Main

All implementation pull requests target the integration branch. The final pull
request targets `main` and contains the approved integration result.

Use a merge strategy that preserves both histories. Do not squash the entire
reconciliation or force-update `main`. Keep the old release branches until the
new `main` has completed final validation.

## Pull Request Sequence

1. Design document and project matrix
2. Shared build and package infrastructure
3. Low-risk stable samples from `main`
4. Stable `WindowsAIFoundry`
5. Reconciled `WindowsML`
6. C++ toolset normalization
7. Documentation and CI
8. Final integration-to-`main` cutover

Each pull request must have one intent and describe what changed, why it
changed, and how it was validated.

## Risks

- Package normalization can change sample behavior or supported SDK versions.
- A unique commit may already exist under another path or implementation.
- Stable release branches may contain intentional removals mixed with stale
  shared configuration.
- Large WindowsML path changes can make automatic cherry-picks misleading.
- Source branches may advance while reconciliation is in progress.
- The final merge can reintroduce changes if its expected tree is not verified.

Mitigations include pinned source commits, project-level decisions, focused
pull requests, targeted builds, and a final tree comparison.

## Validation Plan

Before modifying build behavior, read and use the repository's existing
`build.ps1` or `build.cmd`; do not construct separate restore or MSBuild logic.

For each migrated sample, run the targeted build:

```powershell
pwsh -File build.ps1 -Sample <SampleName>
```

Validation also includes:

- No new build warnings or errors
- No unapproved prerelease package references
- Existing x64 and ARM64 configurations remain valid
- Smoke tests for activation, UI, and WindowsML scenarios where applicable
- Project matrix confirms that no approved sample was deleted
- Final tree matches every approved project-level disposition
- Full repository build after all targeted builds pass

## Completion Criteria

- Every approved stable sample is present in the integration branch.
- Every unique stable change has been included or has a documented disposition.
- Every excluded project has a documented reason.
- Targeted builds pass for all affected samples.
- The full repository build passes without new warnings or errors.
- Documentation and CI reflect the final sample set.
- Future contributions are directed to `main`.

## Execution Status

- [x] Pin the initial four branch tips.
- [x] Create the local integration branch.
- [x] Establish the initial branch and top-level sample comparison.
- [x] Generate and review the project-level matrix.
- [x] Reconcile shared infrastructure.
- [x] Review low-risk stable sample candidates.
- [ ] Import low-risk stable samples.
- [ ] Stabilize WindowsAIFoundry.
- [ ] Reconcile WindowsML.
- [ ] Decide experimental candidates.
- [ ] Normalize C++ toolset selection.
- [ ] Update documentation and CI.
- [ ] Complete final validation and cut over to `main`.
