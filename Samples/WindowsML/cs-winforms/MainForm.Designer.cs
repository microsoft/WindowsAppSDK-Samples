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
            this.selectedImagePictureBox = new PictureBox();
            this.resultsTextBox = new TextBox();
            this.imagePathLabel = new Label();
            this.label1 = new Label();
            this.label2 = new Label();
            ((System.ComponentModel.ISupportInitialize)(this.selectedImagePictureBox)).BeginInit();
            this.SuspendLayout();
            // 
            // selectImageButton
            // 
            this.selectImageButton.Location = new Point(12, 12);
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
            this.runInferenceButton.Location = new Point(150, 12);
            this.runInferenceButton.Name = "runInferenceButton";
            this.runInferenceButton.Size = new Size(120, 35);
            this.runInferenceButton.TabIndex = 1;
            this.runInferenceButton.Text = "Run Inference";
            this.runInferenceButton.UseVisualStyleBackColor = true;
            this.runInferenceButton.Click += new EventHandler(this.RunInferenceButton_Click);
            // 
            // selectedImagePictureBox
            // 
            this.selectedImagePictureBox.BorderStyle = BorderStyle.FixedSingle;
            this.selectedImagePictureBox.Location = new Point(12, 105);
            this.selectedImagePictureBox.Name = "selectedImagePictureBox";
            this.selectedImagePictureBox.Size = new Size(400, 300);
            this.selectedImagePictureBox.SizeMode = PictureBoxSizeMode.Zoom;
            this.selectedImagePictureBox.TabIndex = 2;
            this.selectedImagePictureBox.TabStop = false;
            // 
            // resultsTextBox
            // 
            this.resultsTextBox.Font = new Font("Consolas", 9F);
            this.resultsTextBox.Location = new Point(430, 105);
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
            this.imagePathLabel.Location = new Point(12, 60);
            this.imagePathLabel.Name = "imagePathLabel";
            this.imagePathLabel.Size = new Size(400, 20);
            this.imagePathLabel.TabIndex = 4;
            this.imagePathLabel.Text = "No image selected";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new Font("Segoe UI", 9F, FontStyle.Bold);
            this.label1.Location = new Point(12, 85);
            this.label1.Name = "label1";
            this.label1.Size = new Size(94, 15);
            this.label1.TabIndex = 5;
            this.label1.Text = "Selected Image:";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Font = new Font("Segoe UI", 9F, FontStyle.Bold);
            this.label2.Location = new Point(430, 85);
            this.label2.Name = "label2";
            this.label2.Size = new Size(127, 15);
            this.label2.TabIndex = 6;
            this.label2.Text = "Classification Results:";
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new SizeF(7F, 15F);
            this.AutoScaleMode = AutoScaleMode.Font;
            this.ClientSize = new Size(970, 425);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.imagePathLabel);
            this.Controls.Add(this.resultsTextBox);
            this.Controls.Add(this.selectedImagePictureBox);
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
        private PictureBox selectedImagePictureBox;
        private TextBox resultsTextBox;
        private Label imagePathLabel;
        private Label label1;
        private Label label2;
    }
}
