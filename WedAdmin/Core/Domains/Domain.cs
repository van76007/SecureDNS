namespace Core.Domains
{
    using System;



    public enum DomainType
    {
        MALWARE,
        CAI,
        WHITELISTED
    }

    public class Domain
    {
        public string Name { get; set; }
        public DomainType Type { get; set; }
        public string Source { get; set; }
        public string Description { get; set; }
        public string TargetHost { get; set; }
        public string CategoryName { get; set; }
        public DateTime Created { get; set; }
        public DateTime Modified { get; set; }



        public Domain() {}

        public Domain(string name, DomainType type)
        {
            Name = name;
            Type = type;
        }
    }
}