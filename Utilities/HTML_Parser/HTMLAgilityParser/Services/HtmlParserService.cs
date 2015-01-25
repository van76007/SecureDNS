using System;
using System.Collections.Generic;
using System.Text;
using System.Net;
using System.IO;
using HTMLAgilityParser.DataAccess;
using System.Text.RegularExpressions;
using HtmlAgilityPack;
using System.Threading;
using System.Web;
using System.Collections;
using System.Data;

namespace HTMLAgilityParser.Services
{
    public class HtmlParserService
    {
        #region Variables

        private volatile static HtmlParserService singleton;
        private static object lockObj = new object();

        private string urlREGEXPattern = @"[\w\-_]+(\.[\w\-_]+)+([\w\-\.,@?^=%&amp;:/~\+#]*[\w\-\@?^=%&amp;/~\+#])?";
        private string ipREGEXPattern = @"(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9])\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[0-9])";

        #endregion

        #region Enums

        private enum CountryCodes
        {
            AL,
            DZ,
            AR,
            AM,
            AU,
            AT,
            AZ,
            BS,
            BH,
            BD,
            BB,
            BY,
            BE,
            BO,
            BA,
            BR,
            BN,
            BG,
            KH,
            CM,
            CA,
            CL,
            CN,
            CO,
            CG,
            CR,
            CI,
            HR,
            CU,
            CY,
            CZ,
            DK,
            DO,
            EC,
            EG,
            SV,
            EE,
            FI,
            FR,
            GE,
            DE,
            GH,
            GR,
            GP,
            GT,
            HN,
            HK,
            HU,
            IS,
            IN,
            ID,
            IR,
            IQ,
            IE,
            IL,
            IT,
            JM,
            JP,
            JO,
            KZ,
            KE,
            KW,
            KG,
            LV,
            LB,
            LY,
            LT,
            LU,
            MO,
            MK,
            MG,
            MY,
            MT,
            MQ,
            MR,
            MU,
            MX,
            MD,
            MN,
            ZR,
            MA,
            NP,
            NL,
            AN,
            NZ,
            NI,
            NG,
            NO,
            OM,
            PK,
            PS,
            PA,
            PY,
            PE,
            PH,
            PL,
            PT,
            PR,
            QA,
            RE,
            RO,
            RU,
            SA,
            SN,
            CS,
            SG,
            SK,
            SI,
            ZA,
            KR,
            ES,
            LK,
            SD,
            SE,
            CH,
            SY,
            TW,
            TH,
            TT,
            TN,
            TR,
            UG,
            UA,
            GB,
            US,
            UY,
            UZ,
            VE,
            VN,
            YE
        }

        #endregion

        #region Singleton

        public static HtmlParserService Singleton
        {
            get
            {
                if (singleton == null)
                {
                    lock (lockObj)
                    {
                        if (singleton == null)
                            singleton = new HtmlParserService();
                    }
                }

                return singleton;
            }
        }

        #endregion

        #region Malware

