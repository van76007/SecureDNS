<%@ Page Title="" Language="C#" MasterPageFile="~/Views/Shared/Site.Master" Inherits="System.Web.Mvc.ViewPage<Portal.Models.ListZonesModel>" %>

<asp:Content ID="Content1" ContentPlaceHolderID="TitleContent" runat="server">
	list
</asp:Content>

<asp:Content ID="Content2" ContentPlaceHolderID="MainContent" runat="server">

    <a href="New">Add new</a>    
    <div>
        <fieldset>
            <legend>RBLs List</legend>
            <%= Model.ParsedHTML %>
        </fieldset>
    </div>    
</asp:Content>
