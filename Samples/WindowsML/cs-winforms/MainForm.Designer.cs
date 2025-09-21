// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE.md in the repo root for license information.

namespace WindowsMLWinFormsSample
{
    partial class MainForm
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
                selectedImagePictureBox.Image?.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        ///  Required method for Designer support - do not modify
        ///  the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.selectImageButton = new Button();
            this.runInferenceButton = new Button();
            this.allowProviderDownloadCheckBox = new CheckBox();
            this.selectedImagePictureBox = new PictureBox();
            this.resultsTextBox = new TextBox();
            this.imagePathLabel = new Label();
            this.label1 = new Label();
            this.label2 = new Label();
            // New EP related controls
            this.epLabel = new Label();
            this.epCombo = new ComboBox();
            this.deviceLabel = new Label();
            this.deviceCombo = new ComboBox();
            this.reloadSessionButton = new Button();
            ((System.ComponentModel.ISupportInitialize)(this.selectedImagePictureBox)).BeginInit();
            this.SuspendLayout();
            // 
            // epLabel
            // 
            this.epLabel.AutoSize = true;
            this.epLabel.Location = new Point(12, 12);
            this.epLabel.Name = "epLabel";
            this.epLabel.Size = new Size(108, 15);
            this.epLabel.TabIndex = 100;
            this.epLabel.Text = "Execution Provider:";
            // 
            // epCombo
            // 
            this.epCombo.DropDownStyle = ComboBoxStyle.DropDownList;
            this.epCombo.Location = new Point(140, 8);
            this.epCombo.Name = "epCombo";
            this.epCombo.Size = new Size(260, 23);
            this.epCombo.TabIndex = 101;
            this.epCombo.SelectedIndexChanged += new EventHandler(this.EpCombo_SelectedIndexChanged);
            // 
            // deviceLabel
            // 
            this.deviceLabel.AutoSize = true;
            this.deviceLabel.Location = new Point(12, 44);
            this.deviceLabel.Name = "deviceLabel";
            this.deviceLabel.Size = new Size(72, 15);
            this.deviceLabel.TabIndex = 102;
            this.deviceLabel.Text = "Device Type:";
            // 
            // deviceCombo
            // 
            this.deviceCombo.DropDownStyle = ComboBoxStyle.DropDownList;
            this.deviceCombo.Enabled = false;
            this.deviceCombo.Location = new Point(140, 40);
            this.deviceCombo.Name = "deviceCombo";
            this.deviceCombo.Size = new Size(180, 23);
            this.deviceCombo.TabIndex = 103;
            // 
            // reloadSessionButton
            // 
            this.reloadSessionButton.Location = new Point(340, 37);
            this.reloadSessionButton.Name = "reloadSessionButton";
            this.reloadSessionButton.Size = new Size(140, 30);
            this.reloadSessionButton.TabIndex = 104;
            this.reloadSessionButton.Text = "Load / Reload Model";
            this.reloadSessionButton.UseVisualStyleBackColor = true;
            this.reloadSessionButton.Click += new EventHandler(this.ReloadSessionButton_Click);
            // 
            // selectImageButton
            // 
            this.selectImageButton.Location = new Point(12, 78);
            this.selectImageButton.Name = "selectImageButton";
            this.selectImageButton.Size = new Size(120, 35);
            this.selectImageButton.TabIndex = 0;
            this.selectImageButton.Text = "Select Image";
            this.selectImageButton.UseVisualStyleBackColor = true;
            this.selectImageButton.Click += new EventHandler(this.SelectImageButton_Click);
            // 
            // runInferenceButton
            // 
            this.runInferenceButton.Enabled = false;
            this.runInferenceButton.Location = new Point(150, 78);
            this.runInferenceButton.Name = "runInferenceButton";
            this.runInferenceButton.Size = new Size(120, 35);
            this.runInferenceButton.TabIndex = 1;
            this.runInferenceButton.Text = "Run Inference";
            this.runInferenceButton.UseVisualStyleBackColor = true;
            this.runInferenceButton.Click += new EventHandler(this.RunInferenceButton_Click);
            // 
            // allowProviderDownloadCheckBox
            // 
            this.allowProviderDownloadCheckBox.AutoSize = true;
            this.allowProviderDownloadCheckBox.Location = new Point(290, 86);
            this.allowProviderDownloadCheckBox.Name = "allowProviderDownloadCheckBox";
            this.allowProviderDownloadCheckBox.Size = new Size(205, 19);
            this.allowProviderDownloadCheckBox.TabIndex = 7;
            this.allowProviderDownloadCheckBox.Text = "Allow execution provider downloads";
            this.allowProviderDownloadCheckBox.UseVisualStyleBackColor = true;
            // 
            // selectedImagePictureBox
            // 
            this.selectedImagePictureBox.BorderStyle = BorderStyle.FixedSingle;
            this.selectedImagePictureBox.Location = new Point(12, 171);
            this.selectedImagePictureBox.Name = "selectedImagePictureBox";
            this.selectedImagePictureBox.Size = new Size(400, 300);
            this.selectedImagePictureBox.SizeMode = PictureBoxSizeMode.Zoom;
            this.selectedImagePictureBox.TabIndex = 2;
            this.selectedImagePictureBox.TabStop = false;
            // 
            // resultsTextBox
            // 
            this.resultsTextBox.Font = new Font("Consolas", 9F);
            this.resultsTextBox.Location = new Point(430, 171);
            this.resultsTextBox.Multiline = true;
            this.resultsTextBox.Name = "resultsTextBox";
            this.resultsTextBox.ReadOnly = true;
            this.resultsTextBox.ScrollBars = ScrollBars.Vertical;
            this.resultsTextBox.Size = new Size(520, 300);
            this.resultsTextBox.TabIndex = 3;
            this.resultsTextBox.Text = "Select an image and click 'Run Inference' to see classification results.";
            // 
            // imagePathLabel
            // 
            this.imagePathLabel.AutoSize = false;
            this.imagePathLabel.Location = new Point(12, 126);
            this.imagePathLabel.Name = "imagePathLabel";
            this.imagePathLabel.Size = new Size(400, 20);
            this.imagePathLabel.TabIndex = 4;
            this.imagePathLabel.Text = "No image selected";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new Font("Segoe UI", 9F, FontStyle.Bold);
            this.label1.Location = new Point(12, 151);
            this.label1.Name = "label1";
            this.label1.Size = new Size(94, 15);
            this.label1.TabIndex = 5;
            this.label1.Text = "Selected Image:";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Font = new Font("Segoe UI", 9F, FontStyle.Bold);
            this.label2.Location = new Point(430, 151);
            this.label2.Name = "label2";
            this.label2.Size = new Size(127, 15);
            this.label2.TabIndex = 6;
            this.label2.Text = "Classification Results:";
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new SizeF(7F, 15F);
            this.AutoScaleMode = AutoScaleMode.Font;
            this.ClientSize = new Size(970, 488);
            this.Controls.Add(this.reloadSessionButton);
            this.Controls.Add(this.deviceCombo);
            this.Controls.Add(this.deviceLabel);
            this.Controls.Add(this.epCombo);
            this.Controls.Add(this.epLabel);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.imagePathLabel);
            this.Controls.Add(this.resultsTextBox);
            this.Controls.Add(this.selectedImagePictureBox);
            this.Controls.Add(this.allowProviderDownloadCheckBox);
            this.Controls.Add(this.runInferenceButton);
            this.Controls.Add(this.selectImageButton);
            this.FormBorderStyle = FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.Name = "MainForm";
            this.Text = "Windows ML WinForms Sample - SqueezeNet Image Classification";
            ((System.ComponentModel.ISupportInitialize)(this.selectedImagePictureBox)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();
        }

        #endregion

        private Button selectImageButton;
        private Button runInferenceButton;
        private CheckBox allowProviderDownloadCheckBox;
        private PictureBox selectedImagePictureBox;
        private TextBox resultsTextBox;
        private Label imagePathLabel;
        private Label label1;
        private Label label2;
        // EP selection designer fields
        private Label epLabel;
        private ComboBox epCombo;
        private Label deviceLabel;
        private ComboBox deviceCombo;
        private Button reloadSessionButton;
    }
}
