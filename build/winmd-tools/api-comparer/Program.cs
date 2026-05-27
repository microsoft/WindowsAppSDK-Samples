// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

// API Comparer - compares WinMD files between stable and experimental packages
// to identify experimental-only APIs.
//
// Usage:
//   ApiComparer <stable-dir> <exp-dir> <output-dir> [<stable-version> <exp-version>]

using System.Reflection.Metadata;
using System.Reflection.PortableExecutable;
using System.Text.Json;

if (args.Length < 3)
{
    Console.Error.WriteLine("Usage: ApiComparer <stable-winmd-dir> <exp-winmd-dir> <output-dir> [<stable-version> <exp-version>]");
    Console.Error.WriteLine();
    Console.Error.WriteLine("Compares WinMD files from two directories and outputs exp-only APIs.");
    return 1;
}

var stableDir = args[0];
var expDir = args[1];
var outputDir = args[2];
var stableVersion = args.Length > 3 ? args[3] : "unknown";
var expVersion = args.Length > 4 ? args[4] : "unknown";

Console.WriteLine("=== WinAppSDK API Comparer ===");
Console.WriteLine($"Stable:       {stableDir}");
Console.WriteLine($"Experimental: {expDir}");
Console.WriteLine($"Output:       {outputDir}");
Console.WriteLine();

// Find WinMD files
var stableWinMd = FindWinMdFiles(stableDir);
var expWinMd = FindWinMdFiles(expDir);

Console.WriteLine($"Stable WinMD files: {stableWinMd.Count}");
foreach (var f in stableWinMd) Console.WriteLine($"  - {Path.GetFileName(f)}");
Console.WriteLine($"Experimental WinMD files: {expWinMd.Count}");
foreach (var f in expWinMd) Console.WriteLine($"  - {Path.GetFileName(f)}");
Console.WriteLine();

// Parse APIs
Console.WriteLine("Parsing stable APIs...");
var stableApis = ParseApis(stableWinMd);
Console.WriteLine($"  {stableApis.Types.Count} types, {stableApis.Members.Count} members");

Console.WriteLine("Parsing experimental APIs...");
var expApis = ParseApis(expWinMd);
Console.WriteLine($"  {expApis.Types.Count} types, {expApis.Members.Count} members");
Console.WriteLine();

// Compute difference
Console.WriteLine("Computing difference...");
var expOnlyTypes = expApis.Types.Keys.Where(t => !stableApis.Types.ContainsKey(t)).OrderBy(t => t).ToList();
var expOnlyMembers = expApis.Members.Keys.Where(m => !stableApis.Members.ContainsKey(m)).OrderBy(m => m).ToList();

Console.WriteLine($"  Exp-only types: {expOnlyTypes.Count}");
Console.WriteLine($"  Exp-only members: {expOnlyMembers.Count}");
Console.WriteLine();

// Output results
Directory.CreateDirectory(outputDir);

var result = new
{
    StableVersion = stableVersion,
    ExperimentalVersion = expVersion,
    StableDir = stableDir,
    ExperimentalDir = expDir,
    GeneratedAt = DateTime.Now.ToString("o"),
    ExpOnlyTypes = expOnlyTypes,
    ExpOnlyMembers = expOnlyMembers,
    Summary = new
    {
        StableTypeCount = stableApis.Types.Count,
        ExpTypeCount = expApis.Types.Count,
        ExpOnlyTypeCount = expOnlyTypes.Count,
        ExpOnlyMemberCount = expOnlyMembers.Count
    }
};

var jsonPath = Path.Combine(outputDir, "exp-only-apis.json");
File.WriteAllText(jsonPath, JsonSerializer.Serialize(result, new JsonSerializerOptions { WriteIndented = true }));
Console.WriteLine($"JSON output: {jsonPath}");

