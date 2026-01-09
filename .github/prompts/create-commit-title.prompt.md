---
agent: 'agent'
model: 'GPT-5.1-Codex-Max'
description: 'Generate an 80-character git commit title for the local diff'
---

# Generate Commit Title

## Purpose
Provide a single-line, ready-to-paste git commit title (<= 80 characters) that reflects the most important local changes since `HEAD`.

## Input to collect
- Run exactly one command to view the local diff:
  ```@terminal
  git diff HEAD
  ```

## How to decide the title
1. From the diff, find the dominant area (e.g., `Samples/AppLifecycle/*`, `docs/**`) and the change type (new sample, bug fix, docs update, config tweak).
2. Draft an imperative, plain-ASCII title that:
   - Mentions the primary feature/sample when obvious (e.g., `AppLifecycle:` or `Docs:`)
   - Stays within 80 characters and has no trailing punctuation

## Final output
- Reply with only the commit title on a single line—no extra text.

## PR title convention (when asked)
Use Conventional Commits style:

`<type>(<scope>): <summary>`

**Allowed types**
- feat, fix, docs, refactor, perf, test, build, ci, chore, sample

**Scope rules**
- Use a short, WindowsAppSDK-Samples-focused scope (one word preferred). Common scopes:
  - Core: `docs`, `build`, `ci`, `templates`, `repo`
  - Features: `applifecycle`, `activation`, `instancing`, `restart`, `notifications`, `push`, `app-notifications`, `windowing`, `widgets`, `mica`, `textrendering`, `resourcemanagement`, `deployment`, `installer`, `unpackaged`, `islands`, `composition`, `input`, `customcontrols`, `backgroundtask`, `scenegraph`, `photoeditor`, `windowsai`, `windowsml`
- If unclear, pick the closest feature or sample area; omit only if unavoidable

**Summary rules**
- Imperative, present tense (“add”, “update”, “remove”, “fix”)
- Keep it <= 72 characters when possible; be specific, avoid “misc changes”

**Examples**
- `feat(applifecycle): add activation sample for file type associations`
- `fix(notifications): correct push notification channel registration`
- `docs(windowing): document presenter configuration options`
- `build(ci): add SamplesCI pipeline for new feature`
- `sample(widgets): add weather widget implementation`
