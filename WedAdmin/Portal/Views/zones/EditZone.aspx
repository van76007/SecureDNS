<%@ Page Title="" Language="C#" MasterPageFile="~/Views/Shared/Site.Master" Inherits="System.Web.Mvc.ViewPage<Core.Reputationals.Zone>" %>

<asp:Content ID="Content1" ContentPlaceHolderID="TitleContent" runat="server">
	EditZone
</asp:Content>

<asp:Content ID="Content2" ContentPlaceHolderID="MainContent" runat="server">

    <h2>EditZone</h2>
    <% using (Html.BeginForm()) { %>
        <%= Html.ValidationSummary(true, "Zone registration failed.") %>
        <div>
            <fieldset>
                <legend>Zone Information</legend>
                
                <div class="editor-label">
                    <%= Html.LabelFor(m => m.Name) %>
                </div>
                <div class="editor-field">
                    <%= Html.TextBoxFor(m => m.Name) %>
                    <%= Html.ValidationMessageFor(m => m.Name) %>
                </div>
                
                <div class="editor-label">
                    <%= Html.LabelFor(m => m.Type) %>
                </div>
                <div class="editor-field">
                    <%= Html.DropDownListFor(m => m.Type, new SelectList(Enum.GetValues(typeof(Core.Reputationals.ZoneType))))%>
                    <%= Html.ValidationMessageFor(m => m.Type) %>
                </div>
                <p>
                    <input type="submit" value="Save" />
                </p>
            </fieldset>
        </div>
    <% } %>
</asp:Content>