        private string GetUnifiedResponse(string unified_response_template, string category)
        {
            // Code to determine unified_response
            // to transpose
            string flavor = "2";
            string reason = "16";
            category = category.ToUpper();

            if (category.IndexOf("INFECTION") >= 0
            || category.IndexOf("BACKDOOR") >= 0
            || category.IndexOf("C&C") >= 0
            || category.IndexOf("BOT") >= 0
            || category.IndexOf("CONFIG") >= 0
            || category.IndexOf("CALL*HOM") >= 0
            || category.IndexOf("DROP") >= 0)
            {
                reason = "32";
            }
            else if (category.IndexOf("VIRU") >= 0
            || category.IndexOf("WORM") >= 0
            || category.IndexOf("ZEUS") >= 0
            || category.IndexOf("BANKER") >= 0
            || category.IndexOf("SPYEYE") >= 0
            || category.IndexOf("CARBERP") >= 0
            || category.IndexOf("MALWARE") >= 0
            || category.IndexOf("DOWNLOADER") >= 0
            || category.IndexOf("DUQU") >= 0
            || category.IndexOf("GOZI") >= 0
            || category.IndexOf("KOOBFACE") >= 0
            || category.IndexOf("MEBROOT") >= 0
            || category.IndexOf("ROOT") >= 0
            || category.IndexOf("PWS") >= 0
            || category.IndexOf("WIN32") >= 0
            || category.IndexOf("TDL") >= 0
            || category.IndexOf("TDSS") >= 0
            || category.IndexOf("TR/") >= 0
            || category.IndexOf("W32") >= 0
            || category.IndexOf("ZLOB") >= 0
            || category.IndexOf("TORPIG") >= 0
            || category.IndexOf("SINOWAL") >= 0)
            {
                reason = "17";
            }
            else if (category.IndexOf("EXPLOIT") >= 0
            || category.IndexOf("BLACKHOLE") >= 0
            || category.IndexOf("COMPROMI") >= 0
            || category.IndexOf("PACK") >= 0
            || category.IndexOf("CRIMINAL") >= 0
            || category.IndexOf("CRIME") >= 0
            || category.IndexOf("DDOS") >= 0
            || category.IndexOf("DIRECT") >= 0
            || category.IndexOf("CHANGER") >= 0
            || category.IndexOf("DRIVE") >= 0
            || category.IndexOf("FLASH") >= 0
            || category.IndexOf("FTP") >= 0
            || category.IndexOf("JACK") >= 0
            || category.IndexOf("RISK") >= 0
            || category.IndexOf("LOIT") >= 0
            || category.IndexOf("LOGGER") >= 0
            || category.IndexOf("SCRIPT") >= 0
            || category.IndexOf("MALICI") >= 0
            || category.IndexOf("OBFUSC") >= 0
            || category.IndexOf("HACK") >= 0
            || category.IndexOf("PASSW") >= 0
            || category.IndexOf("ACCOUNT") >= 0
            || category.IndexOf("RANS") >= 0
            || category.IndexOf("SQL") >= 0
            || category.IndexOf("RFI") >= 0
            || category.IndexOf("ATTACK") >= 0
            || category.IndexOf("KIT") >= 0)
            {
                reason = "64";
            }
            else if (category.IndexOf("FLUX") >= 0)
            {
                reason = "65";
            }
            else if (category.IndexOf("ADWARE") >= 0
            || category.IndexOf("SPYWARE") >= 0
            || category.IndexOf("FAKEWARE") >= 0)
            {
                reason = "20";
            }
            else if (category.IndexOf("TROJAN") >= 0)
            {
                reason = "19";
            }
            else if (category.IndexOf("BLACKLIST") >= 0)
            {
                flavor = "1";
                reason = "1";
            }
            else if (category.IndexOf("SEARCH") >= 0
            || category.IndexOf("SEO") >= 0
                || category.IndexOf("CARD") >= 0
            || category.IndexOf("SPAM") >= 0
            || category.IndexOf("SCAM") >= 0)
            {
                reason = "128";
            }
            else if (category.IndexOf("PHISH") >= 0
                || category.IndexOf("FRAUD") >= 0
                || category.IndexOf("MULE") >= 0)
            {
                reason = "129";
            }

            return string.Format(unified_response_template, flavor, reason);
        }

