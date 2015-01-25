<%@ Page Title="" Language="C#" MasterPageFile="~/Views/Shared/Site.Master" Inherits="System.Web.Mvc.ViewPage<Portal.Models.ResponsesZonesModel>" %>

<asp:Content ID="Content1" ContentPlaceHolderID="TitleContent" runat="server">
	responses
</asp:Content>

<asp:Content ID="Content2" ContentPlaceHolderID="MainContent" runat="server">
    <div>
        <fieldset>
            <legend>Manage responses</legend>
            <%= Model.ParsedHTML %>
         </fieldset>
    </div>
    <% using (Html.BeginForm("NewResponse", "Zones", FormMethod.Post)) { %>
            <%= Html.ValidationSummary(true, "Adding response failed") %>
            <div>
                <fieldset>
                    <legend>Add new response</legend>
                    
                    <div class="editor-label">
                        <%= Html.LabelFor(m => m.ResponseModel.ResponseIP) %>
                    </div>
                    <div class="editor-field">
                        <%= Html.TextBoxFor(m => m.ResponseModel.ResponseIP)%>
                        <%= Html.ValidationMessageFor(m => m.ResponseModel.ResponseIP)%>
                    </div>
                    
                    <div class="editor-label">
                        <%= Html.LabelFor(m => m.ResponseModel.CategoryName)%>
                    </div>
                    <div class="editor-field">
                        <%= Html.DropDownListFor(m => m.ResponseModel.CategoryName, Model.Categories)%>
                        <%= Html.ValidationMessageFor(m => m.ResponseModel.CategoryName)%>
                    </div>
                    <%= Html.HiddenFor(m => m.ResponseModel.ZoneName) %>
                    <p>
                        <input type="submit" value="Save" />
                    </p>
                </fieldset>
            </div>
        <% } %>
</asp:Content>
