using System;
using System.Runtime.InteropServices;
using Windows.Devices.Bluetooth.GenericAttributeProfile;
using Windows.Foundation.Metadata;

namespace SDKTemplate
{
    // Define the characteristics and other properties of our custom service.
    static class Constants
    {
        // BT_Code: Initializes custom local parameters w/ properties, protection levels as well as common descriptors like User Description.
        public static GattLocalCharacteristicParameters gattOperand1Parameters { get; } = new GattLocalCharacteristicParameters
        {
            CharacteristicProperties = GattCharacteristicProperties.Write |
                                       GattCharacteristicProperties.WriteWithoutResponse,
            WriteProtectionLevel = GattProtectionLevel.Plain,
            UserDescription = "Operand 1 Characteristic"
        };

        public static GattLocalCharacteristicParameters gattOperand2Parameters { get; } = new GattLocalCharacteristicParameters
        {
            CharacteristicProperties = GattCharacteristicProperties.Write |
                                       GattCharacteristicProperties.WriteWithoutResponse,
            WriteProtectionLevel = GattProtectionLevel.Plain,
            UserDescription = "Operand 2 Characteristic"
        };

        public static GattLocalCharacteristicParameters gattOperatorParameters { get; } = new GattLocalCharacteristicParameters
        {
            CharacteristicProperties = GattCharacteristicProperties.Write |
                                       GattCharacteristicProperties.WriteWithoutResponse,
            WriteProtectionLevel = GattProtectionLevel.Plain,
            UserDescription = "Operator Characteristic"
        };

        public static GattLocalCharacteristicParameters gattResultParameters { get; } = new GattLocalCharacteristicParameters
        {
            CharacteristicProperties = GattCharacteristicProperties.Read |
                                       GattCharacteristicProperties.Notify,
            WriteProtectionLevel = GattProtectionLevel.Plain,
            UserDescription = "Result Characteristic",
            PresentationFormats =
            {
                // 32-bit unsigned integer, with exponent 0, the unit is unitless, with no company description
                GattPresentationFormat.FromParts(
                    GattPresentationFormatTypes.UInt32,
                    PresentationFormats.Exponent,
                    Convert.ToUInt16(PresentationFormats.Units.Unitless),
                    Convert.ToByte(PresentationFormats.NamespaceId.BluetoothSigAssignedNumber),
                    PresentationFormats.Description),
            },
        };

        public static Guid CalculatorServiceUuid { get; } = Guid.Parse("caecface-e1d9-11e6-bf01-fe55135034f0");

        public static Guid Operand1CharacteristicUuid { get; } = Guid.Parse("caec2ebc-e1d9-11e6-bf01-fe55135034f1");
        public static Guid Operand2CharacteristicUuid { get; } = Guid.Parse("caec2ebc-e1d9-11e6-bf01-fe55135034f2");
        public static Guid OperatorCharacteristicUuid { get; } = Guid.Parse("caec2ebc-e1d9-11e6-bf01-fe55135034f3");
        public static Guid ResultCharacteristicUuid { get; } = Guid.Parse("caec2ebc-e1d9-11e6-bf01-fe55135034f4");

        public static Guid BackgroundCalculatorServiceUuid { get; } = Guid.Parse("caecface-e1d9-11e6-bf01-fe55135034f5");

        public static Guid BackgroundOperand1Uuid { get; } = Guid.Parse("caec2ebc-e1d9-11e6-bf01-fe55135034f6");
        public static Guid BackgroundOperand2Uuid { get; } = Guid.Parse("caec2ebc-e1d9-11e6-bf01-fe55135034f7");
        public static Guid BackgroundOperatorUuid { get; } = Guid.Parse("caec2ebc-e1d9-11e6-bf01-fe55135034f8");
        public static Guid BackgroundResultUuid { get; } = Guid.Parse("caec2ebc-e1d9-11e6-bf01-fe55135034f9");

        // Name of data container for sharing Calculator service data between background and foreground.
        public static string CalculatorContainerName => "Calculator";
    }
}


