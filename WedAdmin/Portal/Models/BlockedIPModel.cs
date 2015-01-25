namespace Portal.Models
{
    using System.Collections.Generic;
    using System.ComponentModel.DataAnnotations;
    using System.Text;
    using System.Web.Mvc;
    using Core.BlockedIps;
    using Core.Categories;



    public class BlockedIPModel
    {
        [Required]
        [RegularExpression(@"\b(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\b", ErrorMessage = @"Invalid IP")]
        public string IP { get; set; }

        [Required]
        public string Category { get; set; }

        public string Reason { get; set; }
        public string Source { get; set; }

        public IEnumerable<SelectListItem> Categories
        {
            get
            {
                List<SelectListItem> categories = new List<SelectListItem>();

                foreach (var c in CategoryServices.singleton.GetAll())
                    categories.Add(new SelectListItem { Text = c.Name, Value = c.Name });

                return categories;
            }
        }
    }

    public class BlockedIPsModel
    {
        public List<BlockedIp> Ips { get; set; }

        public string ParsedHTML
        {
            get
            {
                StringBuilder sb = new StringBuilder();

                //sb.Append("<table>");
                //sb.AppendFormat("<tr><th colspan='5'>{0}</th></tr>", "Blocked IPs");
                //sb.AppendFormat("<tr class='title'><td>{0}</td><td>{1}</td><td>{2}</td><td>{3}</td><td>{4}</td></tr>", "IP", "Reason", "Category", "Source", "Actions");

                foreach (var ip in Ips)
                    sb.AppendFormat("<tr><td>{0}</td><td>{1}</td><td>{2}</td><td>{3}</td><td><a href='Delete?ip={0}' onclick=\"javascript:return confirm('Are you sure?');\">Delete</a></td></tr>", ip.IP, ip.Reason, ip.Category, ip.Source);

                //sb.Append("</table>");
                
                return sb.ToString();
            }
        }
    }
}
