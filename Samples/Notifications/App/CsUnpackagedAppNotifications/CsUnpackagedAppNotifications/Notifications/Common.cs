// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

class Common
{
	const string scenarioTag = "scenarioId";

    public static string ExtractParamFromArgs(string args, string paramName)
    {
        var tag = paramName;
        tag += "=";

        var tagStart = args.IndexOf(tag);
        if (tagStart == -1)
        {
            return null;
        }

        var paramStart = tagStart + tag.Length;

        var paramEnd = args.IndexOf("&", paramStart);
        if (paramEnd == -1)
        {
            paramEnd = args.Length;
        }

        return args.Substring(paramStart, paramEnd - paramStart);
    }

    public static string MakeScenarioIdToken(int id)
    {
        return scenarioTag + "=" + id.ToString();
    }

    public static int ExtractScenarioIdFromArgs(string args)
    {
        var scenarioId = ExtractParamFromArgs(args, scenarioTag);

        if (scenarioId == null)
        {
            return 0;
        }

        return int.Parse(scenarioId);
    }
}
