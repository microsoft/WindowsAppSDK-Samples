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
            this.AddVarsButton = new System.Windows.Forms.ToolStripButton();
            this.DelVarsButton = new System.Windows.Forms.ToolStripButton();
            this.AddPathButton = new System.Windows.Forms.ToolStripButton();
            this.DelPathButton = new System.Windows.Forms.ToolStripButton();
            this.AddPathExtButton = new System.Windows.Forms.ToolStripButton();
            this.DelPathExtButton = new System.Windows.Forms.ToolStripButton();
            this.listView1 = new System.Windows.Forms.ListView();
            this.toolStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // toolStrip1
            // 
            this.toolStrip1.ImageScalingSize = new System.Drawing.Size(20, 20);
            this.toolStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.GetVarsButton,
            this.AddVarsButton,
            this.DelVarsButton,
            this.AddPathButton,
            this.DelPathButton,
            this.AddPathExtButton,
            this.DelPathExtButton});
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
            // AddVarsButton
            // 
            this.AddVarsButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.AddVarsButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.AddVarsButton.Name = "AddVarsButton";
            this.AddVarsButton.Size = new System.Drawing.Size(68, 24);
            this.AddVarsButton.Text = "AddVars";
            this.AddVarsButton.Click += new System.EventHandler(this.AddVarsButton_Click);
            // 
            // DelVarsButton
            // 
            this.DelVarsButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.DelVarsButton.Image = ((System.Drawing.Image)(resources.GetObject("DelVarsButton.Image")));
            this.DelVarsButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.DelVarsButton.Name = "DelVarsButton";
            this.DelVarsButton.Size = new System.Drawing.Size(63, 24);
            this.DelVarsButton.Text = "DelVars";
            this.DelVarsButton.Click += new System.EventHandler(this.DelVarsButton_Click);
            // 
            // AddPathButton
            // 
            this.AddPathButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.AddPathButton.Image = ((System.Drawing.Image)(resources.GetObject("AddPathButton.Image")));
            this.AddPathButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.AddPathButton.Name = "AddPathButton";
            this.AddPathButton.Size = new System.Drawing.Size(69, 24);
            this.AddPathButton.Text = "AddPath";
            this.AddPathButton.Click += new System.EventHandler(this.AddPathButton_Click);
            // 
            // DelPathButton
            // 
            this.DelPathButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.DelPathButton.Image = ((System.Drawing.Image)(resources.GetObject("DelPathButton.Image")));
            this.DelPathButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.DelPathButton.Name = "DelPathButton";
            this.DelPathButton.Size = new System.Drawing.Size(64, 24);
            this.DelPathButton.Text = "DelPath";
            this.DelPathButton.Click += new System.EventHandler(this.DelPathButton_Click);
            // 
            // AddPathExtButton
            // 
            this.AddPathExtButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.AddPathExtButton.Image = ((System.Drawing.Image)(resources.GetObject("AddPathExtButton.Image")));
            this.AddPathExtButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.AddPathExtButton.Name = "AddPathExtButton";
            this.AddPathExtButton.Size = new System.Drawing.Size(89, 24);
            this.AddPathExtButton.Text = "AddPathExt";
            this.AddPathExtButton.Click += new System.EventHandler(this.AddPathExtButton_Click);
            // 
            // DelPathExtButton
            // 
            this.DelPathExtButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.DelPathExtButton.Image = ((System.Drawing.Image)(resources.GetObject("DelPathExtButton.Image")));
            this.DelPathExtButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.DelPathExtButton.Name = "DelPathExtButton";
            this.DelPathExtButton.Size = new System.Drawing.Size(84, 24);
            this.DelPathExtButton.Text = "DelPathExt";
            this.DelPathExtButton.Click += new System.EventHandler(this.DelPathExtButton_Click);
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
        private System.Windows.Forms.ToolStripButton AddVarsButton;
        private System.Windows.Forms.ListView listView1;
        private System.Windows.Forms.ToolStripButton AddPathButton;
        private System.Windows.Forms.ToolStripButton AddPathExtButton;
        private System.Windows.Forms.ToolStripButton DelVarsButton;
        private System.Windows.Forms.ToolStripButton DelPathButton;
        private System.Windows.Forms.ToolStripButton DelPathExtButton;
    }
}

