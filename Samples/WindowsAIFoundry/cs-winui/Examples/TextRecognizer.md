SoftwareBitmap inputImage;
var recognizerResult = model.RecognizeTextFromImage(inputImage);
IEnumerable<string> result = recognizerResult.Lines.Select(line => line.Text);
