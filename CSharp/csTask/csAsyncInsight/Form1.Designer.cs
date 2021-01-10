
namespace csAsyncInsight
{
    partial class Form1
    {
        /// <summary>
        /// 必需的设计器变量。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 清理所有正在使用的资源。
        /// </summary>
        /// <param name="disposing">如果应释放托管资源，为 true；否则为 false。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows 窗体设计器生成的代码

        /// <summary>
        /// 设计器支持所需的方法 - 不要修改
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            this.panel1 = new System.Windows.Forms.Panel();
            this.panel3 = new System.Windows.Forms.Panel();
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.panel2 = new System.Windows.Forms.Panel();
            this.edtUIThreadMillis = new System.Windows.Forms.TextBox();
            this.ckbMainUISleep = new System.Windows.Forms.CheckBox();
            this.lblRunParam = new System.Windows.Forms.TextBox();
            this.btnClearText = new System.Windows.Forms.Button();
            this.ckbAppendText = new System.Windows.Forms.CheckBox();
            this.ckbStickUIThread = new System.Windows.Forms.CheckBox();
            this.ckbThrowAfterAwait = new System.Windows.Forms.CheckBox();
            this.ckbThrowBeforeAwait = new System.Windows.Forms.CheckBox();
            this.ckbEnableAwait = new System.Windows.Forms.CheckBox();
            this.btnRun = new System.Windows.Forms.Button();
            this.lblTaskDelayMillis = new System.Windows.Forms.Label();
            this.edtTaskDelayMillis = new System.Windows.Forms.TextBox();
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
            this.panel1.Size = new System.Drawing.Size(925, 438);
            this.panel1.TabIndex = 0;
            // 
            // panel3
            // 
            this.panel3.Controls.Add(this.textBox1);
            this.panel3.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel3.Location = new System.Drawing.Point(135, 0);
            this.panel3.Name = "panel3";
            this.panel3.Size = new System.Drawing.Size(790, 438);
            this.panel3.TabIndex = 1;
            // 
            // textBox1
            // 
            this.textBox1.BackColor = System.Drawing.SystemColors.ButtonHighlight;
            this.textBox1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.textBox1.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.textBox1.Location = new System.Drawing.Point(0, 0);
            this.textBox1.Multiline = true;
            this.textBox1.Name = "textBox1";
            this.textBox1.ReadOnly = true;
            this.textBox1.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.textBox1.Size = new System.Drawing.Size(790, 438);
            this.textBox1.TabIndex = 0;
            // 
            // panel2
            // 
            this.panel2.Controls.Add(this.edtTaskDelayMillis);
            this.panel2.Controls.Add(this.lblTaskDelayMillis);
            this.panel2.Controls.Add(this.edtUIThreadMillis);
            this.panel2.Controls.Add(this.ckbMainUISleep);
            this.panel2.Controls.Add(this.lblRunParam);
            this.panel2.Controls.Add(this.btnClearText);
            this.panel2.Controls.Add(this.ckbAppendText);
            this.panel2.Controls.Add(this.ckbStickUIThread);
            this.panel2.Controls.Add(this.ckbThrowAfterAwait);
            this.panel2.Controls.Add(this.ckbThrowBeforeAwait);
            this.panel2.Controls.Add(this.ckbEnableAwait);
            this.panel2.Controls.Add(this.btnRun);
            this.panel2.Dock = System.Windows.Forms.DockStyle.Left;
            this.panel2.Location = new System.Drawing.Point(0, 0);
            this.panel2.Name = "panel2";
            this.panel2.Size = new System.Drawing.Size(135, 438);
            this.panel2.TabIndex = 0;
            // 
            // edtUIThreadMillis
            // 
            this.edtUIThreadMillis.Location = new System.Drawing.Point(25, 183);
            this.edtUIThreadMillis.Name = "edtUIThreadMillis";
            this.edtUIThreadMillis.Size = new System.Drawing.Size(92, 20);
            this.edtUIThreadMillis.TabIndex = 6;
            this.edtUIThreadMillis.Text = "1000";
            // 
            // ckbMainUISleep
            // 
            this.ckbMainUISleep.Location = new System.Drawing.Point(4, 154);
            this.ckbMainUISleep.Name = "ckbMainUISleep";
            this.ckbMainUISleep.Size = new System.Drawing.Size(125, 31);
            this.ckbMainUISleep.TabIndex = 5;
            this.ckbMainUISleep.Text = "UI thread sleep millisec before return";
            this.ckbMainUISleep.UseVisualStyleBackColor = true;
            this.ckbMainUISleep.CheckedChanged += new System.EventHandler(this.RunParamChanged);
            // 
            // lblRunParam
            // 
            this.lblRunParam.BackColor = System.Drawing.SystemColors.Control;
            this.lblRunParam.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.lblRunParam.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblRunParam.Location = new System.Drawing.Point(4, 84);
            this.lblRunParam.Name = "lblRunParam";
            this.lblRunParam.Size = new System.Drawing.Size(100, 15);
            this.lblRunParam.TabIndex = 7;
            this.lblRunParam.Text = "(,,)";
            this.lblRunParam.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // btnClearText
            // 
            this.btnClearText.Location = new System.Drawing.Point(25, 400);
            this.btnClearText.Name = "btnClearText";
            this.btnClearText.Size = new System.Drawing.Size(75, 23);
            this.btnClearText.TabIndex = 6;
            this.btnClearText.Text = "&Clear text";
            this.btnClearText.UseVisualStyleBackColor = true;
            this.btnClearText.Click += new System.EventHandler(this.btnClearText_Click);
            // 
            // ckbAppendText
            // 
            this.ckbAppendText.AutoSize = true;
            this.ckbAppendText.Location = new System.Drawing.Point(25, 288);
            this.ckbAppendText.Name = "ckbAppendText";
            this.ckbAppendText.Size = new System.Drawing.Size(87, 17);
            this.ckbAppendText.TabIndex = 20;
            this.ckbAppendText.Text = "&Append Text";
            this.ckbAppendText.UseVisualStyleBackColor = true;
            // 
            // ckbStickUIThread
            // 
            this.ckbStickUIThread.AutoSize = true;
            this.ckbStickUIThread.Location = new System.Drawing.Point(4, 210);
            this.ckbStickUIThread.Name = "ckbStickUIThread";
            this.ckbStickUIThread.Size = new System.Drawing.Size(109, 17);
            this.ckbStickUIThread.TabIndex = 7;
            this.ckbStickUIThread.Text = "Stick &to UI thread";
            this.ckbStickUIThread.UseVisualStyleBackColor = true;
            // 
            // ckbThrowAfterAwait
            // 
            this.ckbThrowAfterAwait.AutoSize = true;
            this.ckbThrowAfterAwait.Location = new System.Drawing.Point(4, 61);
            this.ckbThrowAfterAwait.Name = "ckbThrowAfterAwait";
            this.ckbThrowAfterAwait.Size = new System.Drawing.Size(104, 17);
            this.ckbThrowAfterAwait.TabIndex = 3;
            this.ckbThrowAfterAwait.Text = "throw &after await";
            this.ckbThrowAfterAwait.UseVisualStyleBackColor = true;
            this.ckbThrowAfterAwait.CheckedChanged += new System.EventHandler(this.RunParamChanged);
            // 
            // ckbThrowBeforeAwait
            // 
            this.ckbThrowBeforeAwait.AutoSize = true;
            this.ckbThrowBeforeAwait.Location = new System.Drawing.Point(4, 37);
            this.ckbThrowBeforeAwait.Name = "ckbThrowBeforeAwait";
            this.ckbThrowBeforeAwait.Size = new System.Drawing.Size(113, 17);
            this.ckbThrowBeforeAwait.TabIndex = 2;
            this.ckbThrowBeforeAwait.Text = "throw &before await";
            this.ckbThrowBeforeAwait.UseVisualStyleBackColor = true;
            this.ckbThrowBeforeAwait.CheckedChanged += new System.EventHandler(this.RunParamChanged);
            // 
            // ckbEnableAwait
            // 
            this.ckbEnableAwait.AutoSize = true;
            this.ckbEnableAwait.Location = new System.Drawing.Point(4, 13);
            this.ckbEnableAwait.Name = "ckbEnableAwait";
            this.ckbEnableAwait.Size = new System.Drawing.Size(86, 17);
            this.ckbEnableAwait.TabIndex = 1;
            this.ckbEnableAwait.Text = "&enable await";
            this.ckbEnableAwait.UseVisualStyleBackColor = true;
            this.ckbEnableAwait.CheckedChanged += new System.EventHandler(this.RunParamChanged);
            // 
            // btnRun
            // 
            this.btnRun.Location = new System.Drawing.Point(25, 256);
            this.btnRun.Name = "btnRun";
            this.btnRun.Size = new System.Drawing.Size(83, 25);
            this.btnRun.TabIndex = 0;
            this.btnRun.Text = "&Run";
            this.btnRun.UseVisualStyleBackColor = true;
            this.btnRun.Click += new System.EventHandler(this.btnRun_Click);
            // 
            // lblTaskDelayMillis
            // 
            this.lblTaskDelayMillis.AutoSize = true;
            this.lblTaskDelayMillis.Location = new System.Drawing.Point(4, 106);
            this.lblTaskDelayMillis.Name = "lblTaskDelayMillis";
            this.lblTaskDelayMillis.Size = new System.Drawing.Size(123, 13);
            this.lblTaskDelayMillis.TabIndex = 10;
            this.lblTaskDelayMillis.Text = "Async task delay millisec";
            // 
            // edtTaskDelayMillis
            // 
            this.edtTaskDelayMillis.Location = new System.Drawing.Point(25, 123);
            this.edtTaskDelayMillis.Name = "edtTaskDelayMillis";
            this.edtTaskDelayMillis.Size = new System.Drawing.Size(92, 20);
            this.edtTaskDelayMillis.TabIndex = 4;
            this.edtTaskDelayMillis.Text = "1000";
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(925, 438);
            this.Controls.Add(this.panel1);
            this.Name = "Form1";
            this.Text = "csAsyncInsight";
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
        private System.Windows.Forms.Button btnRun;
        private System.Windows.Forms.CheckBox ckbThrowAfterAwait;
        private System.Windows.Forms.CheckBox ckbThrowBeforeAwait;
        private System.Windows.Forms.CheckBox ckbEnableAwait;
        private System.Windows.Forms.CheckBox ckbStickUIThread;
        private System.Windows.Forms.CheckBox ckbAppendText;
        private System.Windows.Forms.Button btnClearText;
        private System.Windows.Forms.TextBox lblRunParam;
        private System.Windows.Forms.TextBox edtUIThreadMillis;
        private System.Windows.Forms.CheckBox ckbMainUISleep;
        private System.Windows.Forms.TextBox edtTaskDelayMillis;
        private System.Windows.Forms.Label lblTaskDelayMillis;
    }
}

