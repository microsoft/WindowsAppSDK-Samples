#include <filesystem>
#include <iosfwd>
#include <Unknwn.h>
#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Graphics.Imaging.h>
#include <winrt/Microsoft.Windows.AI.MachineLearning.h>
#include <string>
#include <vector>

namespace ResNetModelHelper
{
std::wostream& operator<<(std::wostream& outputStream, const winrt::Microsoft::Windows::AI::MachineLearning::ExecutionProviderReadyResultState& readyResultState);

std::wostream& operator<<(std::wostream& outputStream, const winrt::Microsoft::Windows::AI::MachineLearning::ExecutionProviderReadyState& readyState);

std::filesystem::path GetExecutablePath();

winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Graphics::Imaging::SoftwareBitmap> LoadImageFileAsync(winrt::hstring filePath);

std::vector<float> BindSoftwareBitmapAsTensor(const winrt::Windows::Graphics::Imaging::SoftwareBitmap& bitmap);

std::vector<std::string> LoadLabels(const std::filesystem::path& labelsPath);

std::vector<float> Softmax(std::span<const float> logits);

std::vector<uint16_t> ConvertFloat32ToFloat16(std::span<const float> float32Data);

std::vector<float> ConvertFloat16ToFloat32(std::span<const uint16_t> float16Data);

void PrintResults(const std::vector<std::string>& labels, const std::vector<float>& results);
} // namespace ResNetModelHelper
