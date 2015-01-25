using System;
using System.Collections;
using System.Configuration;
using System.Data;
using System.Web;
using System.Web.Security;
using System.Web.UI;
using System.Web.UI.HtmlControls;
using System.Web.UI.WebControls;
using System.Web.UI.WebControls.WebParts;
using AvauntGuard.ISPStatsConsolidator.Lib.Services;
using System.Globalization;
using System.Text;

namespace AvauntGuard.ISPStatsConsolidator.WebUI
{
    public partial class _Default : System.Web.UI.Page
    {
        private const double MTIPs = 115000.0;
        private const double EMTELIPs = 1020.0;
        private const double INETMUIPs = 7396;
        private const int BARLENGTH = 15;

        public string OrangeBarImage
        {
            get
            {
                int ispStats = ISPStatsConsolidatorService.Singleton.GetISPStats("", "23889");

                return CreateBars(ispStats, "Orange", "", "", MTIPs);
            }
        }

        public string EmtelBarImage
        {
            get
            {
                int ispStats = ISPStatsConsolidatorService.Singleton.GetISPStats("", "30999");

                return CreateBars(ispStats, "Emtel", "", "", EMTELIPs);
            }
        }

        public int GetHostCount(string category, string asn)
        {
            return ISPStatsConsolidatorService.Singleton.GetISPStats(category, asn);
        }

        public string DefacedBar
        {
            get
            {
                int ispStats = ISPStatsConsolidatorService.Singleton.GetISPStats("Defacement", "23889");
                ispStats += ISPStatsConsolidatorService.Singleton.GetISPStats("Defacement", "30999");

                return CreateBars(ispStats, "Global", "", "", MTIPs + EMTELIPs);
            }
        }

        protected void Page_Load(object sender, EventArgs e)
        {
        }

        //private string RenderStats(string category, string description, string cacheKey)
 
        private string CreateBars(int ispStats , string barTootTip, string category, string header, double totalIPs)
        {
            StringBuilder sb = new StringBuilder();

            int multiplicator = 100;
            double ispScore = (ispStats / totalIPs) * multiplicator;
            double barLength = Math.Ceiling((ispScore) * BARLENGTH);
            string barImage = "";

            if (barLength == 0)
                barLength = 2;
            else if (barLength > 150)
                barLength = 150;

            if (barLength > 120)
                barImage = "./_images/level5.png";
            else if (barLength > 80)
                barImage = "./_images/level4.png";
            else if (barLength > 50)
                barImage = "./_images/level3.png";
            else if (barLength > 25)
                barImage = "./_images/level2.png";
            else
                barImage = "./_images/level1.png";

            return "<img src='" + barImage + "' alt='" + barTootTip + "' height='6' width='" + barLength + "'/>";
        }
    }
}
