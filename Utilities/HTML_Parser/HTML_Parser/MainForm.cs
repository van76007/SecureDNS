using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Net;
using System.IO;
using HtmlAgilityPack;
using System.Text.RegularExpressions;
using System.Data.Common;
using Microsoft.Practices.EnterpriseLibrary.Data;
using HTMLAgilityParser.Services;

namespace HTML_Parser
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();
        }

        private void Button_Parse_Click(object sender, EventArgs e)
        {
            int firstPage = Convert.ToInt32(TextBox_FirstPage.Text);
            int lastPage = Convert.ToInt32(TextBox_LastPage.Text);
            labelStatus.Text = "Deleting data from database ...";

            int entriesFound = 0;
            int entriesAffected = 0;
            int ipsAffected = 0;

            Button_Parse.Enabled = false;
            HtmlParserService.Singleton.DeleteMalwareEntries(TextBox_MainUrl.Text, TextBox_CategoryString.Text, TextBox_XPath.Text, TextBox_PageString.Text, firstPage, lastPage, out entriesFound, out entriesAffected, out ipsAffected);
            Button_Parse.Enabled = true;

            labelStatus.Text = "Deleted " + entriesAffected + " records out of " + entriesFound + " found by parser.";
        }

        private void Button_DeleteAll_Click(object sender, EventArgs e)
        {
            Button_DeleteAll.Enabled = false;
            HtmlParserService.Singleton.RunAutomatedCheck();

            labelStatus.Text = "Operation Success";
            Environment.Exit(0);
        }
    }
}
