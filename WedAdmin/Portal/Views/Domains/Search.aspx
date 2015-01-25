<%@ Page Title="" Language="C#" MasterPageFile="~/Views/Shared/Site.Master" Inherits="System.Web.Mvc.ViewPage<Portal.Models.DomainsSearchResultModel>" %>

<asp:Content ID="Content1" ContentPlaceHolderID="TitleContent" runat="server">
	Search
</asp:Content>

<asp:Content ID="Content2" ContentPlaceHolderID="MainContent" runat="server">
    <a href="New">Add new</a> | <a href="Diag">Run diagnostics</a>
    <% using (Html.BeginForm("Search", "Domains", FormMethod.Get)) { %>
        <%= Html.ValidationSummary(true, "Empty search criterias supplied.") %>
        <div>
            <fieldset>
                <legend>Search keywords</legend>
                
                <div class="editor-label">
                    <%= Html.LabelFor(m => m.Search) %>
                </div>
                <div class="editor-field">
                    <%= Html.TextBoxFor(m => m.Search) %>
                    <%= Html.ValidationMessageFor(m => m.Search) %>
                </div>
                <p>
                    <input type="submit" value="Search" />
                </p>
            </fieldset>
        </div>
    <% } %>
    <%= Model.ParsedHTML %>
</asp:Content>
