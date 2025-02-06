SoftwareBitmap inputImage;
ImageBuffer imageBuffer = ImageBuffer.CreateCopyFromBitmap(inputImage);

// called in async call
ContentFilterOptions filterOptions = new();
filterOptions.PromptMinSeverityLevelToBlock.ViolentContentSeverity = SeverityLevel.Medium;
filterOptions.ResponseMinSeverityLevelToBlock.ViolentContentSeverity = SeverityLevel.Medium;

// Image Description Scenario and Content Filter Options can be chosen as per scenario
LanguageModelResponse modelResponse = await Session.DescribeAsync(imageBuffer, ImageDescriptionScenario.Caption, filterOptions);
return modelResponse.Response;
