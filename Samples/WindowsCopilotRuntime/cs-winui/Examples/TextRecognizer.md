SoftwareBitmap inputImage;
TextRecognizerOptions recognizerOptions;
var recognizerResult = model.RecognizeTextFromImage(inputImage, recognizerOptions);
IEnumerable<string> result = recognizerResult.Lines.Select(line => line.Text);
