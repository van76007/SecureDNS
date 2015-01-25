<%@ Page Title="" Language="C#" MasterPageFile="~/Views/Shared/Site.Master" Inherits="System.Web.Mvc.ViewPage<Portal.Models.ResponseModel>" %>

<asp:Content ID="Content1" ContentPlaceHolderID="TitleContent" runat="server">
	Edit Response
</asp:Content>

<asp:Content ID="Content2" ContentPlaceHolderID="MainContent" runat="server">

    <h2>Edit Response</h2>

    <% using (Html.BeginForm("SaveResponse", "Zones", FormMethod.Post)) { %>
            <%= Html.ValidationSummary(true, "Saving response failed") %>
            <div>
                <fieldset>
                    <legend>Editing response</legend>
                    
                    <div class="editor-label">
                        <%= Html.LabelFor(m => m.ResponseIP) %>
                    </div>
                    <div class="editor-field">
                        <%= Html.TextBoxFor(m => m.ResponseIP, new { @disabled = "disabled" })%>
                        <%= Html.ValidationMessageFor(m => m.ResponseIP)%>
                    </div>
                    
                    <div class="editor-label">
                        <%= Html.LabelFor(m => m.CategoryName)%>
                    </div>
                    <div class="editor-field">
                        <%= Html.DropDownListFor(m => m.CategoryName, Model.Categories)%>
                        <%= Html.ValidationMessageFor(m => m.CategoryName)%>
                    </div>
                    <%= Html.HiddenFor(m => m.ZoneName) %>
                    <p>
                        <input type="submit" value="Save" />
                    </p>
                </fieldset>
            </div>
        <% } %>
</asp:Content>
