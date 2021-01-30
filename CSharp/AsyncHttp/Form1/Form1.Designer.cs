
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
            this.panel3 = new System.Windows.Forms.Panel();
            this.lblURL = new System.Windows.Forms.Label();
            this.edtURL = new System.Windows.Forms.TextBox();
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.panel2 = new System.Windows.Forms.Panel();
            this.ckbEagerReport = new System.Windows.Forms.CheckBox();
            this.edtStressCycles = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.btnStress = new System.Windows.Forms.Button();
            this.btnCancel = new System.Windows.Forms.Button();
            this.btnStart = new System.Windows.Forms.Button();
            this.ckbReportMemUsage = new System.Windows.Forms.CheckBox();
            this.btnReportMemNow = new System.Windows.Forms.Button();
            this.panel1.SuspendLayout();
            this.panel3.SuspendLayout();
            this.panel2.SuspendLayout();
            this.SuspendLayout();
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.panel3);
            this.panel1.Controls.Add(this.panel2);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel1.Location = new System.Drawing.Point(0, 0);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(800, 488);
            this.panel1.TabIndex = 0;
            // 
            // panel3
            // 
            this.panel3.Controls.Add(this.lblURL);
            this.panel3.Controls.Add(this.edtURL);
            this.panel3.Controls.Add(this.textBox1);
            this.panel3.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel3.Location = new System.Drawing.Point(140, 0);
            this.panel3.Name = "panel3";
            this.panel3.Size = new System.Drawing.Size(660, 488);
            this.panel3.TabIndex = 1;
            // 
            // lblURL
            // 
            this.lblURL.AutoSize = true;
            this.lblURL.Font = new System.Drawing.Font("Tahoma", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblURL.Location = new System.Drawing.Point(3, 3);
            this.lblURL.Name = "lblURL";
            this.lblURL.Size = new System.Drawing.Size(32, 14);
            this.lblURL.TabIndex = 2;
            this.lblURL.Text = "UR&L:";
            // 
            // edtURL
            // 
            this.edtURL.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.edtURL.Font = new System.Drawing.Font("Tahoma", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.edtURL.Location = new System.Drawing.Point(44, 0);
            this.edtURL.Name = "edtURL";
            this.edtURL.Size = new System.Drawing.Size(616, 22);
            this.edtURL.TabIndex = 1;
            // 
            // textBox1
            // 
            this.textBox1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.textBox1.BackColor = System.Drawing.SystemColors.Window;
            this.textBox1.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.textBox1.Location = new System.Drawing.Point(0, 20);
            this.textBox1.Multiline = true;
            this.textBox1.Name = "textBox1";
            this.textBox1.ReadOnly = true;
            this.textBox1.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.textBox1.Size = new System.Drawing.Size(660, 468);
            this.textBox1.TabIndex = 0;
            // 
            // panel2
            // 
            this.panel2.Controls.Add(this.btnReportMemNow);
            this.panel2.Controls.Add(this.ckbReportMemUsage);
            this.panel2.Controls.Add(this.ckbEagerReport);
            this.panel2.Controls.Add(this.edtStressCycles);
            this.panel2.Controls.Add(this.label1);
            this.panel2.Controls.Add(this.btnStress);
            this.panel2.Controls.Add(this.btnCancel);
            this.panel2.Controls.Add(this.btnStart);
            this.panel2.Dock = System.Windows.Forms.DockStyle.Left;
            this.panel2.Location = new System.Drawing.Point(0, 0);
            this.panel2.Name = "panel2";
            this.panel2.Size = new System.Drawing.Size(140, 488);
            this.panel2.TabIndex = 0;
            // 
            // ckbEagerReport
            // 
            this.ckbEagerReport.AutoSize = true;
            this.ckbEagerReport.Location = new System.Drawing.Point(16, 144);
            this.ckbEagerReport.Name = "ckbEagerReport";
            this.ckbEagerReport.Size = new System.Drawing.Size(84, 17);
            this.ckbEagerReport.TabIndex = 5;
            this.ckbEagerReport.Text = "&Eager report";
            this.ckbEagerReport.UseVisualStyleBackColor = true;
            // 
            // edtStressCycles
            // 
            this.edtStressCycles.Location = new System.Drawing.Point(59, 124);
            this.edtStressCycles.Name = "edtStressCycles";
            this.edtStressCycles.Size = new System.Drawing.Size(75, 20);
            this.edtStressCycles.TabIndex = 4;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(13, 127);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(40, 13);
            this.label1.TabIndex = 3;
            this.label1.Text = "cycles:";
            // 
            // btnStress
            // 
            this.btnStress.Location = new System.Drawing.Point(12, 95);
            this.btnStress.Name = "btnStress";
            this.btnStress.Size = new System.Drawing.Size(122, 25);
            this.btnStress.TabIndex = 2;
            this.btnStress.Text = "S&tress HTTP";
            this.btnStress.UseVisualStyleBackColor = true;
            this.btnStress.Click += new System.EventHandler(this.btnStress_Click);
            // 
            // btnCancel
            // 
            this.btnCancel.Location = new System.Drawing.Point(31, 43);
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Size = new System.Drawing.Size(75, 25);
            this.btnCancel.TabIndex = 1;
            this.btnCancel.Text = "&Cancel";
            this.btnCancel.UseVisualStyleBackColor = true;
            this.btnCancel.Click += new System.EventHandler(this.btnCancel_Click);
            // 
            // btnStart
            // 
            this.btnStart.Location = new System.Drawing.Point(31, 12);
            this.btnStart.Name = "btnStart";
            this.btnStart.Size = new System.Drawing.Size(75, 25);
            this.btnStart.TabIndex = 0;
            this.btnStart.Text = "&Start HTTP";
            this.btnStart.UseVisualStyleBackColor = true;
            this.btnStart.Click += new System.EventHandler(this.btnStart_Click);
            // 
            // ckbReportMemUsage
            // 
            this.ckbReportMemUsage.AutoSize = true;
            this.ckbReportMemUsage.Location = new System.Drawing.Point(16, 168);
            this.ckbReportMemUsage.Name = "ckbReportMemUsage";
            this.ckbReportMemUsage.Size = new System.Drawing.Size(115, 17);
            this.ckbReportMemUsage.TabIndex = 6;
            this.ckbReportMemUsage.Text = "Report mem usage";
            this.ckbReportMemUsage.UseVisualStyleBackColor = true;
            // 
            // btnReportMemNow
            // 
            this.btnReportMemNow.Location = new System.Drawing.Point(12, 453);
            this.btnReportMemNow.Name = "btnReportMemNow";
            this.btnReportMemNow.Size = new System.Drawing.Size(119, 23);
            this.btnReportMemNow.TabIndex = 7;
            this.btnReportMemNow.Text = "Report Mem now";
            this.btnReportMemNow.UseVisualStyleBackColor = true;
            this.btnReportMemNow.Click += new System.EventHandler(this.btnReportMemNow_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(800, 488);
            this.Controls.Add(this.panel1);
            this.Name = "Form1";
            this.Text = "AsyncHttp demo";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.panel1.ResumeLayout(false);
            this.panel3.ResumeLayout(false);
            this.panel3.PerformLayout();
            this.panel2.ResumeLayout(false);
            this.panel2.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Panel panel3;
        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.Panel panel2;
        private System.Windows.Forms.Button btnStart;
        private System.Windows.Forms.Button btnCancel;
        private System.Windows.Forms.Button btnStress;
        private System.Windows.Forms.Label lblURL;
        private System.Windows.Forms.TextBox edtURL;
        private System.Windows.Forms.TextBox edtStressCycles;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.CheckBox ckbEagerReport;
        private System.Windows.Forms.CheckBox ckbReportMemUsage;
        private System.Windows.Forms.Button btnReportMemNow;
    }
}

