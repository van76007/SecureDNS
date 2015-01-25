namespace Core.Categories
{
    public class Category
    {
        public string Name { get; set; }
        public string Description { get; set; }
        public string UnifiedResponse { get; set; }
        public string Notes { get; set; }


        public Category()
        {
        }



        public Category(string name, string unifiedResponse, string description)
        {
            Name = name;
            UnifiedResponse = unifiedResponse;
            Description = description;
        }
    }
}