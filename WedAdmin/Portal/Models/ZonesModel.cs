namespace Portal.Models
{
    using System.Collections.Generic;
    using System.ComponentModel.DataAnnotations;
    using System.Text;
    using System.Web.Mvc;
    using Core.Reputationals;



    public class ResponseModel
    {
        [Required]
        [RegularExpression(@"\b(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\b", ErrorMessage = @"Invalid IP")]
        public string ResponseIP { get; set; }
        [Required]
        public string CategoryName { get; set; }
        [Required]
        public string ZoneName { get; set; }

        public IEnumerable<SelectListItem> Categories { get; set; }
    }

    public class ResponsesZonesModel
    {
        public Zone Zone { get; set; }

        [DataType(DataType.Html)]
        public string ParsedHTML
        {
            get
            {
                StringBuilder sb = new StringBuilder();

                if (Zone != null)
                {
                    foreach (var s in Zone.Responses)
                    {
                        sb.AppendFormat("{0} - {1} <a href='EditResponse?zoneName={2}&responseIP={0}'>edit</a> | <a href='DeleteResponse?zoneName={2}&responseIP={0}' onclick=\"javascript:return confirm('Are you sure?');\">delete</a><br />", s.ResponseIP, s.Category.Name, Zone.Name);
                    }
                }

                return sb.ToString();
            }
        }

        public IEnumerable<SelectListItem> Categories { get; set; }
        public ResponseModel ResponseModel { get; set; }
    }


    public class ListZonesModel
    {
        public List<Zone> Zones { get; set; }

        [DataType(DataType.Html)]
        public string ParsedHTML
        {
            get
            {
                StringBuilder sb = new StringBuilder();

                if(Zones != null)
                {
                    foreach (var z in Zones)
                    {
                        sb.AppendFormat("{0}{1} <a href='{2}'>manage responses</a> | <a href='EditZone?zoneName={0}'>edit</a> | <a href='Delete?zoneName={0}'>delete</a> <br />", z.Name, z.Type, "Responses?zoneName=" + z.Name);
                    }
                }

                return sb.ToString();
            }
        }
    }
}