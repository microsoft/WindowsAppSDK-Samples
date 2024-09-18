namespace WinFormsWithIslandApp
{
    partial class Form1
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
            panel1 = new Panel();
            desktopWindowXamlSourceControl1 = new WinFormsWithIsland.DesktopWindowXamlSourceControl();
            winFormsButton2 = new Button();
            winFormsButton1 = new Button();
            panel1.SuspendLayout();
            SuspendLayout();
            // 
            // panel1
            // 
            panel1.Controls.Add(desktopWindowXamlSourceControl1);
            panel1.Controls.Add(winFormsButton2);
            panel1.Controls.Add(winFormsButton1);
            panel1.Dock = DockStyle.Fill;
            panel1.Location = new Point(0, 0);
            panel1.Name = "panel1";
            panel1.Padding = new Padding(20);
            panel1.Size = new Size(755, 445);
            panel1.TabIndex = 0;
            // 
            // desktopWindowXamlSourceControl1
            // 
            desktopWindowXamlSourceControl1.Dock = DockStyle.Fill;
            desktopWindowXamlSourceControl1.Location = new Point(20, 80);
            desktopWindowXamlSourceControl1.Name = "desktopWindowXamlSourceControl1";
            desktopWindowXamlSourceControl1.Size = new Size(715, 285);
            desktopWindowXamlSourceControl1.TabIndex = 1;
            desktopWindowXamlSourceControl1.Text = "desktopWindowXamlSourceControl1";
            // 
            // winFormsButton2
            // 
            winFormsButton2.Dock = DockStyle.Bottom;
            winFormsButton2.Location = new Point(20, 365);
            winFormsButton2.Name = "winFormsButton2";
            winFormsButton2.Size = new Size(715, 60);
            winFormsButton2.TabIndex = 2;
            winFormsButton2.Text = "WinForms Button 2";
            winFormsButton2.UseVisualStyleBackColor = true;
            // 
            // winFormsButton1
            // 
            winFormsButton1.Dock = DockStyle.Top;
            winFormsButton1.Location = new Point(20, 20);
            winFormsButton1.Margin = new Padding(0);
            winFormsButton1.Name = "winFormsButton1";
            winFormsButton1.Size = new Size(715, 60);
            winFormsButton1.TabIndex = 0;
            winFormsButton1.Text = "WinForms Button 1";
            winFormsButton1.UseVisualStyleBackColor = true;
            // 
            // Form1
            // 
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(755, 445);
            Controls.Add(panel1);
            Name = "Form1";
            Text = "Form1";
            panel1.ResumeLayout(false);
            ResumeLayout(false);
        }

        #endregion

        private Panel panel1;
        private WinFormsWithIsland.DesktopWindowXamlSourceControl desktopWindowXamlSourceControl1;
        private Button winFormsButton2;
        private Button winFormsButton1;
    }
}
