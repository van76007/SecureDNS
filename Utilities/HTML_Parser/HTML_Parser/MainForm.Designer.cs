namespace HTML_Parser
{
    partial class MainForm
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
            this.Button_Parse = new System.Windows.Forms.Button();
            this.TextBox_MainUrl = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.TextBox_FirstPage = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.TextBox_LastPage = new System.Windows.Forms.TextBox();
            this.labelStatus = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.TextBox_CategoryString = new System.Windows.Forms.TextBox();
            this.TextBox_PageString = new System.Windows.Forms.TextBox();
            this.TextBox_XPath = new System.Windows.Forms.TextBox();
            this.label6 = new System.Windows.Forms.Label();
            this.Button_DeleteAll = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // Button_Parse
            // 
            this.Button_Parse.Location = new System.Drawing.Point(96, 149);
            this.Button_Parse.Name = "Button_Parse";
            this.Button_Parse.Size = new System.Drawing.Size(101, 23);
            this.Button_Parse.TabIndex = 0;
            this.Button_Parse.Text = "Delete from DB";
            this.Button_Parse.UseVisualStyleBackColor = true;
            this.Button_Parse.Click += new System.EventHandler(this.Button_Parse_Click);
            // 
            // TextBox_MainUrl
            // 
            this.TextBox_MainUrl.Location = new System.Drawing.Point(97, 14);
            this.TextBox_MainUrl.Name = "TextBox_MainUrl";
            this.TextBox_MainUrl.Size = new System.Drawing.Size(250, 20);
            this.TextBox_MainUrl.TabIndex = 1;
            this.TextBox_MainUrl.Text = "http://hosts-file.net/?s=Browse";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 17);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(47, 13);
            this.label1.TabIndex = 2;
            this.label1.Text = "Base Url";
            // 
            // TextBox_FirstPage
            // 
            this.TextBox_FirstPage.Location = new System.Drawing.Point(96, 121);
            this.TextBox_FirstPage.Name = "TextBox_FirstPage";
            this.TextBox_FirstPage.Size = new System.Drawing.Size(40, 20);
            this.TextBox_FirstPage.TabIndex = 3;
            this.TextBox_FirstPage.Text = "1";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(11, 124);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(57, 13);
            this.label2.TabIndex = 4;
            this.label2.Text = "Start Page";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(157, 124);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(55, 13);
            this.label3.TabIndex = 5;
            this.label3.Text = "Last Page";
            // 
            // TextBox_LastPage
            // 
            this.TextBox_LastPage.Location = new System.Drawing.Point(218, 121);
            this.TextBox_LastPage.Name = "TextBox_LastPage";
            this.TextBox_LastPage.Size = new System.Drawing.Size(40, 20);
            this.TextBox_LastPage.TabIndex = 6;
            this.TextBox_LastPage.Text = "10";
            // 
            // labelStatus
            // 
            this.labelStatus.AutoSize = true;
            this.labelStatus.ForeColor = System.Drawing.Color.Brown;
            this.labelStatus.Location = new System.Drawing.Point(93, 224);
            this.labelStatus.Name = "labelStatus";
            this.labelStatus.Size = new System.Drawing.Size(117, 13);
            this.labelStatus.TabIndex = 7;
            this.labelStatus.Text = "Please select an action";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(12, 43);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(79, 13);
            this.label4.TabIndex = 8;
            this.label4.Text = "Category String";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(12, 69);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(70, 13);
            this.label5.TabIndex = 9;
            this.label5.Text = "Paging String";
            // 
            // TextBox_CategoryString
            // 
            this.TextBox_CategoryString.Location = new System.Drawing.Point(97, 40);
            this.TextBox_CategoryString.Name = "TextBox_CategoryString";
            this.TextBox_CategoryString.Size = new System.Drawing.Size(100, 20);
            this.TextBox_CategoryString.TabIndex = 10;
            this.TextBox_CategoryString.Text = "&f=ATS";
            // 
            // TextBox_PageString
            // 
            this.TextBox_PageString.Location = new System.Drawing.Point(97, 66);
            this.TextBox_PageString.Name = "TextBox_PageString";
            this.TextBox_PageString.Size = new System.Drawing.Size(100, 20);
            this.TextBox_PageString.TabIndex = 11;
            this.TextBox_PageString.Text = "&page=";
            // 
            // TextBox_XPath
            // 
            this.TextBox_XPath.Location = new System.Drawing.Point(97, 93);
            this.TextBox_XPath.Name = "TextBox_XPath";
            this.TextBox_XPath.Size = new System.Drawing.Size(250, 20);
            this.TextBox_XPath.TabIndex = 12;
            this.TextBox_XPath.Text = "//table//tr//td//table//tr//td//a";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(12, 96);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(36, 13);
            this.label6.TabIndex = 13;
            this.label6.Text = "XPath";
            // 
            // Button_DeleteAll
            // 
            this.Button_DeleteAll.Location = new System.Drawing.Point(203, 149);
            this.Button_DeleteAll.Name = "Button_DeleteAll";
            this.Button_DeleteAll.Size = new System.Drawing.Size(75, 23);
            this.Button_DeleteAll.TabIndex = 14;
            this.Button_DeleteAll.Text = "Delete All";
            this.Button_DeleteAll.UseVisualStyleBackColor = true;
            this.Button_DeleteAll.Click += new System.EventHandler(this.Button_DeleteAll_Click);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(364, 266);
            this.Controls.Add(this.Button_DeleteAll);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.TextBox_XPath);
            this.Controls.Add(this.TextBox_PageString);
            this.Controls.Add(this.TextBox_CategoryString);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.labelStatus);
            this.Controls.Add(this.TextBox_LastPage);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.TextBox_FirstPage);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.TextBox_MainUrl);
            this.Controls.Add(this.Button_Parse);
            this.Name = "MainForm";
            this.Text = "Form1";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button Button_Parse;
        private System.Windows.Forms.TextBox TextBox_MainUrl;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox TextBox_FirstPage;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox TextBox_LastPage;
        private System.Windows.Forms.Label labelStatus;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.TextBox TextBox_CategoryString;
        private System.Windows.Forms.TextBox TextBox_PageString;
        private System.Windows.Forms.TextBox TextBox_XPath;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Button Button_DeleteAll;
    }
}

