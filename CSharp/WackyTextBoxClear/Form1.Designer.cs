namespace WackyTextBoxClear
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
            this.btnFill = new System.Windows.Forms.Button();
            this.btnClearNoRefresh = new System.Windows.Forms.Button();
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.lblHint = new System.Windows.Forms.Label();
            this.btnClearAndRefresh = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // btnFill
            // 
            this.btnFill.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.btnFill.Location = new System.Drawing.Point(12, 70);
            this.btnFill.Name = "btnFill";
            this.btnFill.Size = new System.Drawing.Size(123, 58);
            this.btnFill.TabIndex = 0;
            this.btnFill.Text = "&Fill";
            this.btnFill.UseVisualStyleBackColor = true;
            this.btnFill.Click += new System.EventHandler(this.btnFill_Click);
            // 
            // btnClearNoRefresh
            // 
            this.btnClearNoRefresh.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.btnClearNoRefresh.Location = new System.Drawing.Point(149, 70);
            this.btnClearNoRefresh.Name = "btnClearNoRefresh";
            this.btnClearNoRefresh.Size = new System.Drawing.Size(123, 27);
            this.btnClearNoRefresh.TabIndex = 1;
            this.btnClearNoRefresh.Text = "&Clear1";
            this.btnClearNoRefresh.UseVisualStyleBackColor = true;
            this.btnClearNoRefresh.Click += new System.EventHandler(this.btnClearNoRefresh_Click);
            // 
            // textBox1
            // 
            this.textBox1.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.textBox1.Location = new System.Drawing.Point(12, 134);
            this.textBox1.Multiline = true;
            this.textBox1.Name = "textBox1";
            this.textBox1.Size = new System.Drawing.Size(260, 116);
            this.textBox1.TabIndex = 2;
            // 
            // lblHint
            // 
            this.lblHint.Location = new System.Drawing.Point(13, 13);
            this.lblHint.Name = "lblHint";
            this.lblHint.Size = new System.Drawing.Size(259, 52);
            this.lblHint.TabIndex = 3;
            this.lblHint.Text = "label1";
            // 
            // btnClearAndRefresh
            // 
            this.btnClearAndRefresh.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.btnClearAndRefresh.Location = new System.Drawing.Point(149, 101);
            this.btnClearAndRefresh.Name = "btnClearAndRefresh";
            this.btnClearAndRefresh.Size = new System.Drawing.Size(123, 27);
            this.btnClearAndRefresh.TabIndex = 4;
            this.btnClearAndRefresh.Text = "&Clear2";
            this.btnClearAndRefresh.UseVisualStyleBackColor = true;
            this.btnClearAndRefresh.Click += new System.EventHandler(this.btnClearAndRefersh_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(284, 262);
            this.Controls.Add(this.btnClearAndRefresh);
            this.Controls.Add(this.lblHint);
            this.Controls.Add(this.textBox1);
            this.Controls.Add(this.btnClearNoRefresh);
            this.Controls.Add(this.btnFill);
            this.Name = "Form1";
            this.Text = "Wacky TextBox.Clear";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button btnFill;
        private System.Windows.Forms.Button btnClearNoRefresh;
        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.Label lblHint;
        private System.Windows.Forms.Button btnClearAndRefresh;
    }
}

