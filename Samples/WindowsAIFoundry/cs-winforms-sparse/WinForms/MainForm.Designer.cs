using System.Diagnostics;

namespace WindowsAISample
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
            selectFile = new Button();
            imagePath = new TextBox();
            SelectedImage = new PictureBox();
            processButton = new Button();
            imageTextBox = new GroupBox();
            richTextBoxForImageText = new RichTextBox();
            summaryBox = new GroupBox();
            richTextBoxForImageSummary = new RichTextBox();
            imagePanel = new Panel();
            ((System.ComponentModel.ISupportInitialize)SelectedImage).BeginInit();
            imageTextBox.SuspendLayout();
            summaryBox.SuspendLayout();
            imagePanel.SuspendLayout();
            SuspendLayout();
            // 
            // selectFile
            // 
            selectFile.Location = new Point(12, 12);
            selectFile.Name = "selectFile";
            selectFile.Size = new Size(112, 41);
            selectFile.TabIndex = 0;
            selectFile.Text = "Select File";
            selectFile.UseVisualStyleBackColor = true;
            selectFile.Click += SelectFile_Click;
            // 
            // imagePath
            // 
            imagePath.BorderStyle = BorderStyle.FixedSingle;
            imagePath.Location = new Point(146, 18);
            imagePath.Name = "imagePath";
            imagePath.PlaceholderText = "Image Path";
            imagePath.Size = new Size(948, 31);
            imagePath.TabIndex = 1;
            // 
            // SelectedImage
            // 
            SelectedImage.Location = new Point(0, 3);
            SelectedImage.Name = "SelectedImage";
            SelectedImage.Size = new Size(1304, 367);
            SelectedImage.SizeMode = PictureBoxSizeMode.AutoSize;
            SelectedImage.TabIndex = 2;
            SelectedImage.TabStop = false;
            // 
            // processButton
            // 
            processButton.Location = new Point(1110, 12);
            processButton.Name = "processButton";
            processButton.Size = new Size(150, 41);
            processButton.TabIndex = 3;
            processButton.Text = "Process Image";
            processButton.UseVisualStyleBackColor = true;
            processButton.Click += ProcessButton_Click;
            // 
            // imageTextBox
            // 
            imageTextBox.Controls.Add(richTextBoxForImageText);
            imageTextBox.Location = new Point(12, 438);
            imageTextBox.Name = "imageTextBox";
            imageTextBox.Size = new Size(632, 239);
            imageTextBox.TabIndex = 4;
            imageTextBox.TabStop = false;
            imageTextBox.Text = "Image Text";
            // 
            // richTextBoxForImageText
            // 
            richTextBoxForImageText.Location = new Point(6, 30);
            richTextBoxForImageText.Name = "richTextBoxForImageText";
            richTextBoxForImageText.Size = new Size(620, 200);
            richTextBoxForImageText.TabIndex = 0;
            richTextBoxForImageText.Text = "";
            // 
            // summaryBox
            // 
            summaryBox.Controls.Add(richTextBoxForImageSummary);
            summaryBox.Location = new Point(697, 438);
            summaryBox.Name = "summaryBox";
            summaryBox.Size = new Size(634, 239);
            summaryBox.TabIndex = 5;
            summaryBox.TabStop = false;
            summaryBox.Text = "Summary";
            // 
            // richTextBoxForImageSummary
            // 
            richTextBoxForImageSummary.Location = new Point(6, 30);
            richTextBoxForImageSummary.Name = "richTextBoxForImageSummary";
            richTextBoxForImageSummary.Size = new Size(622, 200);
            richTextBoxForImageSummary.TabIndex = 0;
            richTextBoxForImageSummary.Text = "";
            // 
            // imagePanel
            // 
            imagePanel.AutoScroll = true;
            imagePanel.Controls.Add(SelectedImage);
            imagePanel.Location = new Point(18, 59);
            imagePanel.Name = "imagePanel";
            imagePanel.Size = new Size(1307, 373);
            imagePanel.TabIndex = 6;
            // 
            // MainForm
            // 
            AutoScaleDimensions = new SizeF(10F, 25F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(1343, 692);
            Controls.Add(imagePanel);
            Controls.Add(summaryBox);
            Controls.Add(imageTextBox);
            Controls.Add(processButton);
            Controls.Add(imagePath);
            Controls.Add(selectFile);
            Name = "MainForm";
            Text = "Windows AI Sample WinForm App";
            ((System.ComponentModel.ISupportInitialize)SelectedImage).EndInit();
            imageTextBox.ResumeLayout(false);
            summaryBox.ResumeLayout(false);
            imagePanel.ResumeLayout(false);
            imagePanel.PerformLayout();
            ResumeLayout(false);
            PerformLayout();
        }

        #endregion

        private Button selectFile;
        private TextBox imagePath;
        private PictureBox SelectedImage;
        private Button processButton;
        private GroupBox imageTextBox;
        private GroupBox summaryBox;
        private RichTextBox richTextBoxForImageText;
        private RichTextBox richTextBoxForImageSummary;
        private Panel imagePanel;
    }
}
