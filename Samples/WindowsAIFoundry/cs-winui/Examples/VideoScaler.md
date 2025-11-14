var inputImageBuffer = ImageBuffer.CreateForBuffer(
            inputBuffer.AsBuffer(),
            ImageBufferPixelFormat.Bgr8,
            width,
            height,
            width * 3);
var outputImageBuffer = ImageBuffer.CreateForBuffer(
            outputBuffer.AsBuffer(),
            ImageBufferPixelFormat.Bgr8,
            width,
            height,
            width * 3);

var result = model.ScaleFrame(inputImageBuffer, outputImageBuffer, new VideoScalerOptions());
