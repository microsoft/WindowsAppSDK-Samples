using var inputImage = await LoadYourImageAsSoftwareBitmap();
using ImageForegroundExtractor extractor = await ImageForegroundExtractor.CreateAsync();
using var foregroundMask = extractor.GetMaskFromSoftwareBitmap(inputImage);
// Apply the mask to extract the foreground
var foregroundImage = inputImage.ApplyMask(foregroundMask);