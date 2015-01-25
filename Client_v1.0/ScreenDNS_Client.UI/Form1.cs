using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using ScreenDNS_Client.Services;
using ScreenDNS_Client.UI.Components;
using System.ServiceProcess;
using System.Text.RegularExpressions;

namespace ScreenDNS_Client.UI
{
    public partial class Form1 : Form
    {
        private string ServiceName
        {
            get { return "ScreenDNSClient"; }
        }

        public Form1()
        {
            InitializeComponent();

            textBoxCustomerID.Text = IPService.Singleton.CustomerID;

            if (textBoxCustomerID.Text == string.Empty
                || !Regex.IsMatch(textBoxCustomerID.Text, "^[A-Za-z0-9]+$", RegexOptions.IgnoreCase))
            {
                textBoxCustomerID.ReadOnly = false;
                buttoSaveID.Enabled = true;
                buttonEditID.Enabled = false;
            }

            UpdateServiceStatus(ServiceName);
            GenerateRouterString();
        }

        private void buttonRefresh_Click(object sender, EventArgs e)
        {
            GenerateRouterString();
        }

        private void GenerateRouterString()
        {
            buttonRefresh.Enabled = false;
            string routerString = IPService.Singleton.GenerateRouterString();

            //IPService.Singleton.UpdateClientIP();

            if (!routerString.Equals(string.Empty))
                textBox1.Text = routerString;
            else
                textBox1.Text = "No client ID detected. Please key in your client ID";

            buttonRefresh.Enabled = true;
        }

        private void UpdateServiceStatus(string serviceName)
        {
            labelServiceStatus.Text = "Unknown";
            labelServiceStatus.ForeColor = Color.Orange;

            try
            {
                ServiceController service = new ServiceController(serviceName);

                if (service != null && service.Status == ServiceControllerStatus.Running)
                {
                    labelServiceStatus.Text = "Running";
                    labelServiceStatus.ForeColor = Color.DarkGreen;
                    buttonStopService.Enabled = true;
                }
                else
                {
                    labelServiceStatus.Text = "Stopped";
                    labelServiceStatus.ForeColor = Color.Red;
                    buttonStopService.Enabled = false;
                }

            }
            catch (Exception ex) { }
        }

        private void buttonEditID_Click(object sender, EventArgs e)
        {
            textBoxCustomerID.ReadOnly = false;
            buttoSaveID.Enabled = true;
        }

        private void buttoSaveID_Click(object sender, EventArgs e)
        {
            if (textBoxCustomerID.Text != string.Empty
                && Regex.IsMatch(textBoxCustomerID.Text, "^[A-Za-z0-9]+$", RegexOptions.IgnoreCase))
            {
                textBoxCustomerID.ReadOnly = true;
                buttoSaveID.Enabled = false;
                buttonEditID.Enabled = true;

                IPService.Singleton.CustomerID = textBoxCustomerID.Text;
                IPService.Singleton.UpdateClientIP();
            }
            else
                labelError.Text = "Invalid ID";
        }

        private void buttonStopService_Click(object sender, EventArgs e)
        {
            StopService(ServiceName, 30000);
        }

        private void buttonStartService_Click(object sender, EventArgs e)
        {
            StopService(ServiceName, 30000);
            StartService(ServiceName, 30000);
        }

        void StopService(string serviceName, int timeoutMilliseconds)
        {
            try
            {
                ServiceController service = new ServiceController(serviceName);
                TimeSpan timeout = TimeSpan.FromMilliseconds(timeoutMilliseconds);

                service.Stop();
                service.WaitForStatus(ServiceControllerStatus.Stopped, timeout);
                UpdateServiceStatus(ServiceName);
            }
            catch { }
        }

        void StartService(string serviceName, int timeoutMilliseconds)
        {
            try
            {
                ServiceController service = new ServiceController(serviceName);
                TimeSpan timeout = TimeSpan.FromMilliseconds(timeoutMilliseconds);

                service.Start();
                service.WaitForStatus(ServiceControllerStatus.Running, timeout);
                UpdateServiceStatus(ServiceName);
            }
            catch (Exception ex) { textBox1.Text = ex.ToString(); }
        }
    }
}
