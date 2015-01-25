namespace Core.Reputationals
{
    using System.Collections.Generic;
    using Categories;



    public class ZoneServices
    {
        private ZoneServices()
        {
        }



        public static ZoneServices Singleton = new ZoneServices();



        public bool Save(string zoneName, string type)
        {
            return ZoneData.Save(zoneName, type);
        }



        public bool Delete(string zoneName)
        {
            return ZoneData.DeleteByName(zoneName);
        }



        public Zone GetByName(string zoneName)
        {
            return ZoneData.GetByName(zoneName);
        }



        public List<Zone> GetAll()
        {
            return ZoneData.GetAll();
        }



        public bool SaveResponse(string zoneName, string responseIP, string categoryName)
        {
            return ZoneData.SaveResponse(zoneName, categoryName, responseIP);
        }



        public Zone AttachResponse(Zone zone, string response, Category category)
        {
            zone.AddResponse(response, category);

            return zone;
        }



        public ZoneResponse GetResponse(string zoneName, string response)
        {
            return ZoneData.GetResponse(zoneName, response);
        }



        public bool DeleteResponse(string response, string zoneName)
        {
            return ZoneData.DeleteResponse(zoneName, response);
        }
    }
}