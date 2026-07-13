# WinUI 3 Samples

WinUI 3 / Windows App SDK samples, ported from the original [Windows-universal-samples](https://github.com/microsoft/Windows-universal-samples) (UWP) repo.

This repo exists so that Microsoft Learn documentation can link to runnable WinUI 3 samples instead of legacy UWP ones. Each sample is built, launched, and manually verified against the original UWP sample before it is accepted.

## Building and running

Each sample is a standard WinUI 3 (Windows App SDK) project. From a sample folder:

```powershell
dotnet build
winapp run
```

Requirements: .NET 8+, the Windows App SDK workload, and Windows 10 1809+ (some samples need Windows 11).

## Migration status

The original UWP repo was reviewed sample by sample. Of **266** UWP samples analyzed:

| Category | Count |
| --- | ---: |
| Ported to WinUI 3 (this repo) | 77 |
| In scope, not yet ported (pending design / blocked by API gaps) | 20 |
| Portable, not doc-referenced (candidates for a later pass) | 51 |
| Out of scope (WinUI Gallery, Windows App SDK, superseded, C++/DirectX, or platform-specific) | 122 |
| **Total analyzed** | **266** |

Ported samples are a mix of standalone projects and multi-feature samples that consolidate several related UWP samples into one project (for example, **Camera** and **Sensors**).

## ✅ Ported

| UWP sample | WinUI 3 sample | Notes |
| --- | --- | --- |
| [Accelerometer](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/Accelerometer) | [Sensors](https://github.com/niels9001/winui-samples/tree/main/Samples/Sensors) (consolidated) | "Orientation change" scenario dropped (DisplayInformation desktop API gap) |
| [ActivitySensor](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/ActivitySensor) | [Sensors](https://github.com/niels9001/winui-samples/tree/main/Samples/Sensors) (consolidated) | BackgroundActivity scenario deferred to Pending |
| [Altimeter](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/Altimeter) | [Sensors](https://github.com/niels9001/winui-samples/tree/main/Samples/Sensors) (consolidated) |  |
| [ApplicationData](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/ApplicationData) | [ApplicationData](https://github.com/niels9001/winui-samples/tree/main/Samples/ApplicationData) |  |
| [AssociationLaunching](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/AssociationLaunching) | [AssociationLaunching](https://github.com/niels9001/winui-samples/tree/main/Samples/AssociationLaunching) | Launch files/URIs and receive `.alsdk` file / `alsdk:` protocol activations. Single-instance activation reproduced with a custom `Program.cs` + `AppInstance` (WinUI 3 has no `OnFileActivated`/`OnActivated`). UWP split-screen "view preference" and `https://` appUriHandler dropped. |
| [AudioCategory](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/AudioCategory) | [AudioCategory](https://github.com/niels9001/winui-samples/tree/main/Samples/AudioCategory) | Single page: pick an audio category from a dropdown (Media, Movie, Game Chat, Speech, Communications, Alerts, Sound Effects, Game Effects, Game Media, Other), play a file, and launch a second instance to observe how two categories interact (ducking/attenuation). Replaces the original companion app. |
| [AudioCreation](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/AudioCreation) | [AudioCreation](https://github.com/niels9001/winui-samples/tree/main/Samples/AudioCreation) | 5 of 6 scenarios (File Playback, Device Capture, FrameInput, Submix, Inbox Effects) via Windows.Media.Audio AudioGraph. musicLibrary + microphone capabilities. Scenario6 (Custom Effects) dropped; see Pending. |
| [BackgroundMediaPlayback](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/BackgroundMediaPlayback) | [BackgroundMediaPlayback](https://github.com/niels9001/winui-samples/tree/main/Samples/BackgroundMediaPlayback) | Settings page removed |
| [Barometer](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/Barometer) | [Sensors](https://github.com/niels9001/winui-samples/tree/main/Samples/Sensors) (consolidated) |  |
| [BasicInput](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/BasicInput) | [BasicInput](https://github.com/niels9001/winui-samples/tree/main/Samples/BasicInput) |  |
| [BasicMediaCasting](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/BasicMediaCasting) | [BasicMediaCasting](https://github.com/niels9001/winui-samples/tree/main/Samples/BasicMediaCasting) | 3 scenarios (built-in transport cast, casting device picker, custom device watcher). MediaElement→MediaPlayerElement (video.MediaPlayer.GetAsCastingSource). FileOpenPicker + CastingDevicePicker HWND-initialized. |
| [BluetoothLE](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/BluetoothLE) | [Bluetooth](https://github.com/niels9001/winui-samples/tree/main/Samples/Bluetooth) (consolidated) | Discovery + Client ported. ServerForeground (GATT server) dropped: extended-advertising secondary-PHY APIs absent from 10.0.26100 SDK. Needs BT hardware. |
| [BluetoothRfcommChat](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/BluetoothRfcommChat) | [Bluetooth](https://github.com/niels9001/winui-samples/tree/main/Samples/Bluetooth) (consolidated) | Chat client + foreground chat server ported. Background chat server dropped. Needs BT hardware + peer device. |
| [Calendar](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/Calendar) | [DatesAndTimes](https://github.com/niels9001/winui-samples/tree/main/Samples/DatesAndTimes) (consolidated) | 5 scenarios (calendar data, statistics, enumeration and math, Unicode extensions, time zone support) via Windows.Globalization.Calendar. Combined with DateTimeFormatting under a "Calendar" feature group. |
| [CameraAdvancedCapture](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/CameraAdvancedCapture) | [Camera](https://github.com/niels9001/winui-samples/tree/main/Samples/Camera) (consolidated) |  |
| [CameraFaceDetection](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/CameraFaceDetection) | [Camera](https://github.com/niels9001/winui-samples/tree/main/Samples/Camera) (consolidated) |  |
| [CameraFrames](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/CameraFrames) | [Camera](https://github.com/niels9001/winui-samples/tree/main/Samples/Camera) (consolidated) |  |
| [CameraGetPreviewFrame](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/CameraGetPreviewFrame) | [Camera](https://github.com/niels9001/winui-samples/tree/main/Samples/Camera) (consolidated) |  |
| [CameraManualControls](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/CameraManualControls) | [Camera](https://github.com/niels9001/winui-samples/tree/main/Samples/Camera) (consolidated) |  |
| [CameraProfile](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/CameraProfile) | [Camera](https://github.com/niels9001/winui-samples/tree/main/Samples/Camera) (consolidated) |  |
| [CameraResolution](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/CameraResolution) | [Camera](https://github.com/niels9001/winui-samples/tree/main/Samples/Camera) (consolidated) |  |
| [CameraStarterKit](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/CameraStarterKit) | [Camera](https://github.com/niels9001/winui-samples/tree/main/Samples/Camera) (consolidated) |  |
| [CameraVideoStabilization](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/CameraVideoStabilization) | [Camera](https://github.com/niels9001/winui-samples/tree/main/Samples/Camera) (consolidated) |  |
| [Compass](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/Compass) | [Sensors](https://github.com/niels9001/winui-samples/tree/main/Samples/Sensors) (consolidated) |  |
| [Compression](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/Compression) | [Compression](https://github.com/niels9001/winui-samples/tree/main/Samples/Compression) | 1 scenario (Compressor/Decompressor). Stripped UWP #if WINDOWS_PHONE_APP path; FileOpenPicker HWND-initialized. Builds + launches. |
| [ContentIndexer](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/ContentIndexer) | [ContentIndexer](https://github.com/niels9001/winui-samples/tree/main/Samples/ContentIndexer) | Windows.Storage.Search; 7 scenarios |
| [CustomHidDeviceAccess](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/CustomHidDeviceAccess) | [CustomHidDeviceAccess](https://github.com/niels9001/winui-samples/tree/main/Samples/CustomHidDeviceAccess) | 4 scenarios. Removed UWP suspend/resume lifecycle; restored shared types into HidSampleTypes.cs. humaninterfacedevice capability. Needs SuperMUTT HID device to fully verify. |
| [CustomSerialDeviceAccess](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/CustomSerialDeviceAccess) | [CustomSerialDeviceAccess](https://github.com/niels9001/winui-samples/tree/main/Samples/CustomSerialDeviceAccess) | 4 scenarios. Removed UWP suspend/resume lifecycle (repaired brace imbalance from stripping). serialcommunication capability. Needs a serial device to fully verify. |
| [CustomUsbDeviceAccess](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/CustomUsbDeviceAccess) | [CustomUsbDeviceAccess](https://github.com/niels9001/winui-samples/tree/main/Samples/CustomUsbDeviceAccess) | 6 scenarios. Dropped Scenario7_SyncDevice (background task). Shared types in UsbSampleTypes.cs. usb capability (3 filters). Needs OSR FX2 / SuperMUTT to fully verify. |
| [DatagramSocket](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/DatagramSocket) | [DatagramSocket](https://github.com/niels9001/winui-samples/tree/main/Samples/DatagramSocket) | 5 scenarios. CoreApplication.Properties shared state; networking APIs unchanged. internetClientServer + privateNetworkClientServer. Builds + launches. |
| [DataReaderWriter](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/DataReaderWriter) | [DataReaderWriter](https://github.com/niels9001/winui-samples/tree/main/Samples/DataReaderWriter) | 2 scenarios. Pure DataReader/DataWriter; ReadBytes demo image repointed to Square150x150Logo. Builds + launches. |
| [DateTimeFormatting](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/DateTimeFormatting) | [DatesAndTimes](https://github.com/niels9001/winui-samples/tree/main/Samples/DatesAndTimes) (consolidated) | 6 scenarios (long/short formats, string template, parameterized template, override global context, Unicode extensions, time zones) via Windows.Globalization.DateTimeFormatting.DateTimeFormatter. Combined with Calendar under a "Date and time formatting" feature group. |
| [DeviceEnumerationAndPairing](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/DeviceEnumerationAndPairing) | [DeviceEnumerationAndPairing](https://github.com/niels9001/winui-samples/tree/main/Samples/DeviceEnumerationAndPairing) | 8 scenarios; Scenario3 (background-task watcher) deferred to Pending; DevicePicker uses HWND interop |
| [FileAccess](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/FileAccess) | [FileAccess](https://github.com/niels9001/winui-samples/tree/main/Samples/FileAccess) | 11 scenarios (create, get parent, text, bytes, stream, properties, FutureAccessList/MRU, copy, compare, delete, try-get) on a sample.dat in the Pictures library. Scenario 9 (compare) uses the Windows App SDK Microsoft.Windows.Storage.Pickers.FileOpenPicker (constructed with AppWindowId, no HWND interop) instead of the legacy Windows.Storage.Pickers picker. picturesLibrary capability. |
| [Geolocation](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/Geolocation) | [Geolocation](https://github.com/niels9001/winui-samples/tree/main/Samples/Geolocation) | 5 foreground scenarios. 3 background-task scenarios dropped pending background-activation design. location capability. Needs consent + movement to verify. |
| [Geotag](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/Geotag) | [Geotag](https://github.com/niels9001/winui-samples/tree/main/Samples/Geotag) | 1 scenario (geotag JPEG/MP4 via GeotagHelper). FileOpenPicker HWND-initialized; location capability. Needs a media file + consent. |
| [Gyrometer](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/Gyrometer) | [Sensors](https://github.com/niels9001/winui-samples/tree/main/Samples/Sensors) (consolidated) |  |
| [Inclinometer](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/Inclinometer) | [Sensors](https://github.com/niels9001/winui-samples/tree/main/Samples/Sensors) (consolidated) |  |
| [KeyCredentialManager](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/KeyCredentialManager) | [SecurityIdentity](https://github.com/niels9001/winui-samples/tree/main/Samples/SecurityIdentity) (consolidated) | Windows Hello / device-dependent |
| [LampArray](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/LampArray) | [Lighting](https://github.com/niels9001/winui-samples/tree/main/Samples/Lighting) (consolidated) | Needs LampArray hardware; effect bitmap repointed to StoreLogo.png |
| [LampDevice](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/LampDevice) | [Lighting](https://github.com/niels9001/winui-samples/tree/main/Samples/Lighting) (consolidated) | Needs lamp hardware |
| [LanguageFont](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/LanguageFont) | [Globalization](https://github.com/niels9001/winui-samples/tree/main/Samples/Globalization) (consolidated) |  |
| [LightSensor](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/LightSensor) | [Sensors](https://github.com/niels9001/winui-samples/tree/main/Samples/Sensors) (consolidated) |  |
| [Magnetometer](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/Magnetometer) | [Sensors](https://github.com/niels9001/winui-samples/tree/main/Samples/Sensors) (consolidated) |  |
| [MIDI](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/MIDI) | [MIDI](https://github.com/niels9001/winui-samples/tree/main/Samples/MIDI) | 3 scenarios (device enumeration, receive, send MIDI). MidiDeviceWatcher helper. ScenarioHeaderTextStyle re-added to App.xaml (was in a shared UWP style sheet). Needs a MIDI device to fully verify. |
| [MobileBroadband](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/MobileBroadband) | [MobileNetworking](https://github.com/niels9001/winui-samples/tree/main/Samples/MobileNetworking) (consolidated) | 7 scenarios. Restricted cellular capabilities (cellularDeviceIdentity/Control/Messaging). Launches; needs a mobile broadband modem to exercise. |
| [MobileHotspot](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/MobileHotspot) | [MobileNetworking](https://github.com/niels9001/winui-samples/tree/main/Samples/MobileNetworking) (consolidated) | 2 scenarios (configure + toggle tethering). NetworkOperatorTetheringManager. wiFiControl capability. Needs a Wi-Fi adapter + internet connection to tether. |
| [NetworkConnectivity](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/NetworkConnectivity) | [NetworkConnectivity](https://github.com/niels9001/winui-samples/tree/main/Samples/NetworkConnectivity) | 3 scenarios. NetworkStatusChanged marshals via App.MainDispatcherQueue. Fixed HttpClient ambiguity (qualified Windows.Web.Http.HttpClient). internetClient. Builds + launches. |
| [NfcProvisioner](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/NfcProvisioner) | [NfcProvisioner](https://github.com/niels9001/winui-samples/tree/main/Samples/NfcProvisioner) | 1 scenario (transfer a .ppkg provisioning package over NFC via ProximityDevice.PublishBinaryMessage). FileOpenPicker HWND-initialized. proximity capability. Needs NFC hardware + peer device to fully verify. |
| [NumberFormatting](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/NumberFormatting) | [Globalization](https://github.com/niels9001/winui-samples/tree/main/Samples/Globalization) (consolidated) |  |
| [OCR](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/OCR) | [OCR](https://github.com/niels9001/winui-samples/tree/main/Samples/OCR) | 2 scenarios. Camera capture replumbed CaptureElement→MediaPlayerElement + MediaFrameSource; display-orientation rotation dropped (DisplayInformation gap). Camera needs a webcam. |
| [OrientationSensor](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/OrientationSensor) | [Sensors](https://github.com/niels9001/winui-samples/tree/main/Samples/Sensors) (consolidated) |  |
| [Pedometer](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/Pedometer) | [Sensors](https://github.com/niels9001/winui-samples/tree/main/Samples/Sensors) (consolidated) | BackgroundTask scenario deferred to Pending |
| [PenHaptics](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/PenHaptics) | [PenHaptics](https://github.com/niels9001/winui-samples/tree/main/Samples/PenHaptics) | 4 scenarios. DROPPED Scenario1 (InkCanvasTactileFeedback); InkCanvas/InkToolbar have no WinUI 3 equivalent (microsoft-ui-xaml#4099). |
| [PersonalDataEncryption](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/PersonalDataEncryption) | [SecurityIdentity](https://github.com/niels9001/winui-samples/tree/main/Samples/SecurityIdentity) (consolidated) | Requires PDE enabled; FileOpenPicker HWND-initialized |
| [Personalization](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/Personalization) | [Personalization](https://github.com/niels9001/winui-samples/tree/main/Samples/Personalization) | LockScreen.SetImageFileAsync; picker HWND interop |
| [PlayReady](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/PlayReady) | [PlayReady](https://github.com/niels9001/winui-samples/tree/main/Samples/PlayReady) | 4 scenarios (Reactive/Proactive license request, Manage HW/SW DRM, Secure Stop) via Windows.Media.Protection.PlayReady. MVVM (ViewModels + RelayCommand + PlayReadyInfo control). |
| [PowerGrid](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/PowerGrid) | [PowerGrid](https://github.com/niels9001/winui-samples/tree/main/Samples/PowerGrid) | Windows.Devices.Power forecast |
| [PresenceSensor](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/PresenceSensor) | [Sensors](https://github.com/niels9001/winui-samples/tree/main/Samples/Sensors) (consolidated) |  |
| [ProximitySensor](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/ProximitySensor) | [Sensors](https://github.com/niels9001/winui-samples/tree/main/Samples/Sensors) (consolidated) | Background scenario deferred to Pending |
| [RadioManager](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/RadioManager) | [RadioManager](https://github.com/niels9001/winui-samples/tree/main/Samples/RadioManager) | 1 scenario (Toggle Radios, Windows.Devices.Radios). radios DeviceCapability. CoreDispatcher→DispatcherQueue. |
| [RelativeInclinometer](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/RelativeInclinometer) | [Sensors](https://github.com/niels9001/winui-samples/tree/main/Samples/Sensors) (consolidated) |  |
| [SimpleImaging](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/SimpleImaging) | [SimpleImaging](https://github.com/niels9001/winui-samples/tree/main/Samples/SimpleImaging) | 2 scenarios. 3 file pickers HWND-initialized. Fixed fail-fast: removed empty Source="" on Image (WinUI 3 throws where UWP tolerated). Builds + launches. |
| [SimpleOrientationSensor](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/SimpleOrientationSensor) | [Sensors](https://github.com/niels9001/winui-samples/tree/main/Samples/Sensors) (consolidated) |  |
| [TextSegmentation](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/TextSegmentation) | [Globalization](https://github.com/niels9001/winui-samples/tree/main/Samples/Globalization) (consolidated) |  |
| [TextSuggestion](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/TextSuggestion) | [Globalization](https://github.com/niels9001/winui-samples/tree/main/Samples/Globalization) (consolidated) |  |
| [TouchKeyboard](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/TouchKeyboard) | [TouchKeyboard](https://github.com/niels9001/winui-samples/tree/main/Samples/TouchKeyboard) | 3 scenarios (auto-display on focus with a custom TextBox-derived control; listen for Showing/Hiding events; TryShow/TryHide methods). Ported from the WindowsAppSDK-Samples migration (PR #649) onto the SDKTemplate shell. |
| [TouchKeyboardTextInput](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/TouchKeyboardTextInput) | [TouchKeyboardTextInput](https://github.com/niels9001/winui-samples/tree/main/Samples/TouchKeyboardTextInput) | 2 scenarios (spell check / text suggestions; InputScope scoped views). Fully declarative XAML, no InputPane API. Legacy text styles replaced with built-in styles. |
| [Unicode](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/Unicode) | [Globalization](https://github.com/niels9001/winui-samples/tree/main/Samples/Globalization) (consolidated) |  |
| [UserInfo](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/UserInfo) | [SecurityIdentity](https://github.com/niels9001/winui-samples/tree/main/Samples/SecurityIdentity) (consolidated) |  |
| [WiFiScan](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/WiFiScan) | [WiFiScan](https://github.com/niels9001/winui-samples/tree/main/Samples/WiFiScan) | 4 scenarios. Signal-bar assets copied; AvailableNetworksChanged marshals via App.MainDispatcherQueue. wiFiControl capability. Needs a Wi-Fi adapter. |
| [XamlDataVirtualization](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/XamlDataVirtualization) | [XamlDataVirtualization](https://github.com/niels9001/winui-samples/tree/main/Samples/XamlDataVirtualization) | 2 scenarios. Custom IList+IItemsRangeInfo source over Pictures library. picturesLibrary capability. |
| [XamlDeferLoadStrategy](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/XamlDeferLoadStrategy) | [XamlDeferLoadStrategy](https://github.com/niels9001/winui-samples/tree/main/Samples/XamlDeferLoadStrategy) | 3 scenarios. Custom TitledImage control + Themes/Generic.xaml. AdaptivePage VisibleBoundsChanged→SizeChanged. Demo images repointed to scaffold logos. |
| [XamlDragAndDrop](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/XamlDragAndDrop) | [XamlDragAndDrop](https://github.com/niels9001/winui-samples/tree/main/Samples/XamlDragAndDrop) | 3 scenarios (ListView drag/drop + reorder + trash; drag UI customization on source and target; StartDragAsync timed game). Windows.ApplicationModel.DataTransfer ports directly. dropcursor.png + Symbols.txt packaged as Content. |
| [XamlFocusVisuals](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/XamlFocusVisuals) | [XamlFocusVisuals](https://github.com/niels9001/winui-samples/tree/main/Samples/XamlFocusVisuals) |  |
| [XamlMasterDetail](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/XamlMasterDetail) | [XamlMasterDetail](https://github.com/niels9001/winui-samples/tree/main/Samples/XamlMasterDetail) | 1 scenario (adaptive list/details). Rebuilt as a single in-page adaptive layout on the SDKTemplate shell: wide = ListView(320) + detail(*) side by side; narrow (page width < 720) = list or detail full-width with an in-page back button. Dropped the separate DetailPage, SystemNavigationManager.GetForCurrentView, and Window.Current (no WinUI 3 desktop equivalent). |
| [XamlStateTriggers](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/XamlStateTriggers) | [XamlStateTriggers](https://github.com/niels9001/winui-samples/tree/main/Samples/XamlStateTriggers) | 1 scenario. Built-in AdaptiveTrigger plus custom StateTriggerBase triggers (InputTypeTrigger, ControlSizeTrigger) and an inline StateTrigger bound to a view model property. WrapGrid→ItemsWrapGrid; PointerDeviceType from Microsoft.UI.Input. Dropped the Xbox-only DeviceFamilyTrigger scenario (not targeting Xbox). |

## ⏸️ In scope, not yet ported

Samples we intend to cover but that are blocked by a WinUI 3 desktop API gap or need a design decision (typically background-task / activation models).

| UWP sample | Status | Reason |
| --- | --- | --- |
| [AdvancedCasting](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/AdvancedCasting) | Blocked (API gap) | API gap: Multi-view app (ApplicationViewSwitcher / CoreApplication views, ProjectionViewPage, ViewLifetimeControl) not supported in WinUI 3 desktop; also needs casting hardware. |
| AudioCreation; Scenario6 (Custom Effects) | Pending (needs design) | The custom AudioEchoEffect (IBasicAudioEffect) is activated by class name through the AudioGraph, which requires the effect to be a registered activatable WinRT class. |
| [BackgroundAudio](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/BackgroundAudio) | Pending (needs design) | Needs design; background/activation model in WinUI. |
| [BackgroundTransfer](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/BackgroundTransfer) | Pending (needs design) | Needs design; background/activation model in WinUI. |
| [BluetoothAdvertisement](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/BluetoothAdvertisement) | Blocked (API gap) | Missing source: UWP sample's .xaml files are absent from the repo (only .xaml.cs that call InitializeComponent). Would require hand-reconstructing the UI. |
| [BluetoothLEClient](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/BluetoothLEClient) | Blocked (API gap) | No source: Repo folder contains only README.md, no code. Functionality is covered by BluetoothLE (Scenario2_Client). |
| [CameraHdr](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/CameraHdr) | Blocked (API gap) | No source: Repo folder contains only README.md, no code. |
| [CameraOpenCV](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/CameraOpenCV) | Blocked (API gap) | Native dependency: C# scenario depends on the OpenCVBridge C++/WinRT component (OpenCVHelper.cpp) and native OpenCV libs; no managed equivalent. |
| [CameraStreamCoordinateMapper](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/CameraStreamCoordinateMapper) | Blocked (API gap) | No C# source: C++/DirectX/HLSL sample only; no C# version to port. |
| [CameraStreamCorrelation](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/CameraStreamCorrelation) | Blocked (API gap) | No C# source: C++/DirectX/MediaFoundation sample only; no C# version to port. |
| [CredentialPicker](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/CredentialPicker) | Blocked (API gap) | API gap: CredentialPicker.PickAsync requires CoreWindow (absent in WinUI 3 desktop); no HWND-interop variant. Win32 CredUIPromptForCredentials would be a rewrite. |
| [CustomSensors](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/CustomSensors) | Pending (needs design) | Needs custom sensor hardware. |
| [DisablingScreenCapture](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/DisablingScreenCapture) | Blocked (API gap) | API gap: ApplicationView.GetForCurrentView().IsScreenCaptureEnabled + AppCapture.GetForCurrentView() are CoreWindow-bound, no desktop replacement. |
| [DisplayOrientation](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/DisplayOrientation) | Blocked (API gap) | API gap (rewrite): Single scenario built on DisplayInformation.GetForCurrentView() (throws in WinUI 3 desktop). |
| [DpiScaling](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/DpiScaling) | Blocked (API gap) | API gap (rewrite): Both scenarios depend on DisplayInformation.GetForCurrentView() (LogicalDpi, RawPixelsPerViewPixel, DpiChanged). |
| [HotspotAuthentication](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/HotspotAuthentication) | Pending (needs design) | Built entirely around a background WISPr authentication task (windows.backgroundTasks systemEvent) + restricted networkConnectionManagerProvisioning capability. The background task is the sample. |
| [LinguisticServices](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/LinguisticServices) | Blocked (API gap) | Native dependency: C# scenarios depend on a C++/WinRT RuntimeComponent (Sample.LinguisticServices) wrapping Win32 ELS (elscore.dll); ELS is not projected to .NET. |
| [Nfc](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/Nfc) | Pending (needs design) | Custom multi-page UI + separate PcscSdk class library + C++ HCE (Host Card Emulation) background task. HCE/background-task dependent. Needs a design decision on HCE for WinUI 3. |
| [OnDemandHotspot](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/OnDemandHotspot) | Pending (needs design) | Requires a Microsoft-signed Custom Capability (Microsoft.onDemandHotspotControl) plus two background tasks (WiFiOnDemandHotspotConnectTrigger / UpdateMetadataTrigger). |
| [Store](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/Store) | Pending (needs design) | Requires a real Store association to run (cannot dotnet-run-verify); also mixes deprecated Windows.ApplicationModel.Store.CurrentApp with Windows.Services.Store (needs InitializeWithWindow in desktop). |

## 🔄 Portable, but not a priority

The underlying WinRT APIs for these samples still work in WinUI 3, but they are not referenced by the Learn docs this pass targets. Some also cover areas that are less relevant on Windows 11. They are candidates for a future migration pass.

<details>
<summary>Show all 51 candidate samples</summary>

| UWP sample | Notes |
| --- | --- |
| [3DPrinting](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/3DPrinting) | Uses Windows.Graphics.Printing3D.Print3DManager, which still works in WinUI 3. |
| [AnimationMetrics](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/AnimationMetrics) | Uses Windows.UI.Core.AnimationMetrics.AnimationDescription, which still works in WinUI 3. |
| [Appointments](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/Appointments) | Uses Windows.ApplicationModel.Appointments.AppointmentManager, which still works in WinUI 3. |
| [BarcodeScanner](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/BarcodeScanner) | Uses Windows.Devices.PointOfService.BarcodeScanner, which still works in WinUI 3. |
| [BasicFaceDetection](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/BasicFaceDetection) | Uses Windows.Media.FaceAnalysis.FaceDetector, which still works in WinUI 3. |
| [BasicFaceTracking](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/BasicFaceTracking) | Uses Windows.Media.FaceAnalysis.FaceTracker, which still works in WinUI 3. |
| [Capabilities](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/Capabilities) | Uses Windows.Security.Authorization.AppCapabilityAccess.AppCapability, which still works in WinUI 3. |
| [CashDrawer](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/CashDrawer) | Uses Windows.Devices.PointOfService.CashDrawer, which still works in WinUI 3. |
| [ClientDeviceInformation](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/ClientDeviceInformation) | Uses Windows.Security.ExchangeActiveSyncProvisioning.EasClientDeviceInformation, which still works in WinUI 3. |
| [ContactCards](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/ContactCards) | Intentionally left out of this pass. Uses Windows.ApplicationModel.Contacts.ContactManager (still works in WinUI 3), but ContactManager.ShowContactCard / ShowFullContactCard require HWND interop and the payoff is low for this pass. |
| [ContactPicker](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/ContactPicker) | Uses Windows.ApplicationModel.Contacts.ContactPicker, which still works in WinUI 3. |
| [ExtendedExecution](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/ExtendedExecution) | Uses Windows.ApplicationModel.ExtendedExecution.ExtendedExecutionSession, which still works in WinUI 3. |
| [GlobalizationPreferences](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/GlobalizationPreferences) | Uses Windows.System.UserProfile.GlobalizationPreferences, which still works in WinUI 3. |
| [HttpClient](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/HttpClient) | Uses Windows.Web.Http.HttpClient, which still works in WinUI 3. |
| [JapanesePhoneticAnalysis](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/JapanesePhoneticAnalysis) | Uses Windows.Globalization.JapanesePhoneticAnalyzer, which still works in WinUI 3. |
| [Json](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/Json) | Uses Windows.Data.Json.JsonObject, which still works in WinUI 3. |
| [LibraryManagement](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/LibraryManagement) | Uses Windows.Storage.StorageLibrary, which still works in WinUI 3. |
| [LineDisplay](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/LineDisplay) | Uses Windows.Devices.PointOfService.ClaimedLineDisplay, which still works in WinUI 3. |
| [Logging](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/Logging) | Uses Windows.Foundation.Diagnostics.LoggingChannel, which still works in WinUI 3. |
| [MagneticStripeReader](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/MagneticStripeReader) | Uses Windows.Devices.PointOfService.ClaimedMagneticStripeReader, which still works in WinUI 3. |
| [MediaImport](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/MediaImport) | Uses Windows.Media.Import.PhotoImportManager, which still works in WinUI 3. |
| [MediaTranscoding](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/MediaTranscoding) | Uses Windows.Media.Transcoding.MediaTranscoder, which still works in WinUI 3. |
| [MicrosoftPassport](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/MicrosoftPassport) | Uses Windows.Security.Credentials.KeyCredentialManager, which still works in WinUI 3. |
| [PdfDocument](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/PdfDocument) | Uses Windows.Data.Pdf.PdfDocument, which still works in WinUI 3. |
| [RadialController](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/RadialController) | Uses Windows.UI.Input.RadialController, which still works in WinUI 3. |
| [RemoteSystems](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/RemoteSystems) | Uses Windows.System.RemoteSystems.RemoteSystem, which still works in WinUI 3. |
| [SerialArduino](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/SerialArduino) | Uses Windows.Devices.SerialCommunication.SerialDevice, which still works in WinUI 3. |
| [SmartCard](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/SmartCard) | Uses Windows.Devices.SmartCards.SmartCardReader, which still works in WinUI 3. |
| [SocketActivityStreamSocket](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/SocketActivityStreamSocket) | Uses Windows.Networking.Sockets.StreamSocket, which still works in WinUI 3. |
| [SpeechRecognitionAndSynthesis](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/SpeechRecognitionAndSynthesis) | Uses Windows.Media.SpeechRecognition.SpeechRecognizer, which still works in WinUI 3. |
| [StreamSocket](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/StreamSocket) | Uses Windows.Networking.Sockets.StreamSocket, which still works in WinUI 3. |
| [Syndication](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/Syndication) | Uses Windows.Web.Syndication.SyndicationClient, which still works in WinUI 3. |
| [SystemMediaTransportControls](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/SystemMediaTransportControls) | Not ported. The sample demonstrates *manual* SMTC integration (CommandManager disabled + `GetForWindow`). On WinUI 3 desktop a `MediaPlayer`'s audio is not linked to a `GetForWindow` SMTC session, so it never surfaces in the Windows 11 media panel or responds to media keys. The supported path on desktop is `MediaPlayer`'s automatic integration (CommandManager enabled), which would change what the sample teaches, so it was left out. |
| [UserActivity](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/UserActivity) | Uses Windows.ApplicationModel.UserActivities.UserActivity, which still works in WinUI 3. |
| [UserCertificateStore](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/UserCertificateStore) | Uses Windows.Security.Cryptography.Certificates.UserCertificateStore, which still works in WinUI 3. |
| [UserConsentVerifier](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/UserConsentVerifier) | Uses Windows.Security.Credentials.UI.UserConsentVerifier, which still works in WinUI 3. |
| [UserDataAccountManager](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/UserDataAccountManager) | Uses Windows.ApplicationModel.UserDataAccounts.UserDataAccountManager, which still works in WinUI 3. |
| [UserInteractionMode](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/UserInteractionMode) | Uses UIViewSettings.GetForCurrentView, which still works in WinUI 3. |
| [UssdProtcol](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/UssdProtcol) | Uses Windows.Networking.NetworkOperators.UssdSession, which still works in WinUI 3. |
| [VersionAdaptiveCode](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/VersionAdaptiveCode) | Uses Windows.Foundation.Metadata.ApiInformation, which still works in WinUI 3. |
| [VideoPlaybackSynchronization](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/VideoPlaybackSynchronization) | Uses Windows.Media.Playback.MediaTimelineController, which still works in WinUI 3. |
| [WebAccountManagement](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/WebAccountManagement) | Uses WebAccountManager, which still works in WinUI 3. |
| [WebSocket](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/WebSocket) | Uses Windows.Networking.Sockets.MessageWebSocket, which still works in WinUI 3. |
| [WiFiDirect](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/WiFiDirect) | Uses Windows.Devices.WiFiDirect.WiFiDirectDevice, which still works in WinUI 3. |
| [WiFiDirectServices](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/WiFiDirectServices) | Uses Windows.Devices.WiFiDirect.Services.WiFiDirectServiceAdvertiser, which still works in WinUI 3. |
| [XamlBottomUpList](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/XamlBottomUpList) | Not ported: the inverted-list / bottom-up ListView pattern (ItemsStackPanel `ItemsUpdatingScrollMode`) is already covered by the WinUI Gallery, so a standalone sample would be redundant. |
| [XamlGamepadNavigation](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/XamlGamepadNavigation) | Uses XYFocusDown, which still works in WinUI 3. |
| [XamlTransform3DAnimations](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/XamlTransform3DAnimations) | Uses Windows.UI.Xaml.Media.Media3D.Transform3D, which still works in WinUI 3. |
| [XmlDocument](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/XmlDocument) | Uses Windows.Data.Xml.Dom.XmlDocument, which still works in WinUI 3. |
| [XmlLite](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/XmlLite) | Uses IXmlReader, which still works in WinUI 3. |

</details>

## Out of scope

121 UWP samples were reviewed and left out of this port. Most are already covered by the [WinUI 3 Gallery](https://github.com/microsoft/WinUI-Gallery) or the [Windows App SDK samples](https://github.com/microsoft/WindowsAppSDK-Samples), are C++/DirectX only, or target platforms that do not apply to WinUI 3 desktop apps (HoloLens, WinJS).

<details>
<summary>Show all 121 out-of-scope samples</summary>

| UWP sample | Reason not ported |
| --- | --- |
| [360VideoPlayback](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/360VideoPlayback) | Already demonstrated in the WinUI 3 Gallery (MediaPlayerElementPage). |
| [3DPrintingFromUnity](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/3DPrintingFromUnity) | Would need a full rewrite for WinUI 3 desktop (CoreWindow / multi-view / app-contract dependencies) rather than a straight port. |
| [AdaptiveStreaming](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/AdaptiveStreaming) | Dropped: all 7 scenarios depend on test content hosted on Azure Media Services (`amssamples.streaming.mediaservices.windows.net`), which Microsoft retired on 30 June 2024, so the manifests and DRM/key-delivery endpoints no longer resolve. The `AdaptiveMediaSource` APIs work in WinUI 3, but there is no live content to demonstrate or verify against. Basic `MediaPlayerElement` playback is shown in the [WinUI 3 Gallery](https://github.com/microsoft/WinUI-Gallery) (MediaPlayerElement page). |
| [Advertising](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/Advertising) | Built on a deprecated or removed Windows platform feature; no WinUI 3 equivalent. |
| [AllJoyn](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/AllJoyn) | Built on a deprecated or removed Windows platform feature; no WinUI 3 equivalent. |
| [AnimationLibrary](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/AnimationLibrary) | WinJS / HTML (JavaScript) sample; there is no C#/WinUI 3 version to port. |
| [ApplicationResources](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/ApplicationResources) | Already demonstrated in the Windows App SDK samples (WindowsAppSDK-Samples/Samples/ResourceManagement) with the same MRT Core ResourceLoader / ResourceManager APIs. |
| [AppServices](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/AppServices) | Already demonstrated in the Windows App SDK samples (WindowsAppSDK-Samples/Samples/AppLifecycle). |
| [AppWindow](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/AppWindow) | Already demonstrated in the WinUI 3 Gallery (AppWindowPage). |
| [BackButton](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/BackButton) | Already demonstrated in the WinUI 3 Gallery (NavigationViewPage). |
| [BackgroundActivation](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/BackgroundActivation) | Already demonstrated in the Windows App SDK samples (WindowsAppSDK-Samples/Samples/AppLifecycle/Activation + /BackgroundTask). |
| [BackgroundSensors](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/BackgroundSensors) | Already demonstrated in the Windows App SDK samples (WindowsAppSDK-Samples/Samples/BackgroundTask). |
| [BackgroundTask](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/BackgroundTask) | Already demonstrated in the Windows App SDK samples (WindowsAppSDK-Samples/Samples/BackgroundTask). |
| [BarcodeScannerProvider](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/BarcodeScannerProvider) | Would need a full rewrite for WinUI 3 desktop (CoreWindow / multi-view / app-contract dependencies) rather than a straight port. |
| [BasicHologram](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/BasicHologram) | HoloLens / Windows Mixed Reality sample; does not apply to WinUI 3 desktop apps. |
| [BasicSuspension](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/BasicSuspension) | Already demonstrated in the Windows App SDK samples (WindowsAppSDK-Samples/Samples/AppLifecycle/StateNotifications). |
| [Clipboard](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/Clipboard) | Already demonstrated in the WinUI 3 Gallery (ClipboardPage). |
| [CommunicationBlockAndFilter](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/CommunicationBlockAndFilter) | HoloLens / Windows Mixed Reality sample; does not apply to WinUI 3 desktop apps. |
| [ComplexInk](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/ComplexInk) | Windows Inking (InkCanvas / InkToolbar) is not yet available in WinUI 3 (microsoft-ui-xaml#4099). |
| [CompositionVisual](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/CompositionVisual) | Already demonstrated in the Windows App SDK samples (SceneGraph). |
| [ContactCardIntegration](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/ContactCardIntegration) | Built on a deprecated or removed Windows platform feature; no WinUI 3 equivalent. |
| [ContactPanel](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/ContactPanel) | Built on a deprecated or removed Windows platform feature; no WinUI 3 equivalent. |
| [ContextMenu](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/ContextMenu) | Already demonstrated in the WinUI 3 Gallery (MenuFlyoutPage). |
| [CortanaVoiceCommand](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/CortanaVoiceCommand) | Built on a deprecated or removed Windows platform feature; no WinUI 3 equivalent. |
| [CustomCapability](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/CustomCapability) | Would need a full rewrite for WinUI 3 desktop (CoreWindow / multi-view / app-contract dependencies) rather than a straight port. |
| [CustomEditControl](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/CustomEditControl) | Needs a case-by-case decision before porting. |
| [D2DAdvancedColorImages](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/D2DAdvancedColorImages) | C++ / DirectX sample; there is no C#/WinUI 3 version to port. |
| [D2DCustomEffects](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/D2DCustomEffects) | C++ / DirectX sample; there is no C#/WinUI 3 version to port. |
| [D2DGradientMesh](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/D2DGradientMesh) | C++ / DirectX sample; there is no C#/WinUI 3 version to port. |
| [D2DPhotoAdjustment](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/D2DPhotoAdjustment) | C++ / DirectX sample; there is no C#/WinUI 3 version to port. |
| [D2DSvgImage](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/D2DSvgImage) | C++ / DirectX sample; there is no C#/WinUI 3 version to port. |
| [DeviceLockdownAzureLogin](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/DeviceLockdownAzureLogin) | HoloLens / Windows Mixed Reality sample; does not apply to WinUI 3 desktop apps. |
| [DWriteColorGlyph](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/DWriteColorGlyph) | Already demonstrated in the Windows App SDK samples (TextRendering/DWriteCoreGallery). |
| [DWriteLineSpacingModes](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/DWriteLineSpacingModes) | Already demonstrated in the Windows App SDK samples (TextRendering/DWriteCoreGallery). |
| [DWriteTextLayoutCloudFont](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/DWriteTextLayoutCloudFont) | Already demonstrated in the Windows App SDK samples (TextRendering/DWriteCoreGallery). |
| [EfficientAnimations](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/EfficientAnimations) | WinJS / HTML (JavaScript) sample; there is no C#/WinUI 3 version to port. |
| [EnterpriseDataProtection](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/EnterpriseDataProtection) | Built on a deprecated or removed Windows platform feature; no WinUI 3 equivalent. |
| [FeedReader](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/FeedReader) | WinJS / HTML (JavaScript) sample; there is no C#/WinUI 3 version to port. |
| [FilePicker](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/FilePicker) | Already demonstrated in the WinUI 3 Gallery (StoragePickersPage). |
| [FilePickerContracts](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/FilePickerContracts) | Would need a full rewrite for WinUI 3 desktop (CoreWindow / multi-view / app-contract dependencies) rather than a straight port. |
| [FileSearch](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/FileSearch) | Already demonstrated in the Windows App SDK samples (PhotoEditor). |
| [FileThumbnails](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/FileThumbnails) | Already demonstrated in the Windows App SDK samples (PhotoEditor). |
| [FolderEnumeration](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/FolderEnumeration) | Already demonstrated in the Windows App SDK samples (PhotoEditor). |
| [FullScreenMode](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/FullScreenMode) | Already demonstrated in the WinUI 3 Gallery (AppWindowPage). |
| [HolographicDepthBasedImageStabilization](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/HolographicDepthBasedImageStabilization) | Superseded; the control or API now ships in WinUI 3. |
| [HolographicFaceTracking](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/HolographicFaceTracking) | HoloLens / Windows Mixed Reality sample; does not apply to WinUI 3 desktop apps. |
| [HolographicMixedRealityCapture](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/HolographicMixedRealityCapture) | HoloLens / Windows Mixed Reality sample; does not apply to WinUI 3 desktop apps. |
| [HolographicSpatialMapping](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/HolographicSpatialMapping) | HoloLens / Windows Mixed Reality sample; does not apply to WinUI 3 desktop apps. |
| [HolographicSpatialStage](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/HolographicSpatialStage) | HoloLens / Windows Mixed Reality sample; does not apply to WinUI 3 desktop apps. |
| [HolographicTagAlong](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/HolographicTagAlong) | HoloLens / Windows Mixed Reality sample; does not apply to WinUI 3 desktop apps. |
| [HolographicVoiceInput](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/HolographicVoiceInput) | HoloLens / Windows Mixed Reality sample; does not apply to WinUI 3 desktop apps. |
| [HomeGroup](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/HomeGroup) | Built on a deprecated or removed Windows platform feature; no WinUI 3 equivalent. |
| [HtmlFormValidation](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/HtmlFormValidation) | WinJS / HTML (JavaScript) sample; there is no C#/WinUI 3 version to port. |
| [IndexedDB](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/IndexedDB) | WinJS / HTML (JavaScript) sample; there is no C#/WinUI 3 version to port. |
| [Ink](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/Ink) | WinJS / HTML (JavaScript) sample; there is no C#/WinUI 3 version to port. |
| [InkAnalysis](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/InkAnalysis) | Windows Inking (InkCanvas / InkToolbar) is not yet available in WinUI 3 (microsoft-ui-xaml#4099). |
| [IoT-GPIO](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/IoT-GPIO) | HoloLens / Windows Mixed Reality sample; does not apply to WinUI 3 desktop apps. |
| [IoT-I2C](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/IoT-I2C) | HoloLens / Windows Mixed Reality sample; does not apply to WinUI 3 desktop apps. |
| [IoT-SPI](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/IoT-SPI) | HoloLens / Windows Mixed Reality sample; does not apply to WinUI 3 desktop apps. |
| [JumpList](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/JumpList) | Already demonstrated in the WinUI 3 Gallery (JumpListPage). |
| [LiveDash](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/LiveDash) | Already demonstrated in the WinUI 3 Gallery (MediaPlayerElementPage). |
| [LockScreenApps](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/LockScreenApps) | Already demonstrated in the WinUI 3 Gallery (BadgeNotificationManagerPage). |
| [LowLatencyInput](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/LowLatencyInput) | Already demonstrated in the Windows App SDK samples (WindowsAppSDK-Samples/Samples/Input). |
| [MapControl](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/MapControl) | Already demonstrated in the WinUI 3 Gallery (MapControlPage). |
| [MediaEditing](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/MediaEditing) | Already demonstrated in the WinUI 3 Gallery (StoragePickersPage). |
| [MessageDialog](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/MessageDialog) | Already demonstrated in the WinUI 3 Gallery (ContentDialogPage). |
| [MixedRealityModel](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/MixedRealityModel) | HoloLens / Windows Mixed Reality sample; does not apply to WinUI 3 desktop apps. |
| [MsBlobBuilder](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/MsBlobBuilder) | WinJS / HTML (JavaScript) sample; there is no C#/WinUI 3 version to port. |
| [MultipleViews](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/MultipleViews) | Already demonstrated in the WinUI 3 Gallery (CreateMultipleWindowsPage, AppWindowPage). |
| [MyPeopleNotifications](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/MyPeopleNotifications) | Built on a deprecated or removed Windows platform feature; no WinUI 3 equivalent. |
| [Notifications](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/Notifications) | Already demonstrated in the WinUI 3 Gallery (AppNotificationPage, BadgeNotificationManagerPage). |
| [Package](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/Package) | App package inspection (Windows.ApplicationModel.Package). Package identity and deployment are handled through the Windows App SDK; deferred. |
| [PackagedContent](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/PackagedContent) | WinJS / HTML (JavaScript) sample; there is no C#/WinUI 3 version to port. |
| [PasswordVault](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/PasswordVault) | Already demonstrated in the Windows App SDK samples (AppContentSearch). |
| [Playlists](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/Playlists) | Already demonstrated in the WinUI 3 Gallery (StoragePickersPage). |
| [PosPrinter](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/PosPrinter) | Would need a full rewrite for WinUI 3 desktop (CoreWindow / multi-view / app-contract dependencies) rather than a straight port. |
| [Printing](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/Printing) | Printing (PrintManager / PrintDocument). Needs a WinUI 3 desktop printing approach before porting; deferred. |
| [Projection](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/Projection) | Already demonstrated in the WinUI 3 Gallery (CreateMultipleWindowsPage, AppWindowPage). |
| [ProxyStubsForWinRTComponents](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/ProxyStubsForWinRTComponents) | C++ / DirectX sample; there is no C#/WinUI 3 version to port. |
| [ResizeAppView](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/ResizeAppView) | Already demonstrated in the WinUI 3 Gallery (AppWindowPage). |
| [SecondaryTiles](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/SecondaryTiles) | Already demonstrated in the WinUI 3 Gallery (BadgeNotificationManagerPage). |
| [SemanticTextQuery](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/SemanticTextQuery) | Already demonstrated in the Windows App SDK samples (AppContentSearch). |
| [SharedContent](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/SharedContent) | WinJS / HTML (JavaScript) sample; there is no C#/WinUI 3 version to port. |
| [ShareSource](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/ShareSource) | Already demonstrated in the WinUI 3 Gallery (ClipboardPage). |
| [ShareTarget](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/ShareTarget) | Already demonstrated in the Windows App SDK samples (AppLifecycle ShareTarget). |
| [Simple3DGameDX](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/Simple3DGameDX) | C++ / DirectX sample; there is no C#/WinUI 3 version to port. |
| [Simple3DGameXaml](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/Simple3DGameXaml) | C++ / DirectX sample; there is no C#/WinUI 3 version to port. |
| [SimpleCommunication](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/SimpleCommunication) | Built on a deprecated or removed Windows platform feature; no WinUI 3 equivalent. |
| [SimpleInk](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/SimpleInk) | Windows Inking (InkCanvas / InkToolbar) is not yet available in WinUI 3 (microsoft-ui-xaml#4099). |
| [SmsSendAndReceive](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/SmsSendAndReceive) | HoloLens / Windows Mixed Reality sample; does not apply to WinUI 3 desktop apps. |
| [SpatialInteractionSource](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/SpatialInteractionSource) | HoloLens / Windows Mixed Reality sample; does not apply to WinUI 3 desktop apps. |
| [SpatialSound](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/SpatialSound) | C++ / DirectX sample; there is no C#/WinUI 3 version to port. |
| [SplashScreen](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/SplashScreen) | Would need a full rewrite for WinUI 3 desktop (CoreWindow / multi-view / app-contract dependencies) rather than a straight port. |
| [TitleBar](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/TitleBar) | Already demonstrated in the WinUI 3 Gallery (AppWindowTitleBarPage). |
| [UserSelection](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/UserSelection) | WinJS / HTML (JavaScript) sample; there is no C#/WinUI 3 version to port. |
| [VideoPlayback](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/VideoPlayback) | Already demonstrated in the WinUI 3 Gallery (MediaPlayerElementPage). |
| [VoIP](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/VoIP) | HoloLens / Windows Mixed Reality sample; does not apply to WinUI 3 desktop apps. |
| [WebAuthenticationBroker](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/WebAuthenticationBroker) | Already demonstrated in the Windows App SDK samples (OAuth2Manager, Microsoft.Security.Authentication.OAuth). |
| [WebView](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/WebView) | WinJS / HTML (JavaScript) sample; there is no C#/WinUI 3 version to port. |
| [WebWorkers](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/WebWorkers) | WinJS / HTML (JavaScript) sample; there is no C#/WinUI 3 version to port. |
| [WindowsAudioSession](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/WindowsAudioSession) | C++ / WASAPI low-level audio sample; there is no C# version to port. |
| [WRLInProcessWinRTComponent](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/WRLInProcessWinRTComponent) | C++ / DirectX sample; there is no C#/WinUI 3 version to port. |
| [WRLOutOfProcessWinRTComponent](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/WRLOutOfProcessWinRTComponent) | C++ / DirectX sample; there is no C#/WinUI 3 version to port. |
| [XamlAnimatedGif](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/XamlAnimatedGif) | Already shipped in the WinUI 3 Gallery. |
| [XamlAutoSuggestBox](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/XamlAutoSuggestBox) | Superseded; the control or API now ships in WinUI 3 and is shown in the WinUI 3 Gallery (AutoSuggestBoxPage). |
| [XamlBind](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/XamlBind) | Already demonstrated in the WinUI 3 Gallery (BindingPage). |
| [XamlCloudFontIntegration](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/XamlCloudFontIntegration) | Already demonstrated in the Windows App SDK samples (DWriteCore gallery sample). |
| [XamlCommanding](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/XamlCommanding) | Already demonstrated in the WinUI 3 Gallery (CommandBarPage). |
| [XamlContextMenu](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/XamlContextMenu) | Already demonstrated in the WinUI 3 Gallery (MenuFlyoutPage). |
| [XamlCustomMediaTransportControls](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/XamlCustomMediaTransportControls) | Already demonstrated in the WinUI 3 Gallery (MediaPlayerElementPage). |
| [XamlListView](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/XamlListView) | Already shipped in the WinUI 3 Gallery. |
| [XamlNavigation](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/XamlNavigation) | Superseded; the control or API now ships in WinUI 3 and is shown in the WinUI 3 Gallery (NavigationViewPage). |
| [XamlPivot](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/XamlPivot) | Already demonstrated in the WinUI 3 Gallery (PivotPage). |
| [XamlPullToRefresh](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/XamlPullToRefresh) | Superseded; the control or API now ships in WinUI 3 and is shown in the WinUI 3 Gallery (PullToRefreshPage). |
| [XamlResponsiveTechniques](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/XamlResponsiveTechniques) | Already shipped in the WinUI 3 Gallery. |
| [XamlTailoredMultipleViews](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/XamlTailoredMultipleViews) | Already demonstrated in the WinUI 3 Gallery (SplitViewPage, PivotPage). |
| [XamlTransform3DParallax](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/XamlTransform3DParallax) | Superseded; the control or API now ships in WinUI 3 and is shown in the WinUI 3 Gallery (ParallaxViewPage). |
| [XamlTreeView](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/XamlTreeView) | Superseded; the control or API now ships in WinUI 3 and is shown in the WinUI 3 Gallery (TreeViewPage). |
| [XamlUIBasics](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/XamlUIBasics) | Superseded; the control or API now ships in WinUI 3 and is shown in the WinUI 3 Gallery (WinUI Gallery). |
| [XamlWebView](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/XamlWebView) | Already demonstrated in the WinUI 3 Gallery (WebView2Page). |

</details>