        public void ConsolidateMalwareDatabase(bool executeDeletes, bool executeInserts)
        {
            // TODO : Create Enum for categories
            // Put alot of stuff in config
            bool userFile = false;
            int entriesFound = 0;
            int entriesAffected = 0;
            int ipsAffected = 0;
            string category = string.Empty;

            int firstPage = 1;
            int lastPage = 1;

            // Parses a file and saves the contents to DB if contents match criteria
            if (userFile)
            {
                //  AddMalwareEntriesViaFile(@"C:\hr\add.rsk", "Malware", "Secure Mecca", "Site engaged in malware distribution.");
                //  AddMalwareEntriesViaFile(@"C:\hr\add.dead", "Typosquatting", "Secure Mecca", "Site engaged in typo squatting.");
            }

            if (executeInserts)
            {
                category = "Backdoor";
                lastPage = 1;
                AddMalwareEntries("http://hosts-file.net/?s=Browse&f=HJK", "127.2.19.32", category, "hpHosts", "Site engaged in browser hijacking or other forms of hijacking.", "//table//tr//td//table//tr//td//a", "&page=", firstPage, lastPage, out entriesFound, out entriesAffected, out ipsAffected);

                category = "Drive By";
                lastPage = 150;
                AddMalwareEntries("http://hosts-file.net/?s=Browse&f=EXP", "127.2.19.64", category, "hpHosts", "Site engaged in the exploitation of browser and OS vulnerabilities. ", "//table//tr//td//table//tr//td//a", "&page=", firstPage, lastPage, out entriesFound, out entriesAffected, out ipsAffected);

                category = "Malware";
                // lastPage = 900;
                lastPage = 150;
                AddMalwareEntries("http://hosts-file.net/?s=Browse&f=EMD", "127.2.19.17|127.2.19.19|127.2.19.20", category, "hpHosts", "Site engaged in malware distribution", "//table//tr//td//table//tr//td//a", "&page=", firstPage, lastPage, out entriesFound, out entriesAffected, out ipsAffected);

                category = "Malware";
                lastPage = 30;
                AddMalwareEntries("http://hosts-file.net/?s=Browse&f=FSA", "127.2.19.20", category, "hpHosts", "Site engaged in the selling or distribution of bogus or fraudulent applications.", "//table//tr//td//table//tr//td//a", "&page=", firstPage, lastPage, out entriesFound, out entriesAffected, out ipsAffected);

                category = "Phishing";
                // lastPage = 800;
                lastPage = 150;
                AddMalwareEntries("http://hosts-file.net/?s=Browse&f=PSH", "127.2.19.129", category, "hpHosts", "Site engaged in Phishing.", "//table//tr//td//table//tr//td//a", "&page=", firstPage, lastPage, out entriesFound, out entriesAffected, out ipsAffected);
            }

            if (executeDeletes)
            {
                category = "WRZ";
                lastPage = 5;
                DeleteMalwareEntries("http://hosts-file.net/?s=Browse&f=WRZ", category, "//table//tr//td//table//tr//td//a", "&page=", firstPage, lastPage, out entriesFound, out entriesAffected, out ipsAffected);

                category = "HFS";
                lastPage = 4;
                DeleteMalwareEntries("http://hosts-file.net/?s=Browse&f=HFS", category, "//table//tr//td//table//tr//td//a", "&page=", firstPage, lastPage, out entriesFound, out entriesAffected, out ipsAffected);

                category = "GRM";
                lastPage = 23;
                DeleteMalwareEntries("http://hosts-file.net/?s=Browse&f=GRM", category, "//table//tr//td//table//tr//td//a", "&page=", firstPage, lastPage, out entriesFound, out entriesAffected, out ipsAffected);

                category = "ATS";
                lastPage = 100;
                DeleteMalwareEntries("http://hosts-file.net/?s=Browse&f=ATS", category, "//table//tr//td//table//tr//td//a", "&page=", firstPage, lastPage, out entriesFound, out entriesAffected, out ipsAffected);
            }
        }

        public void DeleteMalwareEntries(string baseUrl, string category, string xPathExpression, string pagingSalt, int firstPage, int lastPage, out int entriesFound, out int entriesAffected, out int ipsAffected)
        {
            // Regex to match valid urls            
            string baseQueryUrl = baseUrl + pagingSalt;

            entriesFound = 0;
            entriesAffected = 0;
            ipsAffected = 0;

            for (int i = firstPage; i <= lastPage; i++)
            {
                HtmlNodeCollection collection = GetHtmlNodes(String.Concat(baseQueryUrl, pagingSalt, i), xPathExpression);

                if (collection == null)
                    break;

                foreach (HtmlNode link in collection)
                {
                    Match m = Regex.Match(link.InnerText, urlREGEXPattern, RegexOptions.IgnoreCase);

                    if (m != null && m.Groups.Count > 0 && m.Groups[0].ToString().Trim() != "")
                    {
                        entriesFound++;

                        // IP Address, check in IP table
                        if (Regex.IsMatch(link.InnerText, ipREGEXPattern, RegexOptions.IgnoreCase))
                        {
                            //if (DataAccessHtmlParser.BlockedIPExists(m.Groups[0].ToString()))
                            {
                                ipsAffected++;
                                DataAccessHtmlParser.DeleteBlockedIPRecord(m.Groups[0].ToString());
                            }
                        }
                        // Domain, check in domain table
                        else
                        {
                            // Exclusions
                            AddExclusion(m.Groups[0].ToString(), category, null);

                            //if (DataAccessHtmlParser.MalwareSiteExists(m.Groups[0].ToString()))
                            {
                                entriesAffected++;

                                DataAccessHtmlParser.DeleteMalwareRecord(m.Groups[0].ToString());
                            }
                        }
                    }
                }
            }

            using (StreamWriter file = new StreamWriter(String.Format(@"C:\Automated_DB_Cleanup_{0}.txt", category), true))
            {
                file.WriteLine("Time is " + DateTime.Now.ToString("yyyy-MM-dd hh:mm:ss tt"));
                file.WriteLine(String.Format("Deleted {0} entries out of {1} entries parsed, for category {2}", entriesAffected, entriesFound, category));
                file.WriteLine(String.Format("Deleted {0} ips out of {1} entries parsed, for category {2}", ipsAffected, entriesFound, category));
                file.WriteLine(Environment.NewLine);
            }
        }

