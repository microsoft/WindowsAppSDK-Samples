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
| `Branch` | Hydrates and anonymously validates the complete dependency closure for the triggering branch. For a manual run, `SamplesBranch` may select `main` or a protected `release/*` branch; blank defaults to the queued branch, normally `main`. |
| `ValidateOnly` | Restores a selected branch anonymously without enabling or changing feed upstreams. |
| `WindowsAppSDKVersion` | Requires `WindowsAppSDKVersion`, then hydrates and anonymously validates that `Microsoft.WindowsAppSDK` package version and its NuGet dependency closure. |

## Flow

1. Clone the protected branch whose update triggered the pipeline.
2. Enable NuGet Gallery as the feed upstream.
3. Restore all sample solutions and explicit CMake NuGet dependencies through the single-source hydration config with the authenticated build identity.
4. Disable the upstream in an `always()` cleanup step.
5. Use a fresh job without `NuGetAuthenticate` to verify that the complete closure restores anonymously.

If hydration fails, first confirm that the cleanup step removed the upstream before retrying.
