using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.ServiceProcess;
using System.Text;

using System.Net.NetworkInformation;
using System.Timers;
using System.Configuration;
using System.Threading;
using ScreenDNS_Client.Services;

namespace ScreenDNS_Client
{
    public partial class ScreenDNSService : ServiceBase
    {
        System.Timers.Timer updateTimer;
         
        #region " Properties "

        private int UpdateInterval
        {
            get
            {
                int interval = 3600000;

                if (ConfigurationSettings.AppSettings["update_interval"] != null)
                    interval = Convert.ToInt32(ConfigurationSettings.AppSettings["update_interval"]);

                return interval;
            }
        }
 
        #endregion

        #region " Constructor "

        public ScreenDNSService()
        {
            InitializeComponent();

            if (!System.Diagnostics.EventLog.SourceExists("ScreenDNS"))
            {
                System.Diagnostics.EventLog.CreateEventSource("ScreenDNS", "ScreenDNSLog");
            }

            eventLogScreenDNS.Source = "ScreenDNS";
            eventLogScreenDNS.Log = "ScreenDNSLog";
        }

        #endregion

        private void InitService()
        {
            eventLogScreenDNS.WriteEntry("ScreenDNS Client Started.");
            NetworkChange.NetworkAddressChanged += new NetworkAddressChangedEventHandler(NetworkChange_NetworkAddressChanged);

            updateTimer = new System.Timers.Timer();
            updateTimer.Elapsed += new ElapsedEventHandler(updateTimer_Elapsed);
            updateTimer.Interval = UpdateInterval;
            updateTimer.Enabled = true;

            bool retCode = IPService.Singleton.UpdateClientIP();
        }       

        #region " Events "

        void updateTimer_Elapsed(object sender, ElapsedEventArgs e)
        {
            bool retCode = IPService.Singleton.UpdateClientIP();
        }

        void NetworkChange_NetworkAddressChanged(object sender, EventArgs e)
        {
            updateTimer.Enabled = false;

            // Network changed, wait 60 seconds and update server
            Thread.Sleep(60000);
            bool retCode = IPService.Singleton.UpdateClientIP();

            updateTimer.Enabled = true;
        }
        
        #endregion

        #region " Default Methods "

        protected override void OnStart(string[] args)
        {
            InitService();
        }

        protected override void OnStop()
        {
            updateTimer.Enabled = false;
            eventLogScreenDNS.WriteEntry("ScreenDNS Client Stopped.");
        }

        #endregion
    }
}
