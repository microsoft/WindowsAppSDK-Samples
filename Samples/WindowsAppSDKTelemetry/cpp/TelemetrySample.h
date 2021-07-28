// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once

#include "WindowsAppSDKTelemetry.h"

class WindowsAppSDKTelemetrySample : public wil::TraceLoggingProvider
{
    IMPLEMENT_TRACELOGGING_CLASS(WindowsAppSDKTelemetrySample, "Microsoft.WindowsAppSDK.Telemetry.Sample",
        // {1d0d1998-1ce1-545b-bd2d-dc5bda011d82}
        (0x1d0d1998, 0x1ce1, 0x545b, 0xbd, 0x2d, 0xdc, 0x5b, 0xda, 0x01, 0x1d, 0x82));

    // Event that contains a single bool value as payload
    DEFINE_COMPLIANT_TELEMETRY_EVENT_BOOL(BooleanTelemetryEvent, // Event name
        PDT_ProductAndServiceUsage, // Privacy data data for events. Check wil/traceloggingconfig.h for a full list
        value/* value to be logged*/);


    // Event that contains a string as payload
    DEFINE_COMPLIANT_TELEMETRY_EVENT_STRING(TextPayloadEvent, // Event name
        PDT_ProductAndServiceUsage, // Privacy data data for events. Check wil/traceloggingconfig.h for a full list
        value/* value to be logged*/);

    // Event that contains a single user define data type payload
    DEFINE_COMPLIANT_TELEMETRY_EVENT_PARAM1(EventWithUserDefinedType, //Event name
        PDT_ProductAndServiceUsage, // Privacy data data for events. Check wil/traceloggingconfig.h for a full list
        PCWSTR, // Payload data type
        value);
};
