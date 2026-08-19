# Samples feed automation

`SamplesFeed-Hydration.yml` keeps the public `WinAppSDK-SampleDeps` feed populated without leaving its NuGet Gallery upstream enabled.

## Pipeline setup

Create the pipeline in the public `shine-oss/WinAppSDK-Samples` Azure DevOps project and point it at this repository and YAML file. Configure the pipeline with a maximum of one concurrent run because the feed upstream setting is shared state.

Create a pipeline variable named `SamplesBranch`, allow users to override it at queue time, and give it a default value of `main`. It selects the branch whose complete dependency closure is hydrated; set it to the matching `release/x.0-stable` or `release/x.0-experimental` branch when hydrating a release branch. The YAML also falls back to `main` when the variable is absent and rejects branches outside protected `main` and `release/*`. The pipeline clones that branch separately and uses `eng/SamplesFeed.nuget.config`, so it can populate the feed before the selected branch switches its checked-in `Samples/nuget.config` to the single-source configuration.

The `WinAppSDK-Samples Build Service (shine-oss)` identity needs:

- permission to use the pipeline OAuth token;
- the feed `Administrator` role while toggling upstream settings;
- the feed `Collaborator` capability to save packages from the upstream.

Do not enable pull-request triggers for the hydration job. It receives `System.AccessToken`, so it must only execute scripts from protected branches. Pull requests continue to use the existing sample build pipelines; package-version changes become anonymously available after the protected-branch hydration run completes.
Restrict permission to queue the pipeline and override `SamplesBranch` to trusted maintainers.

## Flow

1. Clone the branch selected by `SamplesBranch`.
2. Enable NuGet Gallery as the feed upstream.
3. Restore all sample solutions and explicit CMake NuGet dependencies through the single-source hydration config with the authenticated build identity.
4. Disable the upstream in an `always()` cleanup step.
5. Use a fresh job without `NuGetAuthenticate` to verify that the complete closure restores anonymously.

## Initial rollout

1. Merge the automation files and create the pipeline without changing the branch's existing `Samples/nuget.config`.
2. Queue the pipeline with `SamplesBranch` set to the branch being migrated and require both hydration and anonymous validation to pass.
3. Only then merge the change that removes nuget.org/package source mapping and makes `WinAppSDK-SampleDeps` the branch's single checked-in source.

If hydration fails, first confirm that the cleanup step removed the upstream before retrying.
