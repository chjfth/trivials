
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
            this.components = new System.ComponentModel.Container();
            this.panel1 = new System.Windows.Forms.Panel();
            this.panel3Right = new System.Windows.Forms.Panel();
            this.btnStop = new System.Windows.Forms.Button();
            this.btnNavigate = new System.Windows.Forms.Button();
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.wb1 = new System.Windows.Forms.WebBrowser();
            this.btnClear = new System.Windows.Forms.Button();
            this.tboxStatus = new System.Windows.Forms.TextBox();
            this.tboxLog = new System.Windows.Forms.TextBox();
            this.lblURL = new System.Windows.Forms.Label();
            this.cbxURL = new System.Windows.Forms.ComboBox();
            this.panel2Left = new System.Windows.Forms.FlowLayoutPanel();
            this.lblIESoftVer = new System.Windows.Forms.Label();
            this.lblWbCommands = new System.Windows.Forms.Label();
            this.lnkRefresh = new System.Windows.Forms.LinkLabel();
            this.lnkBack = new System.Windows.Forms.LinkLabel();
            this.lnkForward = new System.Windows.Forms.LinkLabel();
            this.lnkDocumentMode = new System.Windows.Forms.LinkLabel();
            this.lblEvents = new System.Windows.Forms.Label();
            this.ckbProgressChanged = new System.Windows.Forms.CheckBox();
            this.ckbWBStatusTextChanged = new System.Windows.Forms.CheckBox();
            this.ckbBlockNewWindow = new System.Windows.Forms.CheckBox();
            this.lblWbProperties = new System.Windows.Forms.Label();
            this.ckbScriptErrorsSuppressed = new System.Windows.Forms.CheckBox();
            this.lblSeparator1 = new System.Windows.Forms.Label();
            this.ckbPollReadyState = new System.Windows.Forms.CheckBox();
            this.lblPollRSEveryMs = new System.Windows.Forms.Label();
            this.tboxPollReadyStateMs = new System.Windows.Forms.TextBox();
            this.tmrPollReadyState = new System.Windows.Forms.Timer(this.components);
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
            this.panel3Right.Controls.Add(this.btnStop);
            this.panel3Right.Controls.Add(this.btnNavigate);
            this.panel3Right.Controls.Add(this.splitContainer1);
            this.panel3Right.Controls.Add(this.lblURL);
            this.panel3Right.Controls.Add(this.cbxURL);
            this.panel3Right.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel3Right.Location = new System.Drawing.Point(140, 0);
            this.panel3Right.Name = "panel3Right";
            this.panel3Right.Size = new System.Drawing.Size(765, 557);
            this.panel3Right.TabIndex = 1;
            // 
            // btnStop
            // 
            this.btnStop.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnStop.Location = new System.Drawing.Point(707, 0);
            this.btnStop.Name = "btnStop";
            this.btnStop.Size = new System.Drawing.Size(58, 22);
            this.btnStop.TabIndex = 4;
            this.btnStop.Text = "Sto&p";
            this.btnStop.UseVisualStyleBackColor = true;
            this.btnStop.Click += new System.EventHandler(this.btnStop_Click);
            // 
            // btnNavigate
            // 
            this.btnNavigate.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnNavigate.Location = new System.Drawing.Point(648, 0);
            this.btnNavigate.Name = "btnNavigate";
            this.btnNavigate.Size = new System.Drawing.Size(58, 22);
            this.btnNavigate.TabIndex = 3;
            this.btnNavigate.Text = "&Navigate";
            this.btnNavigate.UseVisualStyleBackColor = true;
            this.btnNavigate.Click += new System.EventHandler(this.btnNavigate_Click);
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
            this.btnClear.Click += new System.EventHandler(this.btnClear_Click);
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
            this.tboxLog.BackColor = System.Drawing.SystemColors.GradientInactiveCaption;
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
            this.lblURL.TabIndex = 1;
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
            this.cbxURL.Size = new System.Drawing.Size(592, 22);
            this.cbxURL.TabIndex = 2;
            this.cbxURL.KeyDown += new System.Windows.Forms.KeyEventHandler(this.cbxURL_KeyDown);
            this.cbxURL.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.cbxURL_KeyPress);
            this.cbxURL.KeyUp += new System.Windows.Forms.KeyEventHandler(this.cbxURL_KeyUp);
            // 
            // panel2Left
            // 
            this.panel2Left.Controls.Add(this.lblIESoftVer);
            this.panel2Left.Controls.Add(this.lblWbCommands);
            this.panel2Left.Controls.Add(this.lnkRefresh);
            this.panel2Left.Controls.Add(this.lnkBack);
            this.panel2Left.Controls.Add(this.lnkForward);
            this.panel2Left.Controls.Add(this.lnkDocumentMode);
            this.panel2Left.Controls.Add(this.lblEvents);
            this.panel2Left.Controls.Add(this.ckbProgressChanged);
            this.panel2Left.Controls.Add(this.ckbWBStatusTextChanged);
            this.panel2Left.Controls.Add(this.ckbBlockNewWindow);
            this.panel2Left.Controls.Add(this.lblWbProperties);
            this.panel2Left.Controls.Add(this.ckbScriptErrorsSuppressed);
            this.panel2Left.Controls.Add(this.lblSeparator1);
            this.panel2Left.Controls.Add(this.ckbPollReadyState);
            this.panel2Left.Controls.Add(this.lblPollRSEveryMs);
            this.panel2Left.Controls.Add(this.tboxPollReadyStateMs);
            this.panel2Left.Dock = System.Windows.Forms.DockStyle.Left;
            this.panel2Left.Location = new System.Drawing.Point(0, 0);
            this.panel2Left.Name = "panel2Left";
            this.panel2Left.Size = new System.Drawing.Size(140, 557);
            this.panel2Left.TabIndex = 0;
            // 
            // lblIESoftVer
            // 
            this.lblIESoftVer.AutoSize = true;
            this.lblIESoftVer.Location = new System.Drawing.Point(3, 0);
            this.lblIESoftVer.Name = "lblIESoftVer";
            this.lblIESoftVer.Size = new System.Drawing.Size(100, 13);
            this.lblIESoftVer.TabIndex = 0;
            this.lblIESoftVer.Text = "IE software version:";
            // 
            // lblWbCommands
            // 
            this.lblWbCommands.AutoSize = true;
            this.lblWbCommands.Location = new System.Drawing.Point(3, 13);
            this.lblWbCommands.Name = "lblWbCommands";
            this.lblWbCommands.Size = new System.Drawing.Size(126, 13);
            this.lblWbCommands.TabIndex = 3;
            this.lblWbCommands.Text = "Common WB commands:";
            // 
            // lnkRefresh
            // 
            this.lnkRefresh.AutoSize = true;
            this.lnkRefresh.Location = new System.Drawing.Point(3, 26);
            this.lnkRefresh.Name = "lnkRefresh";
            this.lnkRefresh.Size = new System.Drawing.Size(44, 13);
            this.lnkRefresh.TabIndex = 4;
            this.lnkRefresh.TabStop = true;
            this.lnkRefresh.Text = "Refresh";
            this.lnkRefresh.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.lnkRefresh_LinkClicked);
            // 
            // lnkBack
            // 
            this.lnkBack.AutoSize = true;
            this.lnkBack.Location = new System.Drawing.Point(53, 26);
            this.lnkBack.Name = "lnkBack";
            this.lnkBack.Size = new System.Drawing.Size(32, 13);
            this.lnkBack.TabIndex = 5;
            this.lnkBack.TabStop = true;
            this.lnkBack.Text = "Back";
            this.lnkBack.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.lnkBack_LinkClicked);
            // 
            // lnkForward
            // 
            this.lnkForward.AutoSize = true;
            this.lnkForward.Location = new System.Drawing.Point(91, 26);
            this.lnkForward.Name = "lnkForward";
            this.lnkForward.Size = new System.Drawing.Size(45, 13);
            this.lnkForward.TabIndex = 6;
            this.lnkForward.TabStop = true;
            this.lnkForward.Text = "Forward";
            this.lnkForward.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.lnkForward_LinkClicked);
            // 
            // lnkDocumentMode
            // 
            this.lnkDocumentMode.AutoSize = true;
            this.lnkDocumentMode.Location = new System.Drawing.Point(3, 39);
            this.lnkDocumentMode.Name = "lnkDocumentMode";
            this.lnkDocumentMode.Size = new System.Drawing.Size(103, 13);
            this.lnkDocumentMode.TabIndex = 15;
            this.lnkDocumentMode.TabStop = true;
            this.lnkDocumentMode.Text = "Get DocumentMode";
            this.lnkDocumentMode.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.lnkDocumentMode_LinkClicked);
            // 
            // lblEvents
            // 
            this.lblEvents.AutoSize = true;
            this.lblEvents.Location = new System.Drawing.Point(3, 52);
            this.lblEvents.Name = "lblEvents";
            this.lblEvents.Size = new System.Drawing.Size(106, 13);
            this.lblEvents.TabIndex = 1;
            this.lblEvents.Text = "WebBrowser events:";
            // 
            // ckbProgressChanged
            // 
            this.ckbProgressChanged.AutoSize = true;
            this.ckbProgressChanged.Location = new System.Drawing.Point(3, 68);
            this.ckbProgressChanged.Name = "ckbProgressChanged";
            this.ckbProgressChanged.Size = new System.Drawing.Size(110, 17);
            this.ckbProgressChanged.TabIndex = 2;
            this.ckbProgressChanged.Text = "ProgressChanged";
            this.ckbProgressChanged.UseVisualStyleBackColor = true;
            this.ckbProgressChanged.CheckedChanged += new System.EventHandler(this.ckbProgressChanged_CheckedChanged);
            // 
            // ckbWBStatusTextChanged
            // 
            this.ckbWBStatusTextChanged.AutoSize = true;
            this.ckbWBStatusTextChanged.Location = new System.Drawing.Point(3, 91);
            this.ckbWBStatusTextChanged.Name = "ckbWBStatusTextChanged";
            this.ckbWBStatusTextChanged.Size = new System.Drawing.Size(120, 17);
            this.ckbWBStatusTextChanged.TabIndex = 10;
            this.ckbWBStatusTextChanged.Text = "StatusTextChanged";
            this.ckbWBStatusTextChanged.UseVisualStyleBackColor = true;
            this.ckbWBStatusTextChanged.CheckedChanged += new System.EventHandler(this.ckbWBStatusTextChanged_CheckedChanged);
            // 
            // ckbBlockNewWindow
            // 
            this.ckbBlockNewWindow.AutoSize = true;
            this.ckbBlockNewWindow.Location = new System.Drawing.Point(3, 114);
            this.ckbBlockNewWindow.Name = "ckbBlockNewWindow";
            this.ckbBlockNewWindow.Size = new System.Drawing.Size(117, 17);
            this.ckbBlockNewWindow.TabIndex = 7;
            this.ckbBlockNewWindow.Text = "Block NewWindow";
            this.ckbBlockNewWindow.UseVisualStyleBackColor = true;
            // 
            // lblWbProperties
            // 
            this.lblWbProperties.AutoSize = true;
            this.lblWbProperties.Location = new System.Drawing.Point(3, 134);
            this.lblWbProperties.Name = "lblWbProperties";
            this.lblWbProperties.Size = new System.Drawing.Size(120, 13);
            this.lblWbProperties.TabIndex = 8;
            this.lblWbProperties.Text = "WebBrowser properties:";
            // 
            // ckbScriptErrorsSuppressed
            // 
            this.ckbScriptErrorsSuppressed.AutoSize = true;
            this.ckbScriptErrorsSuppressed.Location = new System.Drawing.Point(3, 150);
            this.ckbScriptErrorsSuppressed.Name = "ckbScriptErrorsSuppressed";
            this.ckbScriptErrorsSuppressed.Size = new System.Drawing.Size(139, 17);
            this.ckbScriptErrorsSuppressed.TabIndex = 9;
            this.ckbScriptErrorsSuppressed.Text = "ScriptErrorsSuppressed ";
            this.ckbScriptErrorsSuppressed.UseVisualStyleBackColor = true;
            this.ckbScriptErrorsSuppressed.CheckedChanged += new System.EventHandler(this.ckbScriptErrorsSuppressed_CheckedChanged);
            // 
            // lblSeparator1
            // 
            this.lblSeparator1.AutoSize = true;
            this.panel2Left.SetFlowBreak(this.lblSeparator1, true);
            this.lblSeparator1.Location = new System.Drawing.Point(3, 170);
            this.lblSeparator1.Name = "lblSeparator1";
            this.lblSeparator1.Size = new System.Drawing.Size(13, 13);
            this.lblSeparator1.TabIndex = 11;
            this.lblSeparator1.Text = "--";
            // 
            // ckbPollReadyState
            // 
            this.ckbPollReadyState.AutoSize = true;
            this.ckbPollReadyState.Location = new System.Drawing.Point(3, 186);
            this.ckbPollReadyState.Name = "ckbPollReadyState";
            this.ckbPollReadyState.Size = new System.Drawing.Size(123, 17);
            this.ckbPollReadyState.TabIndex = 12;
            this.ckbPollReadyState.Text = "Poll WB ReadyState";
            this.ckbPollReadyState.UseVisualStyleBackColor = true;
            this.ckbPollReadyState.CheckedChanged += new System.EventHandler(this.ckbPollReadyState_CheckedChanged);
            // 
            // lblPollRSEveryMs
            // 
            this.lblPollRSEveryMs.AutoSize = true;
            this.lblPollRSEveryMs.Location = new System.Drawing.Point(3, 206);
            this.lblPollRSEveryMs.Name = "lblPollRSEveryMs";
            this.lblPollRSEveryMs.Size = new System.Drawing.Size(58, 13);
            this.lblPollRSEveryMs.TabIndex = 14;
            this.lblPollRSEveryMs.Text = "every ? ms";
            // 
            // tboxPollReadyStateMs
            // 
            this.tboxPollReadyStateMs.Location = new System.Drawing.Point(67, 209);
            this.tboxPollReadyStateMs.Name = "tboxPollReadyStateMs";
            this.tboxPollReadyStateMs.Size = new System.Drawing.Size(44, 20);
            this.tboxPollReadyStateMs.TabIndex = 13;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(905, 557);
            this.Controls.Add(this.panel1);
            this.Name = "Form1";
            this.Text = "csWebBrowser1";
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.Form1_FormClosed);
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
        private System.Windows.Forms.Button btnNavigate;
        private System.Windows.Forms.ComboBox cbxURL;
        private System.Windows.Forms.Label lblURL;
        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.TextBox tboxStatus;
        private System.Windows.Forms.WebBrowser wb1;
        private System.Windows.Forms.Button btnClear;
        private System.Windows.Forms.Label lblIESoftVer;
        private System.Windows.Forms.Button btnStop;
        private System.Windows.Forms.Label lblEvents;
        private System.Windows.Forms.CheckBox ckbProgressChanged;
        private System.Windows.Forms.LinkLabel lnkRefresh;
        private System.Windows.Forms.Label lblWbCommands;
        private System.Windows.Forms.LinkLabel lnkBack;
        private System.Windows.Forms.LinkLabel lnkForward;
        private System.Windows.Forms.CheckBox ckbBlockNewWindow;
        private System.Windows.Forms.Label lblWbProperties;
        private System.Windows.Forms.CheckBox ckbScriptErrorsSuppressed;
        private System.Windows.Forms.CheckBox ckbWBStatusTextChanged;
        private System.Windows.Forms.Label lblSeparator1;
        private System.Windows.Forms.CheckBox ckbPollReadyState;
        private System.Windows.Forms.Label lblPollRSEveryMs;
        private System.Windows.Forms.TextBox tboxPollReadyStateMs;
        private System.Windows.Forms.Timer tmrPollReadyState;
        private System.Windows.Forms.LinkLabel lnkDocumentMode;
    }
}

