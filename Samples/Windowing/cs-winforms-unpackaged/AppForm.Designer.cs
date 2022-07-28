// Copyright (c) Microsoft Corporation. 
// Licensed under the MIT License. 

namespace winforms_unpackaged_app
{
    partial class AppForm
    {
        /// <summary>
        ///  Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        ///  Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code
        #endregion
        private System.Windows.Forms.Button toggleCompactOverlay;
        private System.Windows.Forms.Button toggleFullScreen;
        private System.Windows.Forms.Label presenterHeader;
        private System.Windows.Forms.Button overlapped;
        private System.Windows.Forms.Label customizeWindowHeader;
        private System.Windows.Forms.TextBox windowTitleTextBox;
        private System.Windows.Forms.Button setWindowTitleButton;
        private System.Windows.Forms.TextBox widthTextBox;
        private System.Windows.Forms.TextBox heightTextBox;
        private System.Windows.Forms.Button resizeButton;
        private System.Windows.Forms.Label customTitleBarHeader;
        private System.Windows.Forms.Button titlebarBrandingBtn;
        private System.Windows.Forms.Button titlebarCustomBtn;
        private System.Windows.Forms.Button resetTitlebarBtn;
        private System.Windows.Forms.FlowLayoutPanel MyTitleBar;
        private System.Windows.Forms.PictureBox MyWindowIcon;
        private System.Windows.Forms.Label MyWindowTitle;
        private System.Windows.Forms.Button clientResizeButton;
        private System.Windows.Forms.Button standardHeightButton;
        private System.Windows.Forms.Button tallHeightButton;
    }
}