        public int AddMalwareRecordWithFilter(string hostname, string unified_response_template, string category, string source, string description)
        {
            return AddMalwareRecordWithFilter(hostname, unified_response_template, category, description, "", source);
        }

        public int AddMalwareRecordWithFilter(string hostname, string unified_response_template, string category, string description, string targethost, string source)
        {
            int retCode = -1;
            bool canAddEntry = true;
            string domain = "";

            if (targethost == null)
                targethost = "";

            try
            {
                hostname = Regex.Match(hostname, urlREGEXPattern, RegexOptions.IgnoreCase).Groups[0].ToString();

                if (hostname.IndexOf('/') >= 0)
                    hostname = hostname.Remove(hostname.IndexOf('/'));

                string[] sections = hostname.Split('.');

                if (sections.Length >= 2)
                {
                    domain = sections[sections.Length - 2] + "." + sections[sections.Length - 1];
                    DataSet exclusions = HtmlParserService.Singleton.GetExclusions(hostname, domain, category);

                    // if extry exists in DB
                    // or if entry matches database Regex
                    // we should not add it
                    if (exclusions.Tables.Count > 0)
                        foreach (DataRow dr in exclusions.Tables[0].Rows)
                            if (dr["hostname"].ToString().ToLower().Trim().Equals(hostname.ToLower())
                                || dr["domain"].ToString().ToLower().Trim().Equals(hostname.ToLower())
                                || Regex.IsMatch(hostname, dr["regex_pattern"].ToString().ToLower().Trim(), RegexOptions.IgnoreCase))
                            {
                                canAddEntry = false;

                                Console.WriteLine(string.Format("CAN'T ADD MALWARE SITE '{0}'.", hostname));

                                break;
                            }
                }

                if (canAddEntry && !hostname.Equals(string.Empty) && hostname.Length > 4)
                {
                    foreach (string template in unified_response_template.Split('|'))
                    {
                        string unified_response;

                        if (unified_response_template.IndexOf("{0}") >= 0)
                            unified_response = GetUnifiedResponse(template, category);
                        else
                            unified_response = unified_response_template;

                        Console.WriteLine("Adding domain '{0} / {1}' : '{2}'.", hostname, unified_response, description);
                        retCode = DataAccessHtmlParser.AddMalwareRecord(hostname, unified_response, category, description, targethost, source);
                    }
                }
            }
            catch (Exception ex) { }

            return retCode;
        }

        public void AddMalwareEntries(string baseQueryUrl, string unified_response_template, string category, string source, string description, string xPathExpression, string pagingSalt, int firstPage, int lastPage, out int entriesFound, out int entriesAffected, out int ipsAffected)
        {
            Match m = null;

            entriesFound = 0;
            entriesAffected = 0;
            ipsAffected = 0;

            for (int i = firstPage; i <= lastPage; i++)
            {
             //   try
                {
                    m = null;
                    bool isIP = false;

                    foreach (HtmlNode link in GetHtmlNodes(String.Concat(baseQueryUrl, pagingSalt, i), xPathExpression))
                    {
                        // IP Address, check in IP table
                        if (Regex.IsMatch(link.InnerText, ipREGEXPattern, RegexOptions.IgnoreCase))
                            isIP = true;

                        // Domain, check in domain table
                        if (!isIP && Regex.IsMatch(link.InnerText, urlREGEXPattern, RegexOptions.IgnoreCase))
                        {
                            m = Regex.Match(link.InnerText, urlREGEXPattern, RegexOptions.IgnoreCase);

                            if (m != null && m.Groups.Count > 0 && !(m.Groups[0].ToString().Trim().Equals("")))
                            {
                                entriesFound++;
                                entriesAffected++;

                                foreach (string response in unified_response_template.Split('|'))
                                    AddMalwareRecordWithFilter(m.Groups[0].ToString(), response, category, description, "", source);
                            }
                        }
                    }
                }
               // catch (Exception ex) { }

                // Sleep 30 secs to prevent DoS
                Thread.Sleep(30000);
            }

            using (StreamWriter file = new StreamWriter(String.Format(@"C:\Automated_DB_Update_{0}.txt", category), true))
            {
                file.WriteLine("Time is " + DateTime.Now.ToString("yyyy-MM-dd hh:mm:ss tt"));
                file.WriteLine(String.Format("Added {0} entries out of {1} entries parsed, for category {2}", entriesAffected, entriesFound, category));
                file.WriteLine(String.Format("Added {0} ips out of {1} entries parsed, for category {2}", ipsAffected, entriesFound, category));
                file.WriteLine(Environment.NewLine);
            }
        }

