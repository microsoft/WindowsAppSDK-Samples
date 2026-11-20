# Samples feed automation

`SamplesFeed-Hydration.yml` keeps the public `WinAppSDK-SampleDeps` feed populated without leaving its NuGet Gallery upstream enabled.

## Pipeline setup

Create the pipeline in the public `shine-oss/WinAppSDK-Samples` Azure DevOps project and point it at this repository and YAML file.

Automatic runs use `Branch` mode and derive the branch to hydrate from `Build.SourceBranch`. Updates to `main` hydrate `main`, while updates to a matching `release/*` branch hydrate that release branch. Branches outside protected `main` and `release/*` are rejected. The pipeline clones the selected branch separately and uses `eng/SamplesFeed.nuget.config`.

The `WinAppSDK-Samples Build Service (shine-oss)` identity needs:

- permission to use the pipeline OAuth token;
- the feed `Administrator` role while toggling upstream settings;
- the feed `Collaborator` capability to save packages from the upstream.

Do not enable pull-request triggers for the hydration job. It receives `System.AccessToken`, so it must only execute scripts from protected branches. Pull requests continue to use the existing sample build pipelines; package-version changes become anonymously available after the protected-branch hydration run completes.
Restrict permission to queue the pipeline to trusted maintainers.

## Modes

| Mode | Behavior |
| --- | --- |
| `Branch` | Hydrates and anonymously validates a branch. |
| `ValidateOnly` | Anonymously restores that branch without changing upstreams. |
| `WindowsAppSDKVersion` | Hydrates and anonymously validates an SDK version. |

The queue-time variable `BranchOrVersion` is interpreted according to the mode:

- `Branch` or `ValidateOnly`: leave it blank to use the triggering branch,
  or enter `main` or a protected `release/*` branch.
- `WindowsAppSDKVersion`: enter an exact NuGet version, such as
  `2.7.0`. This restores `Microsoft.WindowsAppSDK` and its dependency closure.
  The pipeline rejects invalid versions before enabling the upstream.

In the pipeline's **Variables** settings, add `BranchOrVersion` with an empty
value and enable **Let users override this value when running this pipeline**.
Do not define the input variable in YAML, which would shadow the UI setting.
When queuing a run, select the mode and edit this input under **Variables**.
No placeholder values are used. API callers can omit it or pass an empty value
to select the triggering branch in branch modes.

Unlike runtime string parameters, variables allow empty values. Validation
runs at the start of each job, before authentication or upstream enablement,
not in the queue form: a blank version can be queued but fails immediately.
API callers previously supplying `SamplesBranch` or `WindowsAppSDKVersion`
as template parameters must now supply the `BranchOrVersion` variable instead.

Target resolution tests run without accessing feeds:

```powershell
powershell -NoProfile -Command `
    "Invoke-Pester .\eng\Tests\ResolveSamplesFeedTarget.Tests.ps1"
```

## Flow

1. Clone the protected branch whose update triggered the pipeline.
2. Enable NuGet Gallery as the feed upstream.
3. Restore all sample solutions and explicit CMake NuGet dependencies through the single-source hydration config with the authenticated build identity.
4. Disable the upstream in an `always()` cleanup step.
5. Use a fresh job without `NuGetAuthenticate` to verify that the complete closure restores anonymously.

If hydration fails, first confirm that the cleanup step removed the upstream before retrying.

A version appearing on nuget.org does not guarantee that the Azure Artifacts
upstream can resolve it yet. If NuGet reports a published version as not found,
verify availability through the authenticated feed with its upstream enabled.
If the version remains unavailable, investigate Azure Artifacts upstream
availability rather than repeatedly rerunning the pipeline. Do not bypass the
feed or leave its upstream enabled: anonymous validation still requires the
complete package closure to be saved in the feed.

## .NET toolchain coverage

The `DotNetToolchains` matrix keeps the global.json-driven restore of every
sample on its existing VS 2022 image. Additional jobs restore desktop SDK-style
C# projects with SDKs 9.0.321 and 10.0.403 in isolated installation roots.
The anonymous job repeats the same matrix. Authenticated jobs run one at a time
so their upstream enable/disable steps cannot overlap.

Runtime-pack versions are selected by the SDK, not just the target framework.
For example, SDK 10.0.403 selects 8.0.33 when restoring net8.0 samples, while
SDK 8.0.428 selects 8.0.34. Keep older SDK entries when extending coverage so
their exact runtime dependencies remain available.

`-DesktopSdkProjectsOnly` restores the non-MAUI SDK-style C# projects directly.
Native solutions and MAUI stay in the original full-solution job, avoiding
migration of older C++ XAML targets and unsupported net8.0 mobile workloads
just to hydrate implicit .NET packages. Only the SDK 10 job uses the VS 2026
image, which supplies its required MSBuild 18.
