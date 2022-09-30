# Windows App SDK Samples Version Update Process

This document outlines the steps that should be followed when updating version data for the Windows App SDK Samples in this repo. 

## Checklist

1. Create a branch off of main
2. Run the [update script](../UpdateVersions.cmd) on your local machine 
3. Run the [local test script](../build.cmd)
4. Push changes
5. Create PR
6. Run build pipelines - SamplesCI-All will build every sample
7. Create an issue in the repo of each problem to be addressed
8. Assign the issue to samples owners
9. Revert broken changes
10. Merge
11. Once any issues are resolved, samples owners create their own PRs
12. Separate PRs are reviewed and merged