        public void AddMalwareEntriesViaFile(string filePath, string unified_response, string category, string source, string description)
        {
            string text = null;

            int entriesFound = 0;
            int entriesAffected = 0;
            int ipsAffected = 0;

            if (File.Exists(filePath))
            {
                // Parse each text file in the folder
                using (TextReader tr = new StreamReader(filePath))
                {
                    while ((text = tr.ReadLine()) != null)
                    {
                        Match m = Regex.Match(text, urlREGEXPattern, RegexOptions.IgnoreCase);

                        if (m != null && m.Groups.Count > 0 && m.Groups[0].ToString().Trim() != "")
                        {
                            entriesFound++;

                            // IP Address, check in IP table
                            if (Regex.IsMatch(m.Groups[0].ToString(), ipREGEXPattern, RegexOptions.IgnoreCase))
                            {
                                ipsAffected++;
                                DataAccessHtmlParser.AddBlockedIPRecord(m.Groups[0].ToString(), unified_response, category, source, description);
                            }
                            // Domain, check in domain table
                            else
                            {
                                entriesAffected++;
                                DataAccessHtmlParser.AddMalwareRecord(m.Groups[0].ToString(), unified_response, category, source, description);
                            }
                        }
                    }
                }

                using (StreamWriter file = new StreamWriter(String.Format(@"C:\Automated_DB_FileUpdate_{0}_{1}.txt", DateTime.Now.ToString("yyyy-MM-dd"), category), true))
                {
                    file.WriteLine("Time is " + DateTime.Now.ToString("yyyy-MM-dd hh:mm:ss tt"));
                    file.WriteLine(String.Format("Added {0} entries out of {1} entries parsed, for category {2}", entriesAffected, entriesFound, category));
                    file.WriteLine(String.Format("Added {0} ips out of {1} entries parsed, for category {2}", ipsAffected, entriesFound, category));
                    file.WriteLine(Environment.NewLine);
                }
            }
        }

        public void AddBlockedIPRecordWithFilter(string ip, string unified_response_template, string category, string source, string reason)
        {
            ip = ip.Replace("http://", "");
            ip = ip.Replace("https://", "");

            if (ip.IndexOf("/") >= 0)
                ip = ip.Remove(ip.IndexOf("/"));

            if (ip.IndexOf("0") == 0)
                ip = ip.Substring(ip.IndexOf("0") + 1);

            int retCode = -1;
            bool canAddEntry = true;
            ip = ip.Trim();

            try
            {
                DataSet exclusions = HtmlParserService.Singleton.GetExclusions(ip, ip, category);

                // if extry exists in DB
                // or if entry matches database Regex
                // we should not add it
                if (exclusions.Tables.Count > 0)
                    foreach (DataRow dr in exclusions.Tables[0].Rows)
                        if (dr["hostname"].ToString().ToLower().Trim().Equals(ip)
                            || dr["domain"].ToString().ToLower().Trim().Equals(ip)
                            || Regex.IsMatch(ip, dr["regex_pattern"].ToString().ToLower().Trim(), RegexOptions.IgnoreCase)
                            || Regex.IsMatch(ip, "[A-Za-z]", RegexOptions.IgnoreCase))
                        {
                            canAddEntry = false;

                            Console.WriteLine(string.Format("CAN'T ADD MALWARE IP '{0}'.", ip));

                            break;
                        }

                if (canAddEntry && !ip.Trim().Equals(""))
                {
                    foreach (string template in unified_response_template.Split('|'))
                    {
                        string unified_response;

                        if (unified_response_template.IndexOf("{0}") >= 0)
                            unified_response = GetUnifiedResponse(template, category);
                        else
                            unified_response = unified_response_template;

                        Console.WriteLine("Adding IP '{0} / {1}' : '{2}'.", ip, unified_response, reason);
                        retCode = DataAccessHtmlParser.AddBlockedIPRecord(ip, unified_response, category, source, reason);
                    }
                }
            }
            catch (Exception ex) { }
        }

