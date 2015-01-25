namespace Core.Reputationals
{
    using System.Collections.Generic;
    using System.ComponentModel;
    using System.ComponentModel.DataAnnotations;
    using Categories;



    public enum ZoneType
    {
        REVERSE,
        DOMAIN,
        BOTH
    }

    public class Zone
    {
        [Required]
        [DisplayName("Name")]
        public string Name { get; set; }

        [Required]
        [DisplayName("Type")]
        public ZoneType Type { get; set; }

        public readonly List<ZoneResponse> Responses = new List<ZoneResponse>();



        public Zone()
        {
        }



        public Zone(string name, ZoneType type)
        {
            Name = name;
            Type = type;
        }



        internal void AddResponse(string response, Category category)
        {
            lock (Responses)
            {
                ZoneResponse resp = new ZoneResponse(response, category);
                //if (!Responses.Contains(resp)) 
                    Responses.Add(resp);
            }
        }
    }
}