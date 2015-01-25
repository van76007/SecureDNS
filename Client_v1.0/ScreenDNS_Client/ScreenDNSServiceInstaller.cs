using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Configuration.Install;
using System.ServiceProcess;


namespace ScreenDNS_Client
{
    [RunInstaller(true)]
    public partial class ScreenDNSServiceInstaller : Installer
    {
        public ScreenDNSServiceInstaller()
        {
            InitializeComponent();
        }

        protected override void OnAfterInstall(System.Collections.IDictionary savedState)
        {
            base.OnAfterInstall(savedState);
            using (var serviceController = new ServiceController(this.serviceInstaller1.ServiceName, Environment.MachineName))
                serviceController.Start();
        }
    }
}
