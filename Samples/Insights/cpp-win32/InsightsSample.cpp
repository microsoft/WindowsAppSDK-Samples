// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "InsightsSample.h"

/*
* Boolean Event
* 
* "data": {
        "value": 1,
        "COMMON_WINDOWSAPPSDK_PARAMS": {
            "SDKVersion": "0.9",
            "SDKExperimentationLevel": "",
            "IsDebugging": false,
            "AppSessionGuid": "00000D80-0001-0002-B425-C8C4E284D701"
        }
    }

* TextPayloadEvent
* 
* "data": {
        "value": "This text is the event payload",
        "COMMON_WINDOWSAPPSDK_PARAMS": {
            "SDKVersion": "0.9",
            "SDKExperimentationLevel": "",
            "IsDebugging": false,
            "AppSessionGuid": "00000D80-0001-0002-B425-C8C4E284D701"
        }

* EventWithUserDefinedType 
* 
*  "data": {
        "value": "This is the user-defined type payload",
        "COMMON_WINDOWSAPPSDK_PARAMS": {
            "SDKVersion": "0.9",
            "SDKExperimentationLevel": "",
            "IsDebugging": false,
            "AppSessionGuid": "00000D80-0001-0002-B425-C8C4E284D701"
        }
    }
*/

int main()
{
    InsightsSample::BooleanTelemetryEvent(true);
    InsightsSample::TextPayloadEvent(L"This text is the event payload");
    InsightsSample::EventWithUserDefinedType(L"This is the user-defined type payload");
}
