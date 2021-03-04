
namespace prjSkeleton
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
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
            this.panel3Right = new System.Windows.Forms.Panel();
            this.btn1 = new System.Windows.Forms.Button();
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.wb1 = new System.Windows.Forms.WebBrowser();
            this.btnClear = new System.Windows.Forms.Button();
            this.tboxStatus = new System.Windows.Forms.TextBox();
            this.tboxLog = new System.Windows.Forms.TextBox();
            this.lblURL = new System.Windows.Forms.Label();
            this.cbxURL = new System.Windows.Forms.ComboBox();
            this.panel2Left = new System.Windows.Forms.FlowLayoutPanel();
            this.checkBox1 = new System.Windows.Forms.CheckBox();
            this.panel1.SuspendLayout();
            this.panel3Right.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.panel2Left.SuspendLayout();
            this.SuspendLayout();
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.panel3Right);
            this.panel1.Controls.Add(this.panel2Left);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel1.Location = new System.Drawing.Point(0, 0);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(905, 557);
            this.panel1.TabIndex = 0;
            // 
            // panel3Right
            // 
            this.panel3Right.Controls.Add(this.btn1);
            this.panel3Right.Controls.Add(this.splitContainer1);
            this.panel3Right.Controls.Add(this.lblURL);
            this.panel3Right.Controls.Add(this.cbxURL);
            this.panel3Right.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel3Right.Location = new System.Drawing.Point(140, 0);
            this.panel3Right.Name = "panel3Right";
            this.panel3Right.Size = new System.Drawing.Size(765, 557);
            this.panel3Right.TabIndex = 1;
            // 
            // btn1
            // 
            this.btn1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btn1.Location = new System.Drawing.Point(702, 0);
            this.btn1.Name = "btn1";
            this.btn1.Size = new System.Drawing.Size(63, 22);
            this.btn1.TabIndex = 1;
            this.btn1.Text = "&Navigate";
            this.btn1.UseVisualStyleBackColor = true;
            this.btn1.Click += new System.EventHandler(this.btnNavigate_Click);
            // 
            // splitContainer1
            // 
            this.splitContainer1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.splitContainer1.Location = new System.Drawing.Point(3, 22);
            this.splitContainer1.Name = "splitContainer1";
            this.splitContainer1.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.splitContainer1.Panel1.Controls.Add(this.wb1);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(192)))), ((int)(((byte)(0)))), ((int)(((byte)(0)))));
            this.splitContainer1.Panel2.Controls.Add(this.btnClear);
            this.splitContainer1.Panel2.Controls.Add(this.tboxStatus);
            this.splitContainer1.Panel2.Controls.Add(this.tboxLog);
            this.splitContainer1.Size = new System.Drawing.Size(762, 530);
            this.splitContainer1.SplitterDistance = 406;
            this.splitContainer1.TabIndex = 3;
            // 
            // wb1
            // 
            this.wb1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.wb1.Location = new System.Drawing.Point(0, 0);
            this.wb1.MinimumSize = new System.Drawing.Size(20, 20);
            this.wb1.Name = "wb1";
            this.wb1.Size = new System.Drawing.Size(762, 406);
            this.wb1.TabIndex = 0;
            // 
            // btnClear
            // 
            this.btnClear.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnClear.BackColor = System.Drawing.SystemColors.ButtonFace;
            this.btnClear.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.btnClear.Location = new System.Drawing.Point(695, 100);
            this.btnClear.Name = "btnClear";
            this.btnClear.Size = new System.Drawing.Size(67, 20);
            this.btnClear.TabIndex = 2;
            this.btnClear.Text = "&Clear";
            this.btnClear.UseVisualStyleBackColor = false;
            // 
            // tboxStatus
            // 
            this.tboxStatus.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tboxStatus.Location = new System.Drawing.Point(0, 100);
            this.tboxStatus.Name = "tboxStatus";
            this.tboxStatus.ReadOnly = true;
            this.tboxStatus.Size = new System.Drawing.Size(695, 20);
            this.tboxStatus.TabIndex = 1;
            // 
            // tboxLog
            // 
            this.tboxLog.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tboxLog.BackColor = System.Drawing.SystemColors.Window;
            this.tboxLog.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.tboxLog.Location = new System.Drawing.Point(0, 0);
            this.tboxLog.Multiline = true;
            this.tboxLog.Name = "tboxLog";
            this.tboxLog.ReadOnly = true;
            this.tboxLog.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.tboxLog.Size = new System.Drawing.Size(762, 100);
            this.tboxLog.TabIndex = 0;
            // 
            // lblURL
            // 
            this.lblURL.AutoSize = true;
            this.lblURL.Location = new System.Drawing.Point(3, 3);
            this.lblURL.Name = "lblURL";
            this.lblURL.Size = new System.Drawing.Size(32, 13);
            this.lblURL.TabIndex = 2;
            this.lblURL.Text = "UR&L:";
            // 
            // cbxURL
            // 
            this.cbxURL.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.cbxURL.Font = new System.Drawing.Font("Tahoma", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.cbxURL.FormattingEnabled = true;
            this.cbxURL.Location = new System.Drawing.Point(52, 0);
            this.cbxURL.Name = "cbxURL";
            this.cbxURL.Size = new System.Drawing.Size(646, 22);
            this.cbxURL.TabIndex = 0;
            // 
            // panel2Left
            // 
            this.panel2Left.Controls.Add(this.checkBox1);
            this.panel2Left.Dock = System.Windows.Forms.DockStyle.Left;
            this.panel2Left.Location = new System.Drawing.Point(0, 0);
            this.panel2Left.Name = "panel2Left";
            this.panel2Left.Size = new System.Drawing.Size(140, 557);
            this.panel2Left.TabIndex = 0;
            // 
            // checkBox1
            // 
            this.checkBox1.AutoSize = true;
            this.checkBox1.Location = new System.Drawing.Point(3, 3);
            this.checkBox1.Name = "checkBox1";
            this.checkBox1.Size = new System.Drawing.Size(80, 17);
            this.checkBox1.TabIndex = 1;
            this.checkBox1.Text = "checkBox1";
            this.checkBox1.UseVisualStyleBackColor = true;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(905, 557);
            this.Controls.Add(this.panel1);
            this.Name = "Form1";
            this.Text = "csWebBrowser1";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.panel1.ResumeLayout(false);
            this.panel3Right.ResumeLayout(false);
            this.panel3Right.PerformLayout();
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel2.ResumeLayout(false);
            this.splitContainer1.Panel2.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
            this.splitContainer1.ResumeLayout(false);
            this.panel2Left.ResumeLayout(false);
            this.panel2Left.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Panel panel3Right;
        private System.Windows.Forms.TextBox tboxLog;
        private System.Windows.Forms.FlowLayoutPanel panel2Left;
        private System.Windows.Forms.Button btn1;
        private System.Windows.Forms.ComboBox cbxURL;
        private System.Windows.Forms.Label lblURL;
        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.CheckBox checkBox1;
        private System.Windows.Forms.TextBox tboxStatus;
        private System.Windows.Forms.WebBrowser wb1;
        private System.Windows.Forms.Button btnClear;
    }
}

