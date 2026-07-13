# Contributing

## Adding a migrated sample

1. Copy the original UWP sample name as the folder name under `Samples/`.
2. Follow [docs/MIGRATION_VERIFICATION.md](docs/MIGRATION_VERIFICATION.md) end to end.
3. Use the `winui-uwp-migration` skill (win-dev-skills) to drive the port.
4. Start each sample README from `templates/SAMPLE_README_TEMPLATE.md`.
5. A sample is only mergeable once the manual verification checklist passes and the
   per-sample README links the Learn doc(s) it serves.

## Scope

- In scope: samples with verdict **Migrate** from the analysis.
- Out of scope (for now): **Rewrite**, **Superseded** verdicts. **Conditional** samples
  need a decision before porting.

## Conventions

- One sample per folder; standard WinUI 3 (Windows App SDK) project shape.
- `<RootNamespace>` matches the original UWP namespace where practical.
- Launch with `winapp run`, never the raw `.exe`.