        #endregion

        #region Whitelist

        /// <summary>
        /// If currentCountryCode is empty, then do for all countries
        /// Else do for current country only
        /// </summary>
        /// <param name="currentCountryCode"></param>
        public void ConsolidateWhitelistDatabase(string currentCountryCode)
        {
            string paginationString = string.Empty;
            long globalStart = DateTime.Now.Ticks;
            long start = DateTime.Now.Ticks;
            Dictionary<string, string> domains = null;
            int rank = 0;
            Dictionary<string, string> cachedIPs = new Dictionary<string, string>();

            foreach (string countryCode in Enum.GetNames(typeof(CountryCodes)))
            {
                start = DateTime.Now.Ticks;
                rank = 1;

                // If currentCountryCode is empty, then do for all countries
                // Else do for current country only
                if (currentCountryCode.Equals(string.Empty)
                    || currentCountryCode.ToUpper().Equals(countryCode))
                {
                    if (rank % 3 == 0)
                        cachedIPs = new Dictionary<string, string>();

                    try
                    {
                        Console.WriteLine("*********************************************************");
                        Console.WriteLine("* Checking Top 100 urls for Country Code : " + countryCode);
                        Console.WriteLine("*********************************************************");

                        // must clear previous entries for this country beforehand
                        // DataAccessSecDNS.DeleteDomain("", countryCode);

                        for (int i = 0; i < 4; i++)
                        {
                            if (i > 0)
                                paginationString = ";" + i;
                            else
                                paginationString = string.Empty;

                            GetTopDomains(String.Format("http://www.alexa.com/topsites/countries{0}/{1}", paginationString, countryCode), out domains);

                            foreach (string whiteListDomain in domains.Keys)
                            {
                                try
                                {
                                    string ip = "";

                                    if (cachedIPs.ContainsKey(whiteListDomain))
                                    {
                                        ip = cachedIPs[whiteListDomain];
                                        Console.WriteLine("Using cached ip for domain '" + whiteListDomain + "'.");
                                    }
                                    else
                                    {
                                        ip = GetIP(whiteListDomain);
                                        cachedIPs.Add(whiteListDomain, ip);
                                        Console.WriteLine(String.Format("{2}. Evaluated IP '{0}' for domain '{1}'", ip, whiteListDomain, rank));
                                    }

                                    DataAccessSecDNS.SaveDomain(whiteListDomain, countryCode, rank, ip, string.Empty, string.Empty);

                                    // Exclusions
                                    AddExclusion(whiteListDomain, null, countryCode + "_Whitelist");

                                    rank++;
                                }
                                catch (Exception ex)
                                {
                                    Console.WriteLine(ex.ToString());
                                    Thread.Sleep(10000);
                                    continue;
                                }
                            }
                        }

                        Console.WriteLine(string.Format("Time taken to fetch current data is {0} seconds.", (DateTime.Now.Ticks - start) / TimeSpan.TicksPerSecond));
                        Console.WriteLine("*********************************************************");
                        Console.WriteLine();
                    }
                    catch (Exception ex) { Console.WriteLine("Exception " + ex.ToString()); }
                }
            }

            Console.WriteLine(string.Format("Time taken to fetch all whitelist data is {0} minutes.", (DateTime.Now.Ticks - globalStart) / TimeSpan.TicksPerMinute));
        }

        #endregion

        #region Utilities

