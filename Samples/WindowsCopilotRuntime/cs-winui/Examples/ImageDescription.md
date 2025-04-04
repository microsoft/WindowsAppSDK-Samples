SoftwareBitmap inputImage;
ImageBuffer imageBuffer = ImageBuffer.CreateCopyFromBitmap(inputImage);

// Image Description Scenario and Content Filter Options can be chosen as per scenario

ContentFilterOptions filterOptions = new();
filterOptions.ImageMaxAllowedSeverityLevel.AdultContentLevel = SeverityLevel.Minimum;
filterOptions.ImageMaxAllowedSeverityLevel.RacyContentLevel = SeverityLevel.Minimum;
filterOptions.ImageMaxAllowedSeverityLevel.GoryContentLevel = SeverityLevel.Minimum;
filterOptions.ImageMaxAllowedSeverityLevel.ViolentContentLevel = SeverityLevel.Minimum;
filterOptions.ResponseMaxAllowedSeverityLevel.Violent = SeverityLevel.Minimum;
filterOptions.ResponseMaxAllowedSeverityLevel.SelfHarm = SeverityLevel.Minimum;
filterOptions.ResponseMaxAllowedSeverityLevel.Sexual = SeverityLevel.Minimum;
filterOptions.ResponseMaxAllowedSeverityLevel.Hate = SeverityLevel.Minimum;

var modelResponse = await Session.DescribeAsync(inputImage, ImageDescriptionKind.BriefDescription, filterOptions);
return modelResponse.Description;
