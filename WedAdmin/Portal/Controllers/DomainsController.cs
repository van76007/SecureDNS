namespace Portal.Controllers
{
    using System;
    using System.Threading;
    using System.Web.Mvc;
    using Core.BlockedIps;
    using Core.Categories;
    using Core.Domains;
    using Models;



    [Authorize]
    public class DomainsController : Controller
    {
        public ActionResult Index()
        {
            return RedirectToAction("Search");
        }



        public ActionResult Diag()
        {
            return View();
        }



        [HttpGet]
        public ActionResult DiagResult(DomainDiagnosticModel model)
        {
            if (!string.IsNullOrEmpty(model.SearchString))
            {
                try
                {
                    new Thread(delegate() { model.BlockedRBLsCategories = CategoryServices.singleton.GetRBLsCategoriesByHostname(model.SearchString); }).Start();
                    new Thread(delegate() { model.BlockedMalwareCategories = CategoryServices.singleton.GetMalwareCategoriesByHostname(model.SearchString); }).Start();
                    new Thread(delegate() { model.MalwareDomains = DomainServices.singleton.GetByName(model.SearchString, DomainType.MALWARE); }).Start();
                    new Thread(delegate() { model.CAIDomains = DomainServices.singleton.GetByName(model.SearchString, DomainType.CAI); }).Start();
                    new Thread(delegate() { model.BlockedIPHistory = BlockedIPServices.singleton.GetBlockingHistory(model.SearchString); }).Start();
                    //new Thread(delegate() { model.HostInfo = DomainServices.singleton.GetHostEntryInfo(model.SearchString); }).Start();

                    model.HostInfo = DomainServices.singleton.GetHostEntryInfo(model.SearchString);
                    //new Thread(delegate() { model.BlockedIps = DomainServices.singleton.GetBlockedResult(model.HostInfo.AddressList); }).Start();

                    //int t = 0;

                    //while (model.HostInfo == null)
                    //{
                    //    if (t >= 10) break;

                    //    Thread.Sleep(100);
                    //    t++;
                    //}

                    if (model.HostInfo != null)
                    {
                        model.BlockedIps = DomainServices.singleton.GetBlockedResult(model.HostInfo.AddressList);
                    }
                    else
                    {
                        ModelState.AddModelError("HostInfo", "NsLookup did not manage to complete in time");
                    }

                    int y = 0;

                    //while (!model.DiagnosticsComplete)
                    //{
                    //    if (y >= 10) break;
                    //    Thread.Sleep(100);
                    //    y++;
                    //}
                }
                catch (Exception ex)
                {
                    ModelState.AddModelError("Error", ex.Message);
                }
            }

            return View("Diag", model);
        }


        [HttpGet]
        public ActionResult Search(string search)
        {
            DomainsSearchResultModel model = new DomainsSearchResultModel();

            if (!string.IsNullOrEmpty(search))
            {
                model.Search = search;

                if (ModelState.IsValid)
                {
                    model.Domains = DomainServices.singleton.GetSearchResult(search);
                }
            }

            return View(model);
        }



        [HttpGet]
        public ActionResult Delete(string domain, string source, string type, string search)
        {
            if (!string.IsNullOrEmpty(domain))
            {
                DomainServices.singleton.Delete(HttpContext.Server.HtmlDecode(domain), source, (DomainType) Enum.Parse(typeof (DomainType), type));
            }

            return RedirectToAction("Search", new {search});
        }



        public ActionResult New()
        {
            return View(new DomainModel());
        }



        [HttpPost]
        public ActionResult New(DomainModel model)
        {
            bool result = false;

            if (ModelState.IsValid)
            {
                switch ((DomainType) Enum.Parse(typeof (DomainType), model.Type))
                {
                    case DomainType.CAI:
                        result = DomainServices.singleton.SaveCAI(model.Name, model.Source);
                        break;

                    case DomainType.MALWARE:
                        result = DomainServices.singleton.SaveMalware(model.Name, model.Category, model.Source, model.Description, model.TargetHost);
                        break;

                    case DomainType.WHITELISTED:
                        result = DomainServices.singleton.SaveWhitelist(model.Name, "adm", model.Description, model.Source);
                        break;
                }

                TempData["message"] = result ? string.Format("New {0} added", model.Type) : string.Format("Failed adding {0}", model.Type);

                return RedirectToAction("New");
            }

            return View("New", model);
        }
    }
}