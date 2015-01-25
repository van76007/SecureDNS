namespace Core.Categories
{
    using System.Collections.Generic;



    public class CategoryServices
    {
        private CategoryServices()
        {
        }



        public static readonly CategoryServices singleton = new CategoryServices();



        public bool Save(string name, string unifiedResponse, string description)
        {
            return CategoryData.Save(name, unifiedResponse, description);
        }



        public bool Delete(string name)
        {
            return CategoryData.DeleteByName(name);
        }



        public Category GetByName(string name)
        {
            return CategoryData.GetByName(name);
        }



        public List<Category> GetAll()
        {
            return CategoryData.GetAll();
        }



        public List<Category> GetRBLsCategoriesByHostname(string hostname)
        {
            return CategoryData.GetRBLsCategoriesByHostname(hostname);
        }



        public List<Category> GetMalwareCategoriesByHostname(string hostname)
        {
            return CategoryData.GetMalwareCategoriesByHostname(hostname);
        }
    }
}