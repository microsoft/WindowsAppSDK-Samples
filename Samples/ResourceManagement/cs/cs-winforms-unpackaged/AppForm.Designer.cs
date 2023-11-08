// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.

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

        /// <summary>
        ///  Required method for Designer support - do not modify
        ///  the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.SampleStringTextBox = new System.Windows.Forms.TextBox();
            this.DefaultLanguageButton = new System.Windows.Forms.Button();
            this.OverrideLanguageButton = new System.Windows.Forms.Button();
            this.ResourceFallbackButton = new System.Windows.Forms.Button();
            this.DefaultLanguageLabel = new System.Windows.Forms.Label();
            this.OverrideLanguageLabel = new System.Windows.Forms.Label();
            this.ResourceFallbackLabel = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // SampleStringTextBox
            // 
            this.SampleStringTextBox.Location = new System.Drawing.Point(264, 354);
            this.SampleStringTextBox.Name = "SampleStringTextBox";
            this.SampleStringTextBox.Size = new System.Drawing.Size(273, 23);
            this.SampleStringTextBox.TabIndex = 0;            
            // 
            // DefaultLanguageButton
            // 
            this.DefaultLanguageButton.Location = new System.Drawing.Point(264, 41);
            this.DefaultLanguageButton.Name = "DefaultLanguageButton";
            this.DefaultLanguageButton.Size = new System.Drawing.Size(275, 40);
            this.DefaultLanguageButton.TabIndex = 1;
            this.DefaultLanguageButton.Text = "Default Language";
            this.DefaultLanguageButton.UseVisualStyleBackColor = true;
            this.DefaultLanguageButton.Click += new System.EventHandler(this.DefaultLanguageButton_Click);
            // 
            // OverrideLanguageButton
            // 
            this.OverrideLanguageButton.Location = new System.Drawing.Point(264, 129);
            this.OverrideLanguageButton.Name = "OverrideLanguageButton";
            this.OverrideLanguageButton.Size = new System.Drawing.Size(275, 40);
            this.OverrideLanguageButton.TabIndex = 2;
            this.OverrideLanguageButton.Text = "Override Language";
            this.OverrideLanguageButton.UseVisualStyleBackColor = true;
            this.OverrideLanguageButton.Click += new System.EventHandler(this.OverrideLanguageButton_Click);
            // 
            // ResourceFallbackButton
            // 
            this.ResourceFallbackButton.Location = new System.Drawing.Point(263, 217);
            this.ResourceFallbackButton.Name = "ResourceFallbackButton";
            this.ResourceFallbackButton.Size = new System.Drawing.Size(275, 40);
            this.ResourceFallbackButton.TabIndex = 3;
            this.ResourceFallbackButton.Text = "Resource Fallback";
            this.ResourceFallbackButton.UseVisualStyleBackColor = true;
            this.ResourceFallbackButton.Click += new System.EventHandler(this.ResourceFallbackButton_Click);
            // 
            // DefaultLanguageLabel
            // 
            this.DefaultLanguageLabel.AutoSize = true;
            this.DefaultLanguageLabel.Location = new System.Drawing.Point(171, 87);
            this.DefaultLanguageLabel.Name = "DefaultLanguageLabel";
            this.DefaultLanguageLabel.Size = new System.Drawing.Size(506, 15);
            this.DefaultLanguageLabel.TabIndex = 4;
            this.DefaultLanguageLabel.Text = "This uses the ResourceLoader.GetString API to find a string resource using the de" +
                "vice language.";
            this.DefaultLanguageLabel.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // OverrideLanguageLabel
            // 
            this.OverrideLanguageLabel.AutoSize = true;
            this.OverrideLanguageLabel.Location = new System.Drawing.Point(54, 172);
            this.OverrideLanguageLabel.Name = "OverrideLanguageLabel";
            this.OverrideLanguageLabel.Size = new System.Drawing.Size(702, 15);
            this.OverrideLanguageLabel.TabIndex = 5;
            this.OverrideLanguageLabel.Text = "This uses the ResourceManager.MainResourceMap.GetValue API with a custom resource" +
                " context to find a string resource in German.";
            this.OverrideLanguageLabel.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // ResourceFallbackLabel
            // 
            this.ResourceFallbackLabel.AutoSize = true;
            this.ResourceFallbackLabel.Location = new System.Drawing.Point(114, 259);
            this.ResourceFallbackLabel.Name = "ResourceFallbackLabel";
            this.ResourceFallbackLabel.Size = new System.Drawing.Size(550, 30);
            this.ResourceFallbackLabel.TabIndex = 6;
            this.ResourceFallbackLabel.Text = "This sets the ResourceNotFound callback, which is invoked if the resource is not " +
                "found using MRT Core.\r\n It enables transparently finding the resource using a di" +
                "fferent technology.";
            this.ResourceFallbackLabel.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // AppForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(800, 450);
            this.Controls.Add(this.ResourceFallbackLabel);
            this.Controls.Add(this.OverrideLanguageLabel);
            this.Controls.Add(this.DefaultLanguageLabel);
            this.Controls.Add(this.ResourceFallbackButton);
            this.Controls.Add(this.OverrideLanguageButton);
            this.Controls.Add(this.DefaultLanguageButton);
            this.Controls.Add(this.SampleStringTextBox);
            this.Name = "AppForm";
            this.Text = "MRT Core C# unpackaged sample";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox SampleStringTextBox;
        private System.Windows.Forms.Button DefaultLanguageButton;
        private System.Windows.Forms.Button OverrideLanguageButton;
        private System.Windows.Forms.Button ResourceFallbackButton;
        private System.Windows.Forms.Label DefaultLanguageLabel;
        private System.Windows.Forms.Label OverrideLanguageLabel;
        private System.Windows.Forms.Label ResourceFallbackLabel;
    }
}

