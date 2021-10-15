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
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.panel1 = new System.Windows.Forms.Panel();
            this.button1 = new System.Windows.Forms.Button();
            this.button2 = new System.Windows.Forms.Button();
            this.panel2 = new System.Windows.Forms.Panel();
            this.windowBasics1 = new winforms_unpackaged_app.WindowBasics();
            this.presenters1 = new winforms_unpackaged_app.Presenters();
            this.titleBar1 = new winforms_unpackaged_app.TitleBar();
            this.button3 = new System.Windows.Forms.Button();
            this.panel1.SuspendLayout();
            this.panel2.SuspendLayout();
            this.SuspendLayout();
            // 
            // panel1
            // 
            this.panel1.BackColor = System.Drawing.Color.White;
            this.panel1.Controls.Add(this.button1);
            this.panel1.Controls.Add(this.button2);
            this.panel1.Controls.Add(this.button3);   
            this.panel1.Dock = System.Windows.Forms.DockStyle.Left;
            this.panel1.Location = new System.Drawing.Point(0, 0);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(161, 450);
            this.panel1.TabIndex = 0;
            this.panel1.Paint += new System.Windows.Forms.PaintEventHandler(this.panel1_Paint);
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(0, 18);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(161, 29);
            this.button1.TabIndex = 0;
            this.button1.Text = "Window Basics";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // button2
            // 
            this.button2.Location = new System.Drawing.Point(0, 53);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(161, 29);
            this.button2.TabIndex = 0;
            this.button2.Text = "Presenters";
            this.button2.UseVisualStyleBackColor = true;
            this.button2.Click += new System.EventHandler(this.button2_Click);
            // 
            // panel2
            // 
            this.panel2.Controls.Add(this.windowBasics1);
            this.panel2.Controls.Add(this.titleBar1);
            this.panel2.Controls.Add(this.presenters1);            
            this.panel2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel2.Location = new System.Drawing.Point(161, 0);
            this.panel2.Name = "panel2";
            this.panel2.Size = new System.Drawing.Size(639, 450);
            this.panel2.TabIndex = 1;
            // 
            // presenters1
            // 
            this.presenters1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.presenters1.Location = new System.Drawing.Point(0, 0);
            this.presenters1.Name = "presenters1";
            this.presenters1.Size = new System.Drawing.Size(639, 450);
            this.presenters1.TabIndex = 1;
            // 
            // windowBasics1
            // 
            this.windowBasics1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.windowBasics1.Location = new System.Drawing.Point(0, 0);
            this.windowBasics1.Name = "windowBasics1";
            this.windowBasics1.Size = new System.Drawing.Size(639, 450);
            this.windowBasics1.TabIndex = 0;
            // 
            // titleBar1
            // 
            this.titleBar1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.titleBar1.ForeColor = System.Drawing.Color.Red;
            this.titleBar1.Location = new System.Drawing.Point(0, 0);
            this.titleBar1.Name = "titleBar1";
            this.titleBar1.Size = new System.Drawing.Size(639, 450);
            this.titleBar1.TabIndex = 2;
            // 
            // button3
            // 
            this.button3.Location = new System.Drawing.Point(0, 88);
            this.button3.Name = "button3";
            this.button3.Size = new System.Drawing.Size(161, 29);
            this.button3.TabIndex = 1;
            this.button3.Text = "Title Bar";
            this.button3.UseVisualStyleBackColor = true;
            this.button3.Click += new System.EventHandler(this.button3_Click);
            // 
            // AppForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 20F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(800, 450);
            this.Controls.Add(this.panel2);
            this.Controls.Add(this.panel1);
            this.Name = "AppForm";
            this.Text = "Form1";
            this.panel1.ResumeLayout(false);
            this.panel2.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.Panel panel2;
        private System.Windows.Forms.Button button2;
        private WindowBasics windowBasics1;
        private Presenters presenters1;
        private System.Windows.Forms.Button button3;
        private TitleBar titleBar1;
    }
}

