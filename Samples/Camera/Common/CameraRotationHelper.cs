using Windows.Devices.Enumeration;
using Windows.Devices.Sensors;
using Windows.Graphics.Imaging;
using Windows.Media.Capture;
using Windows.Storage.FileProperties;

namespace SDKTemplate
{
    internal sealed class CameraRotationHelper
    {
        private readonly EnclosureLocation? _cameraEnclosureLocation;

        public CameraRotationHelper(EnclosureLocation? cameraEnclosureLocation)
        {
            _cameraEnclosureLocation = cameraEnclosureLocation;
        }

        public static bool IsEnclosureLocationExternal(EnclosureLocation? enclosureLocation)
        {
            return enclosureLocation == null || enclosureLocation.Panel == Windows.Devices.Enumeration.Panel.Unknown;
        }

        public SimpleOrientation GetUIOrientation()
        {
            return SimpleOrientation.NotRotated;
        }

        public SimpleOrientation GetCameraCaptureOrientation()
        {
            return SimpleOrientation.NotRotated;
        }

        public SimpleOrientation GetCameraPreviewOrientation()
        {
            return SimpleOrientation.NotRotated;
        }

        public bool ShouldMirrorPreview()
        {
            return _cameraEnclosureLocation?.Panel == Windows.Devices.Enumeration.Panel.Front;
        }

        public static PhotoOrientation ConvertSimpleOrientationToPhotoOrientation(SimpleOrientation orientation)
        {
            return orientation switch
            {
                SimpleOrientation.Rotated90DegreesCounterclockwise => PhotoOrientation.Rotate90,
                SimpleOrientation.Rotated180DegreesCounterclockwise => PhotoOrientation.Rotate180,
                SimpleOrientation.Rotated270DegreesCounterclockwise => PhotoOrientation.Rotate270,
                _ => PhotoOrientation.Normal,
            };
        }

        public static int ConvertSimpleOrientationToClockwiseDegrees(SimpleOrientation orientation)
        {
            return orientation switch
            {
                SimpleOrientation.Rotated90DegreesCounterclockwise => 270,
                SimpleOrientation.Rotated180DegreesCounterclockwise => 180,
                SimpleOrientation.Rotated270DegreesCounterclockwise => 90,
                _ => 0,
            };
        }

        public static VideoRotation ConvertSimpleOrientationToVideoRotation(SimpleOrientation orientation)
        {
            return orientation switch
            {
                SimpleOrientation.Rotated90DegreesCounterclockwise => VideoRotation.Clockwise270Degrees,
                SimpleOrientation.Rotated180DegreesCounterclockwise => VideoRotation.Clockwise180Degrees,
                SimpleOrientation.Rotated270DegreesCounterclockwise => VideoRotation.Clockwise90Degrees,
                _ => VideoRotation.None,
            };
        }
    }
}
