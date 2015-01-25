using System.Web.Mvc;



namespace Portal.Controllers
{
    using Core.BlockedIps;
    using Models;


    [Authorize]
    public class BlockedIpsController : Controller
    {
        public ActionResult Index()
        {
            return RedirectToAction("List");
        }



        public ActionResult List()
        {
            BlockedIPsModel model = new BlockedIPsModel
                                        {
                                            Ips = BlockedIPServices.singleton.GetAll()
                                        };

            return View(model);
        }



        public ActionResult New()
        {
            return View(new BlockedIPModel());
        }


        [HttpPost]
        public ActionResult Save(BlockedIPModel model)
        {
            if(ModelState.IsValid)
            {
                BlockedIPServices.singleton.Save(model.IP, model.Category, model.Reason, model.Source);
                return RedirectToAction("New");
            }

            return View("New", model);
        }



        public ActionResult Delete(string ip)
        {
            BlockedIPServices.singleton.Delete(ip);

            return RedirectToAction("List");
        }
    }
}