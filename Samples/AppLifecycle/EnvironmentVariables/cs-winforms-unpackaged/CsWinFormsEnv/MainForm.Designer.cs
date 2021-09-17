// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

namespace CsWinFormsEnv
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
            this.toolStrip1 = new System.Windows.Forms.ToolStrip();
            this.GetVarsButton = new System.Windows.Forms.ToolStripButton();
            this.SetVarsButton = new System.Windows.Forms.ToolStripButton();
            this.listView1 = new System.Windows.Forms.ListView();
            this.PathButton = new System.Windows.Forms.ToolStripButton();
            this.PathExtButton = new System.Windows.Forms.ToolStripButton();
            this.toolStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // toolStrip1
            // 
            this.toolStrip1.ImageScalingSize = new System.Drawing.Size(20, 20);
            this.toolStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.GetVarsButton,
            this.SetVarsButton,
            this.PathButton,
            this.PathExtButton});
            this.toolStrip1.Location = new System.Drawing.Point(0, 0);
            this.toolStrip1.Name = "toolStrip1";
            this.toolStrip1.Size = new System.Drawing.Size(598, 27);
            this.toolStrip1.TabIndex = 0;
            this.toolStrip1.Text = "toolStrip1";
            // 
            // GetVarsButton
            // 
            this.GetVarsButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.GetVarsButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.GetVarsButton.Name = "GetVarsButton";
            this.GetVarsButton.Size = new System.Drawing.Size(63, 24);
            this.GetVarsButton.Text = "GetVars";
            this.GetVarsButton.Click += new System.EventHandler(this.GetVarsButton_Click);
            // 
            // SetVarsButton
            // 
            this.SetVarsButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.SetVarsButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.SetVarsButton.Name = "SetVarsButton";
            this.SetVarsButton.Size = new System.Drawing.Size(61, 24);
            this.SetVarsButton.Text = "SetVars";
            this.SetVarsButton.Click += new System.EventHandler(this.SetVarsButton_Click);
            // 
            // listView1
            // 
            this.listView1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.listView1.HideSelection = false;
            this.listView1.Location = new System.Drawing.Point(0, 27);
            this.listView1.Name = "listView1";
            this.listView1.Size = new System.Drawing.Size(598, 423);
            this.listView1.TabIndex = 1;
            this.listView1.UseCompatibleStateImageBehavior = false;
            this.listView1.View = System.Windows.Forms.View.List;
            // 
            // PathButton
            // 
            this.PathButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.PathButton.Image = ((System.Drawing.Image)(resources.GetObject("PathButton.Image")));
            this.PathButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.PathButton.Name = "PathButton";
            this.PathButton.Size = new System.Drawing.Size(41, 24);
            this.PathButton.Text = "Path";
            this.PathButton.Click += new System.EventHandler(this.PathButton_Click);
            // 
            // PathExtButton
            // 
            this.PathExtButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.PathExtButton.Image = ((System.Drawing.Image)(resources.GetObject("PathExtButton.Image")));
            this.PathExtButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.PathExtButton.Name = "PathExtButton";
            this.PathExtButton.Size = new System.Drawing.Size(61, 24);
            this.PathExtButton.Text = "PathExt";
            this.PathExtButton.Click += new System.EventHandler(this.PathExtButton_Click);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 20F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(598, 450);
            this.Controls.Add(this.listView1);
            this.Controls.Add(this.toolStrip1);
            this.Name = "MainForm";
            this.Text = "CsWinFormsEnv";
            this.toolStrip1.ResumeLayout(false);
            this.toolStrip1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ToolStrip toolStrip1;
        private System.Windows.Forms.ToolStripButton GetVarsButton;
        private System.Windows.Forms.ToolStripButton SetVarsButton;
        private System.Windows.Forms.ListView listView1;
        private System.Windows.Forms.ToolStripButton PathButton;
        private System.Windows.Forms.ToolStripButton PathExtButton;
    }
}

