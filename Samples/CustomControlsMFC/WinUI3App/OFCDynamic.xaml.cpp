#include "pch.h"
#include "OFCDynamic.xaml.h"
#if __has_include("OFCDynamic.g.cpp")
#include "OFCDynamic.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace winrt::Windows::Foundation;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::WinUI3App::implementation
{
    OFCDynamic::OFCDynamic()
    {
        InitializeComponent();
        auto rt2 = RequestedTheme();
        auto rt3 = ActualTheme();
        auto appTheme = Application::Current().RequestedTheme();
        int k = 0;
        k++;
    }

    int32_t OFCDynamic::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void OFCDynamic::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

    void OFCDynamic::AddLabel_Click(IInspectable const& sender, RoutedEventArgs const& e)
    {
      LabelControl labelControl;
      SP2().Children().Append(labelControl);
      labelControl.text(L"TestLabel");
    }

    void OFCDynamic::AddButton_Click(IInspectable const& sender, RoutedEventArgs const& e)
    {
      ButtonControl buttonControl;
      SP2().Children().Append(buttonControl);
      buttonControl.text(L"TestButton");
    }

    void OFCDynamic::AddCheckBox_Click(IInspectable const& sender, RoutedEventArgs const& e)
    {
      CheckBoxControl checkBoxControl;
      SP2().Children().Append(checkBoxControl);
      checkBoxControl.checked(true);
    }

    void OFCDynamic::AddComboBox_Click(IInspectable const& sender, RoutedEventArgs const& e)
    {
      ComboBoxControl comboBoxControl;
      SP2().Children().Append(comboBoxControl);
      comboBoxControl.addItem(L"test1");
      comboBoxControl.addItem(L"test2");
    }

    void OFCDynamic::AddTextEdit_Click(IInspectable const& sender, RoutedEventArgs const& e)
    {
      TextEditControl textEditControl;
      SP2().Children().Append(textEditControl);
      textEditControl.text(L"Test");
      textEditControl.setPlaceholderText(L"Placeholder Test");
    }

    void OFCDynamic::AddProgressBar_Click(IInspectable const& sender, RoutedEventArgs const& e)
    {
      ProgressBarControl progressBarControl;
      SP2().Children().Append(progressBarControl);
      
    }

    void OFCDynamic::AddNumeric_Click(IInspectable const& sender, RoutedEventArgs const& e)
    {
      NumericControl numericControl;
      SP2().Children().Append(numericControl);
    }

    void OFCDynamic::AddRadioButton_Click(IInspectable const& sender, RoutedEventArgs const& e)
    {
      RadioButtonControl radioButtonControl;
      SP2().Children().Append(radioButtonControl);
      
      radioButtonControl.isChecked(true);
      radioButtonControl.text();
    }

    void OFCDynamic::AddSlider_Click(IInspectable const& sender, RoutedEventArgs const& e)
    {
      SliderControl sliderControl;
      SP2().Children().Append(sliderControl);

      sliderControl.setMaxValue(200);
      sliderControl.setMinValue(100);
      sliderControl.value(150);
    }

    void OFCDynamic::AddListBox_Click(IInspectable const& sender, RoutedEventArgs const& e)
    {
      ListBoxControl listBoxControl;
      SP2().Children().Append(listBoxControl);
      listBoxControl.addItem(L"list item");
    }
}



