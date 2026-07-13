# DatesAndTimes

Ported to WinUI 3 / Windows App SDK from the UWP [DateTimeFormatting](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/DateTimeFormatting) and [Calendar](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/Calendar) samples, combined into a single project.

## What it shows

This sample groups two related globalization features under one NavigationView:

**Date and time formatting** (6 scenarios) uses `Windows.Globalization.DateTimeFormatting.DateTimeFormatter` to format dates and times:

- Long and short formats
- Format via string template
- Format via parameterized template
- Override the user's global context (language, calendar, clock, numeral system)
- Unicode extensions in language tags
- Time zones

**Calendar** (5 scenarios) uses `Windows.Globalization.Calendar` to work with calendar systems:

- Calendar data (month, day, day of week, year across calendar systems)
- Calendar statistics (months in year, days in month, hours in period, era)
- Enumeration and math (enumerate Japanese eras and years; enumerate hours across a DST transition)
- Unicode extensions in language tags
- Time zone support (read/change the time zone and observe the effect on date/time properties)

## APIs featured

- `Windows.Globalization.DateTimeFormatting.DateTimeFormatter`
- `Windows.Globalization.Calendar`
- `Windows.Globalization.CalendarIdentifiers`, `Windows.Globalization.ClockIdentifiers`

## Build & run

```powershell
dotnet build -c Debug -p:Platform=x64
dotnet run -c Debug -p:Platform=x64
```

## Migration notes

Both UWP originals are `SDKTemplate` samples. Their scenarios were carried over unchanged apart from the standard WinUI 3 transforms: `Windows.UI.Xaml` namespaces updated to `Microsoft.UI.Xaml`, the page background removed so the template Mica backdrop shows through, and the UWP `ScenarioDescriptionTextStyle` / `BasicTextStyle` replaced with a "Description:" header (`SubtitleTextBlockStyle`) plus `BodyTextBlockStyle` body text.

`Windows.Globalization.DateTimeFormatter` and `Windows.Globalization.Calendar` (including `CalendarIdentifiers` and `ClockIdentifiers`) all work on WinUI 3 desktop, so no scenarios were dropped.

## Known differences / limitations

None. All 11 scenarios port directly.
