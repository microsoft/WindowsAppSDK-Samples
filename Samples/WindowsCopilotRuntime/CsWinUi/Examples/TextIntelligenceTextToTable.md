using LanguageModel languageModel = await LanguageModel.CreateAsync();
var textToTableConverter = new TextToTableConverter(languageModel);

var response = await textToTableConverter.ConvertAsync(prompt);

if (rows != null)
{
    foreach (var row in rows)
    {
        Console.WriteLine(string.Join(',', row.GetColumns()));
    }
}
