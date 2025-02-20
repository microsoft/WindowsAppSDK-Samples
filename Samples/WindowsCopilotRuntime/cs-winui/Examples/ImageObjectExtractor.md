SoftwareBitmap inputImage;
ImageObjectExtractorHint hint;
var model = await ImageObjectExtractor.CreateWithSoftwareBitmapAsync(inputImage);
SoftwareBitmap result = model.GetSoftwareBitmapObjectMask(hint);
