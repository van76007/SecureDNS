namespace Core.Reputationals
{
    using System.ComponentModel.DataAnnotations;
    using Categories;



    public class ZoneResponse
    {
        [Required]
        public string ResponseIP { get; set; }
        [Required]
        public Category Category { get; set; }




        public ZoneResponse()
        {
        }



        public ZoneResponse(string responseIP, Category category)
        {
            ResponseIP = responseIP;
            Category = category;
        }
    }
}