//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH
// THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//*********************************************************

#include "pch.h"
#include "AnimationPage.h"
#if __has_include("AnimationPage.g.cpp")
#include "AnimationPage.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls::Primitives;

namespace winrt::DynamicRefreshRateTool::implementation
{
    const float DEFAULT_ROTATION_SPEED = 3600;

    AnimationPage::AnimationPage()
    {
        InitializeComponent();
        myStoryBoard().Begin();
    }

    void AnimationPage::Button_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& /*e*/)
    {
        auto buttonClicked = sender.try_as<ToggleButton>();

        for (auto button : { Button1(), Button2(), Button3() }) {
            if ((buttonClicked == button) != button.IsChecked().GetBoolean()) {
                button.OnToggle();
            }
        }

        if (Button1().IsChecked().GetBoolean()) {
            RotationAnimation().To(DEFAULT_ROTATION_SPEED / 2);
        }

        if (Button2().IsChecked().GetBoolean()) {
            RotationAnimation().To(DEFAULT_ROTATION_SPEED);
        }

        if (Button3().IsChecked().GetBoolean()) {
            RotationAnimation().To(DEFAULT_ROTATION_SPEED * 2);
        }
    }
}