// Text output
var textPath = Path.Combine(outputDir, "exp-only-apis.txt");
using (var writer = new StreamWriter(textPath))
{
    writer.WriteLine($"# Experimental-Only APIs");
    writer.WriteLine($"# Stable Version:       {stableVersion}");
    writer.WriteLine($"# Experimental Version: {expVersion}");
    writer.WriteLine($"# Stable Dir:           {stableDir}");
    writer.WriteLine($"# Experimental Dir:     {expDir}");
    writer.WriteLine($"# Generated:            {DateTime.Now:o}");
    writer.WriteLine();
    writer.WriteLine($"## Types ({expOnlyTypes.Count})");
    foreach (var t in expOnlyTypes)
        writer.WriteLine(t);
    writer.WriteLine();
    writer.WriteLine($"## Members ({expOnlyMembers.Count})");
    foreach (var m in expOnlyMembers)
        writer.WriteLine(m);
}
Console.WriteLine($"Text output: {textPath}");

Console.WriteLine();
Console.WriteLine($"=== Done: {expOnlyTypes.Count} exp-only types, {expOnlyMembers.Count} exp-only members ===");
return 0;

// === Helper functions ===

static List<string> FindWinMdFiles(string rootDir)
{
    var files = new List<string>();
    if (Directory.Exists(rootDir))
    {
        files.AddRange(Directory.GetFiles(rootDir, "*.winmd", SearchOption.AllDirectories));
    }

    // Deduplicate by filename (WinMD is arch-neutral)
    var seen = new HashSet<string>(StringComparer.OrdinalIgnoreCase);
    return files.Where(f => seen.Add(Path.GetFileName(f))).ToList();
}

static (Dictionary<string, bool> Types, Dictionary<string, bool> Members) ParseApis(List<string> winmdFiles)
{
    var types = new Dictionary<string, bool>();
    var members = new Dictionary<string, bool>();

    foreach (var file in winmdFiles)
    {
        try
        {
            using var stream = File.OpenRead(file);
            using var peReader = new PEReader(stream);

            if (!peReader.HasMetadata) continue;

            var reader = peReader.GetMetadataReader();

            foreach (var typeHandle in reader.TypeDefinitions)
            {
                var typeDef = reader.GetTypeDefinition(typeHandle);
                var name = reader.GetString(typeDef.Name);
                var ns = reader.GetString(typeDef.Namespace);

                // Skip internal types
                if (string.IsNullOrEmpty(name) || name == "<Module>" || name.StartsWith('<'))
                    continue;

                // Only public types
                var visibility = typeDef.Attributes & System.Reflection.TypeAttributes.VisibilityMask;
                if (visibility != System.Reflection.TypeAttributes.Public &&
                    visibility != System.Reflection.TypeAttributes.NestedPublic)
                    continue;

                var fullName = string.IsNullOrEmpty(ns) ? name : $"{ns}.{name}";
                types[fullName] = true;

                // Get public methods
                foreach (var methodHandle in typeDef.GetMethods())
                {
                    var method = reader.GetMethodDefinition(methodHandle);
                    var methodName = reader.GetString(method.Name);

                    // Skip special methods
                    if (methodName.StartsWith('.') || methodName.StartsWith('<') ||
                        methodName.StartsWith("get_") || methodName.StartsWith("set_") ||
                        methodName.StartsWith("add_") || methodName.StartsWith("remove_"))
                        continue;

                    var methodAccess = method.Attributes & System.Reflection.MethodAttributes.MemberAccessMask;
                    if (methodAccess == System.Reflection.MethodAttributes.Public)
                    {
                        members[$"{fullName}.{methodName}"] = true;
                    }
                }

                // Get properties
                foreach (var propHandle in typeDef.GetProperties())
                {
                    var prop = reader.GetPropertyDefinition(propHandle);
                    var propName = reader.GetString(prop.Name);
                    members[$"{fullName}.{propName}"] = true;
                }

                // Get events
                foreach (var eventHandle in typeDef.GetEvents())
                {
                    var evt = reader.GetEventDefinition(eventHandle);
                    var eventName = reader.GetString(evt.Name);
                    members[$"{fullName}.{eventName}"] = true;
                }
            }
        }
        catch (Exception ex)
        {
            Console.Error.WriteLine($"Warning: Failed to parse {file}: {ex.Message}");
        }
    }

    return (types, members);
}
