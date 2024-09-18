using Microsoft.UI.Xaml;
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
using Windows.Foundation;

namespace WinFormsWithIsland
{
    /// <summary>
    /// This control hosts a DesktopWindowXamlSource in a WinForms application.
    /// The DesktopWindowXamlSource is backed by a DesktopChildSiteBridge HWND that is a child of the WinForms Control.
    /// The HWND tree looks like this:
    /// * Top-level WinForms HWND
    ///   * DesktopWindowXamlSourceControl HWND
    ///     * DesktopChildSiteBridge HWND
    ///   * Other WinForms control HWNDs
    ///   * Other WinForms control HWNDs
    /// </summary>
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
                // The DesktopWindowXamlSourceControl is getting focus, let's redirect focus to the Xaml content by
                // calling DesktopWindowXamlSource.NavigateFocus.
                bool isShiftPressed = ((Form.ModifierKeys & Keys.Shift) != 0);
                var reason = isShiftPressed ? Microsoft.UI.Xaml.Hosting.XamlSourceFocusNavigationReason.Last : Microsoft.UI.Xaml.Hosting.XamlSourceFocusNavigationReason.First;
                var request = new XamlSourceFocusNavigationRequest(reason);
                _desktopWindowXamlSource.NavigateFocus(request);
            }
        }

        private void OnDesktopWindowXamlSourceTakeFocusRequested(object sender, DesktopWindowXamlSourceTakeFocusRequestedEventArgs e)
        {
            // The DesktopWindowXamlSource is requesting that the host take focus.
            // This typically happens when the user is tabbing through the controls in the Xaml content and reaches the first or last control.
            if (e.Request.Reason == XamlSourceFocusNavigationReason.First)
            {
                FocusNextFocusableWinFormsControl(this.Parent, this, true /*forward*/);
            }
            else if (e.Request.Reason == XamlSourceFocusNavigationReason.Last)
            {
                FocusNextFocusableWinFormsControl(this.Parent, this, false /*forward*/);
            }
        }

        private void InitializeDesktopWindowXamlSource()
        {
            _desktopWindowXamlSource = new Microsoft.UI.Xaml.Hosting.DesktopWindowXamlSource();

            _desktopWindowXamlSource.Initialize(new WindowId((ulong)this.Handle));

            _desktopWindowXamlSource.TakeFocusRequested += 
                new TypedEventHandler<DesktopWindowXamlSource, DesktopWindowXamlSourceTakeFocusRequestedEventArgs>(
                    OnDesktopWindowXamlSourceTakeFocusRequested);

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
                // Resize the DesktopChildSiteBridge HWND to match the size of the WinForms control, it's parent.
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

        /// <summary>
        /// Sets the content of the DesktopWindowXamlSource.
        /// </summary>
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

        /// <summary>
        /// Call to move focus to the next focusable control in the parent.
        /// </summary>
        /// <param name="parent">Control or Form that contains the controls.</param>
        /// <param name="start">Control we're starting with.</param>
        /// <param name="forward">If true, focus is moving forward.  If not, backward.</param>
        private static void FocusNextFocusableWinFormsControl(System.Windows.Forms.Control parent, System.Windows.Forms.Control start, bool forward)
        {
            // GetNextControl can return controls that aren't tab stops, so keep going until we find one that is.
            System.Windows.Forms.Control next = start;
            do
            {
                next = parent.GetNextControl(next, forward);
            }
            while (next != null && next.TabStop == false);

            if (next == null)
            {
                // Oops, we ran out of controls.  Get the first control in the parent.
                next = parent.GetNextControl(null, forward);
            }

            if (next != null)
            {
                next.Focus();
            }
        }

        private FrameworkElement? _content;
        private Frame? _frame;
        private DesktopWindowXamlSource? _desktopWindowXamlSource;
    }
}
