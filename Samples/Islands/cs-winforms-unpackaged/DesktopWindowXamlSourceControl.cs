using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Hosting;
using Microsoft.UI.Xaml.Media;
using Microsoft.UI;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using Microsoft.UI.Xaml;

namespace WinFormsWithIsland
{
    public partial class DesktopWindowXamlSourceControl : System.Windows.Forms.Control
    {
        public DesktopWindowXamlSourceControl()
        {
            InitializeComponent();
        }

        protected override void OnGotFocus(EventArgs e)
        {
            if (_desktopWindowXamlSource != null)
            {
                bool isShiftPressed = ((Form.ModifierKeys & Keys.Shift) != 0);
                var reason = isShiftPressed ? Microsoft.UI.Xaml.Hosting.XamlSourceFocusNavigationReason.Last : Microsoft.UI.Xaml.Hosting.XamlSourceFocusNavigationReason.First;
                var request = new XamlSourceFocusNavigationRequest(reason);
                _desktopWindowXamlSource.NavigateFocus(request);
            }
        }

        private void InitializeDesktopWindowXamlSource()
        {
            _desktopWindowXamlSource = new Microsoft.UI.Xaml.Hosting.DesktopWindowXamlSource();

            _desktopWindowXamlSource.Initialize(new WindowId((ulong)this.Handle));

            _desktopWindowXamlSource.TakeFocusRequested += (s, e) => {
                if (e.Request.Reason == Microsoft.UI.Xaml.Hosting.XamlSourceFocusNavigationReason.First)
                {
                    WindowsAppSdkHelper.FocusNextFocusableControl(this.Parent, this, true);
                }
                else if (e.Request.Reason == Microsoft.UI.Xaml.Hosting.XamlSourceFocusNavigationReason.Last)
                {
                    WindowsAppSdkHelper.FocusNextFocusableControl(this.Parent, this, false);
                }
            };

            _desktopWindowXamlSource.SiteBridge.MoveAndResize(new Windows.Graphics.RectInt32(0, 0, this.Width, this.Height));

            _frame = new Frame();
            _desktopWindowXamlSource.Content = _frame;

            _desktopWindowXamlSource.SystemBackdrop = new DesktopAcrylicBackdrop();

            if (_content != null)
            {
                _frame.Content = _content;
            }
        }

        protected override void OnResize(EventArgs e)
        {
            base.OnResize(e);
            if (_desktopWindowXamlSource != null)
            {
                _desktopWindowXamlSource.SiteBridge.MoveAndResize(new Windows.Graphics.RectInt32(0, 0, this.Width, this.Height));
            }
        }

        protected override void OnPaint(PaintEventArgs pe)
        {
            if (this.DesignMode)
            {
                base.OnPaint(pe);
                pe.Graphics.FillRectangle(Brushes.LightGray, this.ClientRectangle);
                pe.Graphics.DrawString("DesktopWindowXamlSourceControl", this.Font, Brushes.Black, 10, 10);
            }
            else if (_desktopWindowXamlSource == null)
            {
                InitializeDesktopWindowXamlSource();
            }
        }

        public FrameworkElement? Content
        {
            get
            {
                return _content;
            }
            set
            {
                _content = value;
                if (_frame != null)
                {
                    _frame.Content = _content;
                }
            }
        }

        FrameworkElement? _content;
        Frame? _frame;
        DesktopWindowXamlSource? _desktopWindowXamlSource;
    }
}
