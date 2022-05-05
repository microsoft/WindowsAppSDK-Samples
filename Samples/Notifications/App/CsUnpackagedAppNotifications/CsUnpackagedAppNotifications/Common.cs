// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#if false
//#include "pch.h"
//#include "Common.h"
#endif

class Common
{
    public static string ExtractParam(string args, string paramName)
    {
        string tag = paramName;
        tag += "=";

        int scenarioIdStart = args.IndexOf(tag);
        if (scenarioIdStart == -1)
        {
            return null;
        }

        scenarioIdStart += tag.Length;

        int scenarioIdEnd = args.IndexOf("&", scenarioIdStart);
        if (scenarioIdEnd == -1)
        {
            scenarioIdEnd = args.Length;
        }

        return args.Substring(scenarioIdStart, scenarioIdEnd - scenarioIdStart);
    }
}