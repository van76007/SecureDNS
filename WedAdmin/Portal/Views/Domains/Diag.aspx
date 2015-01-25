<%@ Page Title="" Language="C#" MasterPageFile="~/Views/Shared/Site.Master" Inherits="System.Web.Mvc.ViewPage<Portal.Models.DomainDiagnosticModel>" %>

<asp:Content ID="Content1" ContentPlaceHolderID="TitleContent" runat="server">
	Diag
</asp:Content>

<asp:Content ID="Content2" ContentPlaceHolderID="MainContent" runat="server">
        <a href="New">Add new</a> | <a href="Search">Search</a>
        <%=Html.ValidationSummary() %>
        <% using (Html.BeginForm("DiagResult", "Domains", FormMethod.Get)) { %>
        <div>
            <fieldset>
                <legend>Domain diagnostics</legend>
                <div class="editor-label">
                    <%= Html.LabelFor(m => m.SearchString) %>
                </div>
                <div class="editor-field">
                    <%= Html.TextBoxFor(m => m.SearchString) %>
                    <%= Html.ValidationMessageFor(m => m.SearchString) %>
                </div>
                <p>
                    <input type="submit" value="Run diagnostic" />
                </p>
                
                <% 
        if (Model != null)
         {
             Response.Write(Model.ParsedHTML);
         } 
        %>
                
            </fieldset>
        </div>
    <% } %>
</asp:Content>
