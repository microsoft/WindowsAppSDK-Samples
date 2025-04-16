SoftwareBitmap inputImage;
ImageBuffer imageBuffer = ImageBuffer.CreateCopyFromBitmap(inputImage);

// Image Description Scenario and Content Filter Options can be chosen as per scenario

ContentFilterOptions filterOptions = new();
var modelResponse = await Session.DescribeAsync(inputImage, ImageDescriptionKind.BriefDescription, filterOptions);
return modelResponse.Description;
