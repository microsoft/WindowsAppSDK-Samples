SoftwareBitmap inputImage;
ImageBuffer imageBuffer = ImageBuffer.CreateCopyFromBitmap(inputImage);

// Image Description Scenario and Content Filter Options can be chosen as per scenario
ContentFilterOptions filterOptions = new();
var modelResponse = await Session.DescribeAsync(inputImage, ImageDescriptionKind.BriefDescription, filterOptions);
if (modelResponse.Status != ImageDescriptionResultStatus.Complete)
{
    return $"Image description failed with status: {modelResponse.Status}";
}
return modelResponse.Description;