        public HtmlNodeCollection GetHtmlNodes(string url, string xPathExpression)
        {
            /* HtmlNodeCollection htmlNodes = null;

             try
             {
                 WebRequest request = null;
                 WebResponse response = null;
                 Stream receiveStream = null;
                 Encoding encode = Encoding.GetEncoding("utf-8");

                 request = HttpWebRequest.Create(url);
                 response = request.GetResponse();
                 HtmlAgilityPack.HtmlDocument htmlDoc = new HtmlAgilityPack.HtmlDocument();

                 // Obtain a 'Stream' object associated with the response object.
                 receiveStream = response.GetResponseStream();

                 htmlDoc.Load(receiveStream); // The HtmlAgilityPack

                 if (htmlDoc.DocumentNode != null)
                 {
                     HtmlNode root = htmlDoc.DocumentNode;
                     htmlNodes = root.SelectNodes(xPathExpression);
                 }
             }
             catch (Exception ex) { throw; }

             return htmlNodes; */

            return GetHtmlNodesViaWebClient(url, xPathExpression);
        }

        public HtmlNodeCollection GetHtmlNodesViaWebClient(string url, string xPathExpression)
        {
            HtmlNodeCollection htmlNodes = null;

            using (WebClient client = new WebClient())
            {
                client.Headers["User-Agent"] = "Mozilla/4.0 (Compatible; Windows NT 5.1; MSIE 6.0)" +
                " (compatible; MSIE 6.0; Windows NT 5.1; .NET CLR 1.1.4322; .NET CLR 2.0.50727)";

                // Download data.
                byte[] arr = client.DownloadData(url);

                using (MemoryStream receiveStream = new MemoryStream(arr))
                {
                    HtmlAgilityPack.HtmlDocument htmlDoc = new HtmlAgilityPack.HtmlDocument();
                    htmlDoc.Load(receiveStream); // The HtmlAgilityPack

                    if (htmlDoc.DocumentNode != null)
                    {
                        HtmlNode root = htmlDoc.DocumentNode;
                        htmlNodes = root.SelectNodes(xPathExpression);
                    }
                }
            }

            return htmlNodes;
        }

        private static void GetTopDomains(string url, out  Dictionary<string, string> domains)
        {
            HtmlWeb htmlWeb = new HtmlWeb();
            HtmlDocument document = htmlWeb.Load(url);
            domains = new Dictionary<string, string>();

            HtmlNodeCollection urlNodes = document.DocumentNode.SelectNodes("//*[@class='small topsites-label']");

            if (urlNodes != null)
                foreach (HtmlNode urlNode in urlNodes)
                    if (!domains.ContainsKey(urlNode.InnerText.ToLower()))
                        domains.Add(urlNode.InnerText.ToLower(), "");
        }

        public string GetIP(string domain)
        {
            string xpathExpression = "//table//tr//td//font//b";
            string ipAddress = "";

            HtmlNodeCollection collection = null;
            string defaultIp = "";

            ipAddress = defaultIp;
            collection = GetHtmlNodes(String.Format("http://cgibin.erols.com/ziring/cgi-bin/nsgate/gate.pl?submit=Search+by+Name&mode=1&q={0}&qtype=A&.cgifields=persist&.cgifields=log", HttpUtility.UrlEncode(domain.Replace("http://", ""))), xpathExpression);

            if (collection != null)
            {
                foreach (HtmlNode node in collection)
                {
                    ipAddress = node.InnerText;

                    if (Regex.IsMatch(ipAddress, ipREGEXPattern, RegexOptions.IgnoreCase)
                        && !Regex.IsMatch(ipAddress, ".arpa", RegexOptions.IgnoreCase))
                    {
                        ipAddress = ipAddress.Replace("&nbsp;", "");
                        ipAddress = ipAddress.Replace("\n", "");
                        ipAddress = ipAddress.Trim();
                        break;
                    }
                    else
                        ipAddress = defaultIp;
                }
            }

            return ipAddress;
        }

        #endregion

        #region Exclusions

        public int AddExclusion(string hostname, string category, string source)
        {
            hostname = hostname.ToLower().Trim();
            string[] sections = hostname.Split('.');
            string domain = "";

            if (sections.Length >= 2)
                domain = sections[sections.Length - 2] + "." + sections[sections.Length - 1];

            return AddExclusion(hostname, domain, null, category, source);
        }

        public int AddExclusion(string hostname, string domain, string regex_pattern, string category, string source)
        {
            return DataAccessHtmlParser.AddExclusion(hostname, domain, regex_pattern, category, source);
        }

        public DataSet GetExclusions(string hostname, string domain, string category)
        {
            return DataAccessHtmlParser.GetExclusions(hostname, domain, category);
        }

        #endregion
    }
}
