// Copyright (c) Microsoft Corporation. 
// Licensed under the MIT License. 
using System;
using System.Linq;
using System.Windows.Forms;
using Microsoft.UI;
using Microsoft.UI.Windowing;
using WinRT;

namespace winforms_unpackaged_app
{
    public partial class AppForm : Form
    {
        public string WindowTitle = "Windowing WinForms C# Sample";
        private AppWindow _mainAppWindow;
        private bool _isBrandedTitleBar = false;

        public AppForm()
        {
            this.InitializeComponent();

            // Gets the AppWindow using the windowing interop methods (see WindowingInterop.cs for details)
            _mainAppWindow = AppWindowExtensions.GetAppWindowFromWinformsWindow(this);
            _mainAppWindow.Changed += AppWindowChangedHandler;
            _mainAppWindow.Title = WindowTitle;
        }

        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(AppForm));
            this.toggleCompactOverlay = new System.Windows.Forms.Button();
            this.toggleFullScreen = new System.Windows.Forms.Button();
            this.presenterHeader = new System.Windows.Forms.Label();
            this.overlapped = new System.Windows.Forms.Button();
            this.customizeWindowHeader = new System.Windows.Forms.Label();
            this.windowTitleTextBox = new System.Windows.Forms.TextBox();
            this.setWindowTitleButton = new System.Windows.Forms.Button();
            this.widthTextBox = new System.Windows.Forms.TextBox();
            this.heightTextBox = new System.Windows.Forms.TextBox();
            this.resizeButton = new System.Windows.Forms.Button();
            this.customTitleBarHeader = new System.Windows.Forms.Label();
            this.titlebarBrandingBtn = new System.Windows.Forms.Button();
            this.titlebarCustomBtn = new System.Windows.Forms.Button();
            this.resetTitlebarBtn = new System.Windows.Forms.Button();
            this.MyTitleBar = new System.Windows.Forms.FlowLayoutPanel();
            this.MyWindowIcon = new System.Windows.Forms.PictureBox();
            this.MyWindowTitle = new System.Windows.Forms.Label();
            this.clientResizeButton = new System.Windows.Forms.Button();
            this.standardHeightButton = new System.Windows.Forms.Button();
            this.tallHeightButton = new System.Windows.Forms.Button();
            this.MyTitleBar.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.MyWindowIcon)).BeginInit();
            this.SuspendLayout();
            // 
            // toggleCompactOverlay
            // 
            this.toggleCompactOverlay.Location = new System.Drawing.Point(12, 223);
            this.toggleCompactOverlay.Name = "toggleCompactOverlay";
            this.toggleCompactOverlay.Size = new System.Drawing.Size(176, 27);
            this.toggleCompactOverlay.TabIndex = 1;
            this.toggleCompactOverlay.Text = "Enter Compact Overlay";
            this.toggleCompactOverlay.UseVisualStyleBackColor = true;
            this.toggleCompactOverlay.Click += new System.EventHandler(this.SwitchPresenter);
            // 
            // toggleFullScreen
            // 
            this.toggleFullScreen.Location = new System.Drawing.Point(194, 223);
            this.toggleFullScreen.Name = "toggleFullScreen";
            this.toggleFullScreen.Size = new System.Drawing.Size(144, 27);
            this.toggleFullScreen.TabIndex = 2;
            this.toggleFullScreen.Text = "Enter Full Screen";
            this.toggleFullScreen.UseVisualStyleBackColor = true;
            this.toggleFullScreen.Click += new System.EventHandler(this.SwitchPresenter);
            // 
            // presenterHeader
            // 
            this.presenterHeader.AutoSize = true;
            this.presenterHeader.ForeColor = System.Drawing.Color.DodgerBlue;
            this.presenterHeader.Location = new System.Drawing.Point(12, 200);
            this.presenterHeader.Name = "presenterHeader";
            this.presenterHeader.Size = new System.Drawing.Size(196, 15);
            this.presenterHeader.TabIndex = 3;
            this.presenterHeader.Text = "Change your AppWindow Presenter";
            // 
            // overlapped
            // 
            this.overlapped.Location = new System.Drawing.Point(344, 223);
            this.overlapped.Name = "overlapped";
            this.overlapped.Size = new System.Drawing.Size(148, 27);
            this.overlapped.TabIndex = 4;
            this.overlapped.Text = "Enter Overlapped ";
            this.overlapped.UseVisualStyleBackColor = true;
            this.overlapped.Click += new System.EventHandler(this.SwitchPresenter);
            // 
            // customizeWindowHeader
            // 
            this.customizeWindowHeader.AutoSize = true;
            this.customizeWindowHeader.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.customizeWindowHeader.ForeColor = System.Drawing.Color.DodgerBlue;
            this.customizeWindowHeader.Location = new System.Drawing.Point(12, 97);
            this.customizeWindowHeader.Name = "customizeWindowHeader";
            this.customizeWindowHeader.Size = new System.Drawing.Size(110, 15);
            this.customizeWindowHeader.TabIndex = 5;
            this.customizeWindowHeader.Text = "Customize Window";
            // 
            // windowTitleTextBox
            // 
            this.windowTitleTextBox.Location = new System.Drawing.Point(12, 120);
            this.windowTitleTextBox.Name = "windowTitleTextBox";
            this.windowTitleTextBox.Size = new System.Drawing.Size(191, 23);
            this.windowTitleTextBox.TabIndex = 6;
            this.windowTitleTextBox.Text = "WinForms ❤️ AppWindow";
            // 
            // setWindowTitleButton
            // 
            this.setWindowTitleButton.Location = new System.Drawing.Point(209, 120);
            this.setWindowTitleButton.Name = "setWindowTitleButton";
            this.setWindowTitleButton.Size = new System.Drawing.Size(133, 27);
            this.setWindowTitleButton.TabIndex = 7;
            this.setWindowTitleButton.Text = "Set Window Title";
            this.setWindowTitleButton.UseVisualStyleBackColor = true;
            this.setWindowTitleButton.Click += new System.EventHandler(this.windowTitleButton_Click);
            // 
            // widthTextBox
            // 
            this.widthTextBox.Location = new System.Drawing.Point(12, 153);
            this.widthTextBox.Name = "widthTextBox";
            this.widthTextBox.Size = new System.Drawing.Size(50, 23);
            this.widthTextBox.TabIndex = 9;
            // 
            // heightTextBox
            // 
            this.heightTextBox.Location = new System.Drawing.Point(68, 153);
            this.heightTextBox.Name = "heightTextBox";
            this.heightTextBox.Size = new System.Drawing.Size(50, 23);
            this.heightTextBox.TabIndex = 10;
            // 
            // resizeButton
            // 
            this.resizeButton.Location = new System.Drawing.Point(124, 153);
            this.resizeButton.Name = "resizeButton";
            this.resizeButton.Size = new System.Drawing.Size(119, 27);
            this.resizeButton.TabIndex = 11;
            this.resizeButton.Text = "Resize Window";
            this.resizeButton.UseVisualStyleBackColor = true;
            this.resizeButton.Click += new System.EventHandler(this.resizeButton_Click);
            // 
            // customTitleBarHeader
            // 
            this.customTitleBarHeader.AutoSize = true;
            this.customTitleBarHeader.ForeColor = System.Drawing.Color.DodgerBlue;
            this.customTitleBarHeader.Location = new System.Drawing.Point(12, 264);
            this.customTitleBarHeader.Name = "customTitleBarHeader";
            this.customTitleBarHeader.Size = new System.Drawing.Size(91, 15);
            this.customTitleBarHeader.TabIndex = 12;
            this.customTitleBarHeader.Text = "Custom TitleBar";
            // 
            // titlebarBrandingBtn
            // 
            this.titlebarBrandingBtn.Location = new System.Drawing.Point(12, 287);
            this.titlebarBrandingBtn.Name = "titlebarBrandingBtn";
            this.titlebarBrandingBtn.Size = new System.Drawing.Size(231, 29);
            this.titlebarBrandingBtn.TabIndex = 13;
            this.titlebarBrandingBtn.Text = "Toggle TitleBar Color Branding";
            this.titlebarBrandingBtn.UseVisualStyleBackColor = true;
            this.titlebarBrandingBtn.Click += new System.EventHandler(this.titlebarBrandingBtn_Click);
            // 
            // titlebarCustomBtn
            // 
            this.titlebarCustomBtn.Location = new System.Drawing.Point(249, 287);
            this.titlebarCustomBtn.Name = "titlebarCustomBtn";
            this.titlebarCustomBtn.Size = new System.Drawing.Size(178, 29);
            this.titlebarCustomBtn.TabIndex = 14;
            this.titlebarCustomBtn.Text = "Toggle Custom TitleBar";
            this.titlebarCustomBtn.UseVisualStyleBackColor = true;
            this.titlebarCustomBtn.Click += new System.EventHandler(this.titlebarCustomBtn_Click);
            // 
            // resetTitlebarBtn
            // 
            this.resetTitlebarBtn.Location = new System.Drawing.Point(12, 365);
            this.resetTitlebarBtn.Name = "resetTitlebarBtn";
            this.resetTitlebarBtn.Size = new System.Drawing.Size(114, 29);
            this.resetTitlebarBtn.TabIndex = 15;
            this.resetTitlebarBtn.Text = "Reset TitleBar";
            this.resetTitlebarBtn.UseVisualStyleBackColor = true;
            this.resetTitlebarBtn.Click += new System.EventHandler(this.resetTitlebarBtn_Click);
            // 
            // MyTitleBar
            // 
            this.MyTitleBar.BackColor = System.Drawing.Color.Blue;
            this.MyTitleBar.Controls.Add(this.MyWindowIcon);
            this.MyTitleBar.Controls.Add(this.MyWindowTitle);
            this.MyTitleBar.ForeColor = System.Drawing.Color.White;
            this.MyTitleBar.Location = new System.Drawing.Point(1, -1);
            this.MyTitleBar.Name = "MyTitleBar";
            this.MyTitleBar.Size = new System.Drawing.Size(682, 32);
            this.MyTitleBar.TabIndex = 16;
            this.MyTitleBar.Visible = false;
            // 
            // MyWindowIcon
            // 
            this.MyWindowIcon.Image = ((System.Drawing.Image)(resources.GetObject("MyWindowIcon.Image")));
            this.MyWindowIcon.InitialImage = ((System.Drawing.Image)(resources.GetObject("MyWindowIcon.InitialImage")));
            this.MyWindowIcon.Location = new System.Drawing.Point(3, 3);
            this.MyWindowIcon.Name = "MyWindowIcon";
            this.MyWindowIcon.Size = new System.Drawing.Size(27, 26);
            this.MyWindowIcon.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.MyWindowIcon.TabIndex = 0;
            this.MyWindowIcon.TabStop = false;
            // 
            // MyWindowTitle
            // 
            this.MyWindowTitle.AutoSize = true;
            this.MyWindowTitle.Dock = System.Windows.Forms.DockStyle.Fill;
            this.MyWindowTitle.Location = new System.Drawing.Point(36, 5);
            this.MyWindowTitle.Margin = new System.Windows.Forms.Padding(3, 0, 3, 0);
            this.MyWindowTitle.Name = "MyWindowTitle";
            this.MyWindowTitle.Size = new System.Drawing.Size(217, 27);
            this.MyWindowTitle.TabIndex = 1;
            this.MyWindowTitle.Text = "Custom titlebar with interactive content";
            this.MyWindowTitle.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // clientResizeButton
            // 
            this.clientResizeButton.Location = new System.Drawing.Point(249, 153);
            this.clientResizeButton.Name = "clientResizeButton";
            this.clientResizeButton.Size = new System.Drawing.Size(119, 27);
            this.clientResizeButton.TabIndex = 17;
            this.clientResizeButton.Text = "Resize Client Area";
            this.clientResizeButton.UseVisualStyleBackColor = true;
            this.clientResizeButton.Click += new System.EventHandler(this.resizeButton_Click);
            // 
            // standardHeightButton
            // 
            this.standardHeightButton.Enabled = false;
            this.standardHeightButton.Location = new System.Drawing.Point(13, 329);
            this.standardHeightButton.Name = "standardHeightButton";
            this.standardHeightButton.Size = new System.Drawing.Size(190, 30);
            this.standardHeightButton.TabIndex = 18;
            this.standardHeightButton.Text = "Standard height title bar";
            this.standardHeightButton.UseVisualStyleBackColor = true;
            this.standardHeightButton.Click += new System.EventHandler(this.standardHeightButton_Click);
            // 
            // tallHeightButton
            // 
            this.tallHeightButton.Enabled = false;
            this.tallHeightButton.Location = new System.Drawing.Point(209, 329);
            this.tallHeightButton.Name = "tallHeightButton";
            this.tallHeightButton.Size = new System.Drawing.Size(150, 30);
            this.tallHeightButton.TabIndex = 19;
            this.tallHeightButton.Text = "Taller title bar";
            this.tallHeightButton.UseVisualStyleBackColor = true;
            this.tallHeightButton.Click += new System.EventHandler(this.tallHeightButton_Click);
            // 
            // AppForm
            // 
            this.BackgroundImageLayout = System.Windows.Forms.ImageLayout.None;
            this.ClientSize = new System.Drawing.Size(682, 453);
            this.Controls.Add(this.tallHeightButton);
            this.Controls.Add(this.standardHeightButton);
            this.Controls.Add(this.clientResizeButton);
            this.Controls.Add(this.MyTitleBar);
            this.Controls.Add(this.resetTitlebarBtn);
            this.Controls.Add(this.titlebarCustomBtn);
            this.Controls.Add(this.titlebarBrandingBtn);
            this.Controls.Add(this.customTitleBarHeader);
            this.Controls.Add(this.resizeButton);
            this.Controls.Add(this.heightTextBox);
            this.Controls.Add(this.widthTextBox);
            this.Controls.Add(this.setWindowTitleButton);
            this.Controls.Add(this.windowTitleTextBox);
            this.Controls.Add(this.customizeWindowHeader);
            this.Controls.Add(this.overlapped);
            this.Controls.Add(this.presenterHeader);
            this.Controls.Add(this.toggleFullScreen);
            this.Controls.Add(this.toggleCompactOverlay);
            this.Name = "AppForm";
            this.Activated += new System.EventHandler(this.AppForm_Activated);
            this.Load += new System.EventHandler(this.AppForm_Load);
            this.MyTitleBar.ResumeLayout(false);
            this.MyTitleBar.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.MyWindowIcon)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        private void AppWindowChangedHandler(AppWindow sender, AppWindowChangedEventArgs args)
        {
            // The presenter changed so we need to update the button captions to reflect the new state
            if (args.DidPresenterChange)
            {
                switch (_mainAppWindow.Presenter.Kind)
                {
                    case AppWindowPresenterKind.CompactOverlay:
                        toggleCompactOverlay.Text = "Exit CompactOverlay";
                        toggleFullScreen.Text = "Enter FullScreen";
                        break;

                    case AppWindowPresenterKind.FullScreen:
                        toggleCompactOverlay.Text = "Enter CompactOverlay";
                        toggleFullScreen.Text = "Exit FullScreen";
                        break;

                    case AppWindowPresenterKind.Overlapped:
                        toggleCompactOverlay.Text = "Enter CompactOverlay";
                        toggleFullScreen.Text = "Enter FullScreen";
                        break;

                    default:
                        // If we end up here the presenter was changed to something we don't know what it is.
                        // This would happen if a new presenter is introduced.
                        // We can ignore this situation since we are not aware of the presenter and have no UI that
                        // reacts to this unknown presenter.
                        break;
                }
            }
            if (args.DidSizeChange && sender.TitleBar.ExtendsContentIntoTitleBar)
            {
                // Need to update our drag region if the size of the window changes
                SetDragRegionForCustomTitleBar(sender);
            }
            if (args.DidSizeChange)
            {
                // update the size of custom titlebar as the window size changes
                MyTitleBar.Width = _mainAppWindow.Size.Width;
            }
        }

        private void SwitchPresenter(object sender, System.EventArgs e)
        {
            // Bail out if we don't have an AppWindow object.
            if (_mainAppWindow != null)
            {

                AppWindowPresenterKind newPresenterKind;
                switch (sender.As<Button>().Name)
                {
                    case "toggleCompactOverlay":
                        newPresenterKind = AppWindowPresenterKind.CompactOverlay;
                        break;

                    case "toggleFullScreen":
                        newPresenterKind = AppWindowPresenterKind.FullScreen;
                        break;

                    case "overlapped":
                        newPresenterKind = AppWindowPresenterKind.Overlapped;
                        break;

                    default:
                        newPresenterKind = AppWindowPresenterKind.Default;
                        break;
                }

                // If the same presenter button was pressed as the mode we're in, toggle the window back to Default.
                if (newPresenterKind == _mainAppWindow.Presenter.Kind)
                {
                    _mainAppWindow.SetPresenter(AppWindowPresenterKind.Default);
                }
                else
                {
                    // else request a presenter of the selected kind to be created and applied to the window.
                    _mainAppWindow.SetPresenter(newPresenterKind);
                }
            }
        }
        private void AppForm_Load(object sender, EventArgs e)
        {
        }
        private void AppForm_Activated(object sender, EventArgs e)
        { 
        }

        private void windowTitleButton_Click(object sender, EventArgs e)
        {
            _mainAppWindow.Title = windowTitleTextBox.Text;
        }

        private void resizeButton_Click(object sender, EventArgs e)
        {
            int windowWidth = 0;
            int windowHeight = 0;

            try
            {
                windowWidth = int.Parse(widthTextBox.Text);
                windowHeight = int.Parse(heightTextBox.Text);
            }
            catch (FormatException)
            {
                // Silently ignore invalid input...
            }

            if (_mainAppWindow != null)
            {
                switch (sender.As<Button>().Name)
                {
                    case "resizeButton":
                        _mainAppWindow.Resize(new Windows.Graphics.SizeInt32(windowWidth, windowHeight));
                        break;

                    case "clientResizeButton":
                        _mainAppWindow.ResizeClient(new Windows.Graphics.SizeInt32(windowWidth, windowHeight));
                        break;
                }
            }
        }

        private void titlebarBrandingBtn_Click(object sender, EventArgs e)
        {
            _mainAppWindow.TitleBar.ResetToDefault();

            _isBrandedTitleBar = !_isBrandedTitleBar;
            // Check to see if customization is supported. Currently only supported on Windows 11.
            if (AppWindowTitleBar.IsCustomizationSupported() && _isBrandedTitleBar)
            {
                _mainAppWindow.Title = "Default titlebar with custom color customization";
                _mainAppWindow.TitleBar.ForegroundColor = Colors.White;
                _mainAppWindow.TitleBar.BackgroundColor = Colors.DarkOrange;
                _mainAppWindow.TitleBar.InactiveBackgroundColor = Colors.Blue;
                _mainAppWindow.TitleBar.InactiveForegroundColor = Colors.White;

                // Buttons
                _mainAppWindow.TitleBar.ButtonBackgroundColor = Colors.DarkOrange;
                _mainAppWindow.TitleBar.ButtonForegroundColor = Colors.White;
                _mainAppWindow.TitleBar.ButtonInactiveBackgroundColor = Colors.Blue;
                _mainAppWindow.TitleBar.ButtonInactiveForegroundColor = Colors.White;
                _mainAppWindow.TitleBar.ButtonHoverBackgroundColor = Colors.Green;
                _mainAppWindow.TitleBar.ButtonHoverForegroundColor = Colors.White;
                _mainAppWindow.TitleBar.ButtonPressedBackgroundColor = Colors.DarkOrange;
                _mainAppWindow.TitleBar.ButtonPressedForegroundColor = Colors.White;
            }
            else
            {
                _mainAppWindow.Title = WindowTitle;
            }
            MyTitleBar.Visible = false;
        }

        private void titlebarCustomBtn_Click(object sender, EventArgs e)
        {
            _mainAppWindow.TitleBar.ExtendsContentIntoTitleBar = !_mainAppWindow.TitleBar.ExtendsContentIntoTitleBar;

            // Check to see if customization is supported. Currently only supported on Windows 11.
            if (AppWindowTitleBar.IsCustomizationSupported() && _mainAppWindow.TitleBar.ExtendsContentIntoTitleBar)
            {
                // Show the custom titlebar
                MyTitleBar.Visible = true;

                // Enable title bar height toggle buttons
                this.standardHeightButton.Enabled = true;
                this.tallHeightButton.Enabled = true;

                // Set Button colors to match the custom titlebar
                _mainAppWindow.TitleBar.ButtonBackgroundColor = Colors.Transparent;
                _mainAppWindow.TitleBar.ButtonForegroundColor = Colors.White;
                _mainAppWindow.TitleBar.ButtonInactiveBackgroundColor = Colors.Transparent;
                _mainAppWindow.TitleBar.ButtonInactiveForegroundColor = Colors.White;
                _mainAppWindow.TitleBar.ButtonHoverBackgroundColor = Colors.Green;
                _mainAppWindow.TitleBar.ButtonHoverForegroundColor = Colors.White;
                _mainAppWindow.TitleBar.ButtonPressedBackgroundColor = Colors.Green;
                _mainAppWindow.TitleBar.ButtonPressedForegroundColor = Colors.White;

                // Set the drag region for the custom TitleBar
                SetDragRegionForCustomTitleBar(_mainAppWindow);
            }
            else
            {
                // Bring back the default titlebar
                MyTitleBar.Visible = false;

                // Disable title bar height toggle buttons
                this.standardHeightButton.Enabled = false;
                this.tallHeightButton.Enabled = false;

                // reset the title bar to default state
                _mainAppWindow.TitleBar.ResetToDefault();
            }
        }

        private void SetDragRegionForCustomTitleBar(AppWindow appWindow)
        {
            //Infer titlebar height
            int titleBarHeight = appWindow.TitleBar.Height;
            MyTitleBar.Height = titleBarHeight;

            // Get caption button occlusion information
            // Use LeftInset if you've explicitly set your window layout to RTL or if app language is a RTL language
            int CaptionButtonOcclusionWidth = appWindow.TitleBar.RightInset;

            // Define your drag Regions
            int windowIconWidthAndPadding = MyWindowIcon.Width + MyWindowIcon.Margin.Right;
            int dragRegionWidth = appWindow.Size.Width - (CaptionButtonOcclusionWidth + windowIconWidthAndPadding);

            Windows.Graphics.RectInt32[] dragRects = new Windows.Graphics.RectInt32[] { };
            Windows.Graphics.RectInt32 dragRect;

            dragRect.X = windowIconWidthAndPadding;
            dragRect.Y = 0;
            dragRect.Height = titleBarHeight;
            dragRect.Width = dragRegionWidth;

            appWindow.TitleBar.SetDragRectangles(dragRects.Append(dragRect).ToArray());
        }

        private void resetTitlebarBtn_Click(object sender, EventArgs e)
        {
            _mainAppWindow.TitleBar.ResetToDefault();
            _mainAppWindow.Title = WindowTitle;
            MyTitleBar.Visible = false;
        }

        private void standardHeightButton_Click(object sender, EventArgs e)
        {
            if (AppWindowTitleBar.IsCustomizationSupported() && _mainAppWindow.TitleBar.ExtendsContentIntoTitleBar)
            {
                // Set the title bar height to standard height
                _mainAppWindow.TitleBar.PreferredHeightOption = TitleBarHeightOption.Standard;

                // Adjust title and icon margins
                this.MyWindowTitle.Margin = new System.Windows.Forms.Padding(3, 0, 3, 0);
                this.MyWindowIcon.Margin = new System.Windows.Forms.Padding(3, 3, 3, 3);

                // Reset the drag region for the custom title bar
                SetDragRegionForCustomTitleBar(_mainAppWindow);
            }
            else
            {
                this.standardHeightButton.Enabled = false;
            }
        }
        private void tallHeightButton_Click(object sender, EventArgs e)
        {
            if (AppWindowTitleBar.IsCustomizationSupported() && _mainAppWindow.TitleBar.ExtendsContentIntoTitleBar)
            {
                // Use a tall title bar to provide more room for interactive elements like searchboxes, person pictures e.t.c
                _mainAppWindow.TitleBar.PreferredHeightOption = TitleBarHeightOption.Tall;

                // Adjust title and icon top margin
                this.MyWindowTitle.Margin = new System.Windows.Forms.Padding(3, 8, 3, 0);
                this.MyWindowIcon.Margin = new System.Windows.Forms.Padding(3, 8, 3, 3);

                // Reset the drag region for the custom title bar
                SetDragRegionForCustomTitleBar(_mainAppWindow);

            }
            else
            {
                this.tallHeightButton.Enabled = false;
            }
        }
    }
}
