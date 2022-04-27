<!--
Thank you for your pull request!

Please see https://github.com/microsoft/WindowsAppSDK-Samples/blob/main/docs/samples-guidelines.md for guidelines on
how to best contribute to the Windows App SDK Samples repository!

-->

## Description

Please include a summary of the change and/or which issue is fixed. Please also include relevant motivation and context. List any dependencies that are required for this change.

## Target Release

Please specify which release this PR should align with. e.g., 1.0, 1.1, 1.1 Preview 1.

## Checklist

- [ ] Samples build and run using the Visual Studio versions listed in the [Windows development docs](https://docs.microsoft.com/windows/apps/windows-app-sdk/set-up-your-development-environment?tabs=stable#2-install-visual-studio).
- [ ] Samples build and run on all supported platforms (x64, x86, ARM64) and configurations (Debug, Release).
- [ ] Samples set the minimum supported OS version to Windows 10 version 1809.
- [ ] Samples build clean with no warnings or errors.
- [ ] **[For new samples]**: Samples have completed the [sample guidelines checklist](https://github.com/microsoft/WindowsAppSDK-Samples/blob/main/docs/samples-guidelines.md#checklist) and follow [standardization/naming guidelines](https://github.com/microsoft/WindowsAppSDK-Samples/blob/main/docs/samples-guidelines.md#standardization-and-naming).
- [ ] If I am onboarding a new feature, then I must have correctly setup a new CI pipeline for my feature with the correct triggers and path filters laid out in the "Onboarding Samples CI Pipeline for new feature" section in samples-guidelines.md.
- [ ] I have commented on my PR `/azp run SamplesCI-<FeatureName>` to have the CI build run on my branch for each of my FeatureName my PR is modifying. This must be done on the latest commit on the PR before merging to ensure the build is up to date and accurate. Warning: the PR will not block automatically if this is not run due to '/azp run' limitation on triggering more than 10 pipelines.
