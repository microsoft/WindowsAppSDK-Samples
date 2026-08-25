# Samples feed automation

`SamplesFeed-Hydration.yml` keeps the public `WinAppSDK-SampleDeps` feed populated without leaving its NuGet Gallery upstream enabled.

## Pipeline setup

Create the pipeline in the public `shine-oss/WinAppSDK-Samples` Azure DevOps project and point it at this repository and YAML file. Configure the pipeline with a maximum of one concurrent run because the feed upstream setting is shared state.

The pipeline derives the branch to hydrate from `Build.SourceBranch`. Updates to `main` hydrate `main`, while updates to a matching `release/*` branch hydrate that release branch. Branches outside protected `main` and `release/*` are rejected. The pipeline clones the triggering branch separately and uses `eng/SamplesFeed.nuget.config`.

The `WinAppSDK-Samples Build Service (shine-oss)` identity needs:

- permission to use the pipeline OAuth token;
- the feed `Administrator` role while toggling upstream settings;
- the feed `Collaborator` capability to save packages from the upstream.

Do not enable pull-request triggers for the hydration job. It receives `System.AccessToken`, so it must only execute scripts from protected branches. Pull requests continue to use the existing sample build pipelines; package-version changes become anonymously available after the protected-branch hydration run completes.
Restrict permission to queue the pipeline to trusted maintainers.

## Flow

1. Clone the protected branch whose update triggered the pipeline.
2. Enable NuGet Gallery as the feed upstream.
3. Restore all sample solutions and explicit CMake NuGet dependencies through the single-source hydration config with the authenticated build identity.
4. Disable the upstream in an `always()` cleanup step.
5. Use a fresh job without `NuGetAuthenticate` to verify that the complete closure restores anonymously.

## Initial rollout

1. Push an automation-only trusted topic branch containing this YAML, the `eng` scripts, and the hydration config. Do not change the branch's existing `Samples/nuget.config` yet.
2. Create the pipeline from `SamplesFeed-Hydration.yml` on that topic branch, then queue it with both the YAML branch and `SamplesBranch` set to the trusted topic branch. This validates dependency corrections included in the automation PR without changing the public branch's NuGet sources.
3. Require both hydration and anonymous validation to pass, merge the automation PR, and retarget the pipeline's default YAML branch to `main`.
4. Only then merge the separate change that removes nuget.org/package source mapping and makes `WinAppSDK-SampleDeps` the branch's single checked-in source.

If hydration fails, first confirm that the cleanup step removed the upstream before retrying.
