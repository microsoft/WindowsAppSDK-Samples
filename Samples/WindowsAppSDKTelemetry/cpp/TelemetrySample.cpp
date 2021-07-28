// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "TelemetrySample.h"

int main()
{
    WindowsAppSDKTelemetrySample::BooleanTelemetryEvent(true);
    WindowsAppSDKTelemetrySample::TextPayloadEvent(L"This text is the event payload");
    WindowsAppSDKTelemetrySample::EventWithUserDefinedType(L"This is the use-defined type payload");
}
