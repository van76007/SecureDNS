<%@ Page Title="" Language="C#" MasterPageFile="~/Views/Shared/Site.Master" Inherits="System.Web.Mvc.ViewPage<Portal.Models.BlockedIPsModel>" %>

<asp:Content ID="Content1" ContentPlaceHolderID="TitleContent" runat="server">
	Blocked ips list
</asp:Content>

<asp:Content ID="Content2" ContentPlaceHolderID="headContent" runat="server">
    <link href="../../Content/pagination.css" rel="stylesheet" type="text/css"/>
    <script src="../../Scripts/jquery-1.5.1.min.js" type="text/javascript"></script>
    <script src="../../Scripts/jquery.pagination.js" type="text/javascript"></script>
    
    <script type="text/javascript">
        /*function pageselectCallback(page_index, jq) {
            var items_per_page = 100;
            var num_entries = jQuery('#hiddenresult tr.prow').length;
            var newcontent = ($('#hiddenresult tr.prow').slice(Math.min(page_index * items_per_page), ((page_index + 1) * items_per_page))).clone();

            // Replace old content with new content
            $('#blockedIps tbody').html(newcontent);

            return false;
        }*/

        /** 
        * Initialisation function for pagination
        */
        /*
        function initPagination() {
            var num_entries = jQuery('#hiddenresult tr.prow').length;

            $("#Pagination").pagination(num_entries, {
                current_page: 0,
                items_per_page: 100,
                num_display_entries: 5,
                next_text: 'Next',
                prev_text: 'Prev',
                callback: pageselectCallback,
                num_edge_entries: 1,
                load_first_page: true
            });
        }

        // When document is ready, initialize pagination
        $(document).ready(function() {
            initPagination();
        });*/

        jQuery(function($) {
            $("#filter").keyup(function() {
                var filter = $(this).val(), count = 0;

                $("#blockedIps tbody tr").each(function() {
                    if ($(this).find('td:nth-child(1)').text().search(new RegExp(filter, "i")) < 0) {
                        $(this).addClass('hidden');
                    } else {
                        $(this).removeClass('hidden');
                        count++;
                    }
                });
                $("#filter-count").text(count);
            });
        });
       </script>
</asp:Content>

<asp:Content ID="Content3" ContentPlaceHolderID="MainContent" runat="server">
    <a href="New">Add new</a>    
    <div>
        <fieldset><legend>Blocked ips list</legend>
            <input id="filter" name="filter" size="40"/><span id="filter-count"></span>
            <table id="blockedIps" class="fullWidth">
                <thead><tr><th>IP</th><th>Reason</th><th>Category</th><th>Source</th><th>Actions</th></tr></thead>
                <tbody>
                    <%=Model.ParsedHTML%>
                </tbody>
            </table>
        </fieldset>
    </div>
</asp:Content>
