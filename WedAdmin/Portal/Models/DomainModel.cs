namespace Portal.Models
{
    using System;
    using System.Collections.Generic;
    using System.ComponentModel;
    using System.ComponentModel.DataAnnotations;
    using System.Net;
    using System.Text;
    using System.Web;
    using System.Web.Mvc;
    using Core.BlockedIps;
    using Core.Categories;
    using Core.Domains;



    public class DomainModel
    {
        [Required]
        [RegularExpression(@"^(?#Protocol)(?:(?:ht|f)tp(?:s?)\:\/\/|~\/|\/)?(?#Username:Password)(?:\w+:\w+@)?(?#Subdomains)(?:(?:[-\w]+\.)+(?#TopLevel Domains)(?:com|org|net|gov|mil|biz|info|mobi|name|aero|jobs|museum|travel|[a-z]{2}))(?#Port)(?::[\d]{1,5})?(?#Directories)(?:(?:(?:\/(?:[-\w~!$+|.,=]|%[a-f\d]{2})+)+|\/)+|\?|#)?(?#Query)(?:(?:\?(?:[-\w~!$+|.,*:]|%[a-f\d{2}])+=?(?:[-\w~!$+|.,*:=]|%[a-f\d]{2})*)(?:&(?:[-\w~!$+|.,*:]|%[a-f\d{2}])+=?(?:[-\w~!$+|.,*:=]|%[a-f\d]{2})*)*)*(?#Anchor)(?:#(?:[-\w~!$+|.,*:=]|%[a-f\d]{2})*)?$", ErrorMessage = @"Invalid hostname")]
        public string Name { get; set; }
        public string Type { get; set; }
        [Required]
        public string Source { get; set; }
        public string Description { get; set; }
        public string TargetHost { get; set; }
        [Required]
        public string Category { get; set; }
        public string Message { get; set; }

        public IEnumerable<SelectListItem> Types
        {
            get
            {
                List<SelectListItem> types = new List<SelectListItem>();

                foreach (DomainType t in Enum.GetValues(typeof (DomainType)))
                {
                    DomainType type = t;
                    types.Add(new SelectListItem {Text = type.ToString(), Value = type.ToString()});
                }

                return types;
            }
        }

        public IEnumerable<SelectListItem> Categories
        {
            get
            {
                List<SelectListItem> categories = new List<SelectListItem>();

                foreach (var c in CategoryServices.singleton.GetAll())
                    categories.Add(new SelectListItem {Text = c.Name, Value = c.Name});

                return categories;
            }
        }
    }


    public class DomainDiagnosticModel
    {
        [Required]
        [RegularExpression(@"^(?#Protocol)(?:(?:ht|f)tp(?:s?)\:\/\/|~\/|\/)?(?#Username:Password)(?:\w+:\w+@)?(?#Subdomains)(?:(?:[-\w]+\.)+(?#TopLevel Domains)(?:com|org|net|gov|mil|biz|info|mobi|name|aero|jobs|museum|travel|[a-z]{2}))(?#Port)(?::[\d]{1,5})?(?#Directories)(?:(?:(?:\/(?:[-\w~!$+|.,=]|%[a-f\d]{2})+)+|\/)+|\?|#)?(?#Query)(?:(?:\?(?:[-\w~!$+|.,*:]|%[a-f\d{2}])+=?(?:[-\w~!$+|.,*:=]|%[a-f\d]{2})*)(?:&(?:[-\w~!$+|.,*:]|%[a-f\d{2}])+=?(?:[-\w~!$+|.,*:=]|%[a-f\d]{2})*)*)*(?#Anchor)(?:#(?:[-\w~!$+|.,*:=]|%[a-f\d]{2})*)?$", ErrorMessage = @"Invalid hostname")]
        [DisplayName("Search string")]
        public string SearchString { get; set; }

        [RegularExpression(@"\b(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\b", ErrorMessage = @"Invalid IP")]
        public string DNSServerIP { get; set; }

        public List<Category> BlockedMalwareCategories { get; set; }
        public List<Category> BlockedRBLsCategories { get; set; }
        public List<Domain> MalwareDomains { get; set; }
        public List<Domain> CAIDomains { get; set; }
        public Dictionary<string, BlockedIp> BlockedIps { get; set; }
        public List<BlockedIp> BlockedIPHistory { get; set; }
        public IPHostEntry HostInfo { get; set; }
        public bool DiagnosticsComplete
        {
            get { return BlockedMalwareCategories != null && BlockedRBLsCategories != null && MalwareDomains != null && CAIDomains != null && BlockedIps != null && HostInfo != null; }
        }

        public string ParsedHTML
        {
            get
            {
                StringBuilder sb = new StringBuilder();

                sb.Append("<table>");
                sb.AppendFormat("<tr><th colspan='4'>{0}</th></tr>", "Malware blocking history");

                if (BlockedMalwareCategories != null && BlockedMalwareCategories.Count > 0)
                {
                    sb.AppendFormat("<tr class='title'><td>{0}</td><td>{1}</td><td>{2}</td><td>{3}</td></tr>", "Category", "Description", "Response", "Notes");

                    foreach (var c in BlockedMalwareCategories)
                    {
                        sb.AppendFormat("<tr><td>{0}</td><td>{1}</td><td>{2}</td><td>{3}</td></tr>", c.Name, c.Description, c.UnifiedResponse, c.Notes);
                    }
                }
                else
                {
                    sb.AppendFormat("<tr><td colpsan='4'>{0}</td></tr>", "No blocking history found");
                }

                sb.Append("</table>");
                sb.Append("<hr />");
                sb.Append("<table>");
                sb.AppendFormat("<tr><th colspan='4'>{0}</th></tr>", "Reputational blocking history");

                if (BlockedRBLsCategories != null && BlockedRBLsCategories.Count > 0)
                {
                    sb.AppendFormat("<tr class='title'><td>{0}</td><td>{1}</td><td>{2}</td><td>{3}</td></tr>", "hostname", "Description", "Response", "Notes");

                    foreach (var r in BlockedRBLsCategories)
                    {
                        sb.AppendFormat("<tr><td>{0}</td><td>{1}</td><td>{2}</td><td>{3}</td></tr>", r.Name, r.Description, r.UnifiedResponse, r.Notes);
                    }
                }
                else
                {
                    sb.AppendFormat("<tr><td colpsan='4'>{0}</td></tr>", "No blocking history found");
                }

                sb.Append("</table>");

                sb.Append("<hr />");

                sb.Append("<table>");
                sb.AppendFormat("<tr><th colspan='4'>{0}</th></tr>", "Malware database entries");

                if (MalwareDomains != null && MalwareDomains.Count > 0)
                {
                    sb.AppendFormat("<tr class='title'><td>{0}</td><td>{1}</td><td>{2}</td><td>{3}</td></tr>", "hostname", "Description", "Category", "Source");
                    foreach (var d in MalwareDomains)
                    {
                        sb.AppendFormat("<tr><td>{0}</td><td>{1}</td><td>{2}</td><td>{3}</td></tr>", d.Name, d.Description, d.CategoryName, d.Source);
                    }
                }
                else
                {
                    sb.AppendFormat("<tr><td colpsan='4'>{0}</td></tr>", "No malware database entries found");
                }

                sb.Append("</table>");

                sb.Append("<hr />");

                sb.Append("<table>");
                sb.AppendFormat("<tr><th>{0}</th></tr>", "CAI database entries");

                if (CAIDomains != null && CAIDomains.Count > 0)
                {
                    sb.AppendFormat("<tr class='title'><td>{0}</td></tr>", "Source");
                    foreach (var s in CAIDomains)
                    {
                        sb.AppendFormat("<tr><td>{0}</td></tr>", s.Source);
                    }
                }
                else
                {
                    sb.AppendFormat("<tr><td>{0}</td></tr>", "No CAI database entries found");
                }

                sb.Append("</table>");

                sb.Append("<hr />");

                sb.Append("<table>");
                sb.AppendFormat("<tr><th colspan='4'>{0}</th></tr>", "NSLookup");

                if (BlockedIps != null && BlockedIps.Count > 0)
                {
                    sb.AppendFormat("<tr class='title'><td>{0}</td><td>{1}</td><td>{2}</td><td>{3}</td></tr>", "IP", "Reason", "Category", "Source");

                    foreach (var kvp in BlockedIps)
                    {
                        if (kvp.Value == null)
                        {
                            sb.AppendFormat("<tr><td>{0}</td><td colspan='3'>{1}</td></tr>", kvp.Key, "Not blocked");
                        }
                        else
                        {
                            sb.AppendFormat("<tr><td>{0}</td><td>{1}</td><td>{2}</td><td>{3}</td></tr>", kvp.Key, kvp.Value.Reason, kvp.Value.Category, kvp.Value.Source);
                        }
                    }
                }

                sb.Append("</table>");

                sb.Append("<hr />");

                sb.Append("<table>");
                sb.AppendFormat("<tr><th colspan='4'>{0}</th></tr>", "Blocking IP history");

                if (BlockedIPHistory != null && BlockedIPHistory.Count > 0)
                {
                    sb.AppendFormat("<tr class='title'><td>{0}</td><td>{1}</td><td>{2}</td><td>{3}</td></tr>", "IP", "Reason", "Category", "Source");

                    foreach (var ip in BlockedIPHistory)
                    {
                        sb.AppendFormat("<tr><td>{0}</td><td>{1}</td><td>{2}</td><td>{3}</td></tr>", ip.IP, ip.Reason, ip.Category, ip.Source);
                    }
                }

                sb.Append("</table>");

                if (HostInfo != null)
                {
                    sb.Append("<hr />");
                    sb.Append("<table>");
                    sb.AppendFormat("<tr><th colspan='2'>{0}</th></tr>", "Host info");

                    if (HostInfo.Aliases.Length > 0)
                    {
                        sb.Append("<tr><td>Aliases: </td><td>");

                        foreach (var a in HostInfo.Aliases)
                        {
                            sb.AppendFormat(" [{0}] ", a);
                        }

                        sb.Append("</td></tr>");
                    }

                    sb.AppendFormat("<tr><td>Hostname: </td><td>{0}</td></tr>", HostInfo.HostName);

                    sb.Append("</table>");
                }

                return sb.ToString();
            }
        }
    }


    public class DomainsSearchResultModel
    {
        [Required]
        public string Search { get; set; }
        public Dictionary<DomainType, List<Domain>> Domains { get; set; }

        public string ParsedHTML
        {
            get
            {
                StringBuilder sb = new StringBuilder();

                if (Domains != null)
                {
                    foreach (var s in Domains)
                    {
                        if (s.Value.Count <= 0) continue;

                        sb.AppendFormat("<div><fieldset><legend>{0}</legend>", s.Key);

                        sb.Append("<table class='fullWidth'>");
                        sb.AppendFormat("<tr><th>Name</th><th>Category</th><th>Description</th><th>Target Host</th><th>Source</th><th>Added</th><th>Modified</th><th>Actions</th></tr>");

                        foreach (Domain d in Domains[s.Key])
                        {
                            sb.AppendFormat("<tr><td>{0}</td><td>{1}</td><td>{2}</td><td>{3}</td><td>{4}</td><td>{5}</td><td>{6}</td><td><a href=\"{7}\" onclick=\"javascript:return confirm('Are you sure?');\">Delete</a></td></tr>", d.Name, d.CategoryName, d.Description, d.TargetHost, d.Source, GetDateLiteral(d.Created), GetDateLiteral(d.Modified), HttpContext.Current.Server.HtmlEncode("Delete?domain=" + d.Name + "&source=" + d.Source + "&type=" + d.Type + "&search=" + Search));
                        }

                        sb.AppendFormat("</table>");
                        sb.Append("</fieldset></div>");
                    }
                }

                return sb.ToString();
            }
        }



        private static string GetDateLiteral(DateTime dateTime)
        {
            return dateTime != DateTime.MinValue ? dateTime.ToString("dd-MM-yyyy") : "";
        }
    }
}