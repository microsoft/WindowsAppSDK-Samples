﻿// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.
using System;
using System.Linq;
using System.Windows;
using System.Windows.Forms;
using Microsoft.UI;
using Microsoft.UI.Windowing;
using WinRT;

namespace winforms_unpackaged_app
{
    public partial class AppForm : Form
    {
        public static IWin32Window m_mainWindow;
        public String m_windowTitle = "WinForms Desktop C# Sample App";
        AppWindow m_mainAppWindow;
        bool m_isBrandedTitleBar = false;

        public AppForm()
        {
            this.InitializeComponent();

            m_mainWindow = this;

            // Gets the AppWindow using the windowing interop methods (see WindowingInterop.cs for details)
            m_mainAppWindow = AppWindowExtensions.GetAppWindowFromWinformsWindow(m_mainWindow);
            m_mainAppWindow.Changed += AppWindowChangedHandler;
            m_mainAppWindow.Title = m_windowTitle;
        }

        private void InitializeComponent()
        {
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
            this.MyTitleBar = new System.Windows.Forms.GroupBox();
            this.label1 = new System.Windows.Forms.Label();
            this.MyTitleBar.SuspendLayout();
            this.SuspendLayout();
            // 
            // toggleCompactOverlay
            // 
            this.toggleCompactOverlay.Location = new System.Drawing.Point(12, 223);
            this.toggleCompactOverlay.Name = "toggleCompactOverlay";
            this.toggleCompactOverlay.Size = new System.Drawing.Size(191, 27);
            this.toggleCompactOverlay.TabIndex = 1;
            this.toggleCompactOverlay.Text = "Toggle Compact Overlay";
            this.toggleCompactOverlay.UseVisualStyleBackColor = true;
            this.toggleCompactOverlay.Click += new System.EventHandler(this.SwitchPresenter);
            // 
            // toggleFullScreen
            // 
            this.toggleFullScreen.Location = new System.Drawing.Point(209, 223);
            this.toggleFullScreen.Name = "toggleFullScreen";
            this.toggleFullScreen.Size = new System.Drawing.Size(144, 27);
            this.toggleFullScreen.TabIndex = 2;
            this.toggleFullScreen.Text = "Toggle Full Screen";
            this.toggleFullScreen.UseVisualStyleBackColor = true;
            this.toggleFullScreen.Click += new System.EventHandler(this.SwitchPresenter);
            // 
            // presenterHeader
            // 
            this.presenterHeader.AutoSize = true;
            this.presenterHeader.ForeColor = System.Drawing.Color.DodgerBlue;
            this.presenterHeader.Location = new System.Drawing.Point(12, 200);
            this.presenterHeader.Name = "presenterHeader";
            this.presenterHeader.Size = new System.Drawing.Size(244, 20);
            this.presenterHeader.TabIndex = 3;
            this.presenterHeader.Text = "Change your AppWindow Presenter";
            // 
            // overlapped
            // 
            this.overlapped.Location = new System.Drawing.Point(359, 223);
            this.overlapped.Name = "overlapped";
            this.overlapped.Size = new System.Drawing.Size(165, 27);
            this.overlapped.TabIndex = 4;
            this.overlapped.Text = "Overlapped Presenter";
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
            this.customizeWindowHeader.Size = new System.Drawing.Size(137, 20);
            this.customizeWindowHeader.TabIndex = 5;
            this.customizeWindowHeader.Text = "Customize Window";
            // 
            // windowTitleTextBox
            // 
            this.windowTitleTextBox.Location = new System.Drawing.Point(12, 120);
            this.windowTitleTextBox.Name = "windowTitleTextBox";
            this.windowTitleTextBox.Size = new System.Drawing.Size(191, 27);
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
            this.widthTextBox.Size = new System.Drawing.Size(50, 27);
            this.widthTextBox.TabIndex = 9;
            // 
            // heightTextBox
            // 
            this.heightTextBox.Location = new System.Drawing.Point(68, 153);
            this.heightTextBox.Name = "heightTextBox";
            this.heightTextBox.Size = new System.Drawing.Size(50, 27);
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
            this.customTitleBarHeader.Size = new System.Drawing.Size(114, 20);
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
            this.resetTitlebarBtn.Location = new System.Drawing.Point(12, 322);
            this.resetTitlebarBtn.Name = "resetTitlebarBtn";
            this.resetTitlebarBtn.Size = new System.Drawing.Size(114, 29);
            this.resetTitlebarBtn.TabIndex = 15;
            this.resetTitlebarBtn.Text = "Reset TitleBar";
            this.resetTitlebarBtn.UseVisualStyleBackColor = true;
            this.resetTitlebarBtn.Click += new System.EventHandler(this.resetTitlebarBtn_Click);
            // 
            // MyTitleBar
            // 
            this.MyTitleBar.BackColor = System.Drawing.SystemColors.Highlight;
            this.MyTitleBar.Controls.Add(this.label1);
            this.MyTitleBar.Location = new System.Drawing.Point(-1, 0);
            this.MyTitleBar.Name = "MyTitleBar";
            this.MyTitleBar.Size = new System.Drawing.Size(688, 67);
            this.MyTitleBar.TabIndex = 16;
            this.MyTitleBar.TabStop = false;
            this.MyTitleBar.Visible = false;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(363, 13);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(50, 20);
            this.label1.TabIndex = 0;
            this.label1.Text = "label1";
            // 
            // AppForm
            // 
            this.BackgroundImageLayout = System.Windows.Forms.ImageLayout.None;
            this.ClientSize = new System.Drawing.Size(682, 453);
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
            this.Load += new System.EventHandler(this.AppForm_Load);
            this.MyTitleBar.ResumeLayout(false);
            this.MyTitleBar.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        private void AppWindowChangedHandler(AppWindow sender, AppWindowChangedEventArgs args)
        {
            // The presenter changed so we need to update the button captions to reflect the new state
            if (args.DidPresenterChange)
            {
                switch (m_mainAppWindow.Presenter.Kind)
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
        }

        private void SwitchPresenter(object sender, System.EventArgs e)
        {
            // Bail out if we don't have an AppWindow object.
            if (m_mainAppWindow != null)
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
                if (newPresenterKind == m_mainAppWindow.Presenter.Kind)
                {
                    m_mainAppWindow.SetPresenter(AppWindowPresenterKind.Default);
                }
                else
                {
                    // else request a presenter of the selected kind to be created and applied to the window.
                    m_mainAppWindow.SetPresenter(newPresenterKind);
                }
            }
        }

        private void AppForm_Load(object sender, EventArgs e)
        {

        }

        private void windowTitleButton_Click(object sender, EventArgs e)
        {
            m_mainAppWindow.Title = windowTitleTextBox.Text;
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

            if (windowHeight > 0 && windowWidth > 0)
            {
                m_mainAppWindow.Resize(new Windows.Graphics.SizeInt32(windowWidth, windowHeight));
            }
        }

        private void titlebarBrandingBtn_Click(object sender, EventArgs e)
        {
            m_mainAppWindow.TitleBar.ResetToDefault();

            m_isBrandedTitleBar = !m_isBrandedTitleBar;
            if (AppWindowTitleBar.IsCustomizationSupported() && m_isBrandedTitleBar)
            {
                m_mainAppWindow.Title = "Default titlebar with custom color customization";
                m_mainAppWindow.TitleBar.ForegroundColor = Colors.White;
                m_mainAppWindow.TitleBar.BackgroundColor = Colors.DarkOrange;
                m_mainAppWindow.TitleBar.InactiveBackgroundColor = Colors.Blue;
                m_mainAppWindow.TitleBar.InactiveForegroundColor = Colors.White;

                //Buttons
                m_mainAppWindow.TitleBar.ButtonBackgroundColor = Colors.DarkOrange;
                m_mainAppWindow.TitleBar.ButtonForegroundColor = Colors.White;
                m_mainAppWindow.TitleBar.ButtonInactiveBackgroundColor = Colors.Blue;
                m_mainAppWindow.TitleBar.ButtonInactiveForegroundColor = Colors.White;
                m_mainAppWindow.TitleBar.ButtonHoverBackgroundColor = Colors.Green;
                m_mainAppWindow.TitleBar.ButtonHoverForegroundColor = Colors.White;
                m_mainAppWindow.TitleBar.ButtonPressedBackgroundColor = Colors.DarkOrange;
                m_mainAppWindow.TitleBar.ButtonPressedForegroundColor = Colors.White;
            }
            else
            {
                m_mainAppWindow.Title = m_windowTitle;
            }
            MyTitleBar.Visible = false;
        }

        private void titlebarCustomBtn_Click(object sender, EventArgs e)
        {
            m_mainAppWindow.TitleBar.ExtendsContentIntoTitleBar = !m_mainAppWindow.TitleBar.ExtendsContentIntoTitleBar;

            if (AppWindowTitleBar.IsCustomizationSupported() && m_mainAppWindow.TitleBar.ExtendsContentIntoTitleBar)
            {
                // Show the custom titlebar
                MyTitleBar.Visible = true;

                // Set Button colors to match the custom titlebar
                m_mainAppWindow.TitleBar.ButtonBackgroundColor = Colors.Blue;
                m_mainAppWindow.TitleBar.ButtonForegroundColor = Colors.White;
                m_mainAppWindow.TitleBar.ButtonInactiveBackgroundColor = Colors.Blue;
                m_mainAppWindow.TitleBar.ButtonInactiveForegroundColor = Colors.White;
                m_mainAppWindow.TitleBar.ButtonHoverBackgroundColor = Colors.Green;
                m_mainAppWindow.TitleBar.ButtonHoverForegroundColor = Colors.White;
                m_mainAppWindow.TitleBar.ButtonPressedBackgroundColor = Colors.Green;
                m_mainAppWindow.TitleBar.ButtonPressedForegroundColor = Colors.White;

                //Infer titlebar height
                int titleBarHeight = m_mainAppWindow.TitleBar.Height;
                MyTitleBar.Height = titleBarHeight;

                // Get caption button occlusion information
                // Use LeftInset if you've explicitly set your window layout to RTL or if app language is a RTL language
                int CaptionButtonOcclusionWidth = m_mainAppWindow.TitleBar.RightInset;

                // Define your drag Regions
                //int windowIconWidthAndPadding = (int)m_mainWindow.MyWindowIcon.ActualWidth + (int)m_mainWindow.MyWindowIcon.Margin.Right;
                int windowIconWidthAndPadding = 0;
                int dragRegionWidth = m_mainAppWindow.Size.Width - (CaptionButtonOcclusionWidth + windowIconWidthAndPadding);

                Windows.Graphics.RectInt32[] dragRects = new Windows.Graphics.RectInt32[] { };
                Windows.Graphics.RectInt32 dragRect;

                dragRect.X = windowIconWidthAndPadding;
                dragRect.Y = 0;
                dragRect.Height = titleBarHeight;
                dragRect.Width = dragRegionWidth;

                var dragRectsArray = dragRects.Append(dragRect).ToArray();
                m_mainAppWindow.TitleBar.SetDragRectangles(dragRectsArray);
            }
            else
            {
                // Bring back the default titlebar
                MyTitleBar.Visible = false;
                m_mainAppWindow.TitleBar.ResetToDefault();
            }
        }

        private void resetTitlebarBtn_Click(object sender, EventArgs e)
        {
            m_mainAppWindow.TitleBar.ResetToDefault();
            m_mainAppWindow.Title = m_windowTitle;
            MyTitleBar.Visible = false;
        }


    }
}
