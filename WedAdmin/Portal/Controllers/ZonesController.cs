using System.Web.Mvc;



namespace Portal.Controllers
{
    using System.Collections.Generic;
    using System.Web.Routing;
    using Core.Categories;
    using Core.Reputationals;
    using Models;



    [Authorize]
    public class ZonesController : Controller
    {
        public ActionResult Index()
        {
            return RedirectToAction("List");
        }



        public ActionResult New()
        {
            return View();
        }



        [HttpPost]
        public ActionResult New(Zone zone)
        {
            if (ModelState.IsValid)
            {
                ZoneServices.Singleton.Save(zone.Name, zone.Type.ToString());
            }

            return View(zone);
        }



        [HttpGet]
        public ActionResult EditZone(string zoneName)
        {
            Zone zone = ZoneServices.Singleton.GetByName(zoneName);
            return View(zone);
        }



        [HttpPost]
        public ActionResult EditZone(Zone model)
        {
            ZoneServices.Singleton.Save(model.Name, model.Type.ToString());

            return View(model);
        }



        [HttpGet]
        public ActionResult Delete(string zoneName)
        {
            ZoneServices.Singleton.Delete(zoneName);

            return RedirectToAction("List");
        }



        public ActionResult List()
        {
            ListZonesModel model = new ListZonesModel
                                       {
                                           Zones = ZoneServices.Singleton.GetAll()
                                       };

            return View(model);
        }



        [HttpGet]
        public ActionResult Responses(string zoneName)
        {
            List<SelectListItem> list = new List<SelectListItem>();

            foreach (var c in CategoryServices.singleton.GetAll())
                list.Add(new SelectListItem {Text = c.Name, Value = c.Name});

            ResponsesZonesModel model = new ResponsesZonesModel
                                            {
                                                Zone = ZoneServices.Singleton.GetByName(zoneName),
                                                Categories = list,
                                                ResponseModel = new ResponseModel {ZoneName = zoneName}
                                            };

            return View(model);
        }



        [HttpPost]
        public ActionResult NewResponse(ResponsesZonesModel model)
        {
            if (ModelState.IsValid)
            {
                ZoneServices.Singleton.SaveResponse(model.ResponseModel.ZoneName, model.ResponseModel.ResponseIP, model.ResponseModel.CategoryName);
                return RedirectToAction("Responses", "Zones", new {model.ResponseModel.ZoneName});
            }

            List<SelectListItem> list = new List<SelectListItem>();

            foreach (var c in CategoryServices.singleton.GetAll())
                list.Add(new SelectListItem {Text = c.Name, Value = c.Name});

            model.Categories = list;

            return View("Responses", model);
        }



        [HttpGet]
        public ActionResult DeleteResponse(string responseIP, string zoneName)
        {
            ZoneServices.Singleton.DeleteResponse(responseIP, zoneName);

            return RedirectToAction("Responses", "Zones", new {zoneName});
        }



        [HttpGet]
        public ActionResult EditResponse(string zoneName, string responseIP)
        {
            ZoneResponse response = ZoneServices.Singleton.GetResponse(zoneName, responseIP);

            List<SelectListItem> list = new List<SelectListItem>();

            foreach (var c in CategoryServices.singleton.GetAll())
                list.Add(new SelectListItem {Text = c.Name, Value = c.Name});

            ResponseModel model = new ResponseModel();

            if (response != null)
            {
                model.CategoryName = response.Category.Name;
                model.ResponseIP = response.ResponseIP;
                model.ZoneName = zoneName;
                model.Categories = list;
            }

            return View(model);
        }



        [HttpPost]
        public ActionResult SaveResponse(ResponseModel response)
        {
            if (ModelState.IsValid)
            {
                ZoneServices.Singleton.SaveResponse(response.ZoneName, response.ResponseIP, response.CategoryName);
                return RedirectToAction("Responses", new {response.ZoneName});
            }

            List<SelectListItem> list = new List<SelectListItem>();

            foreach (var c in CategoryServices.singleton.GetAll())
                list.Add(new SelectListItem {Text = c.Name, Value = c.Name});

            if (response != null)
            {
                response.Categories = list;
            }

            return View("EditResponse", response);
        }
    }
}