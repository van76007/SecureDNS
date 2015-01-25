<%@ Page Title="" Language="C#" MasterPageFile="~/Views/Shared/Site.Master" Inherits="System.Web.Mvc.ViewPage<Portal.Models.BlockedIPModel>" %>

<asp:Content ID="Content1" ContentPlaceHolderID="TitleContent" runat="server">
	Added new blocked IP
</asp:Content>

<asp:Content ID="Content2" ContentPlaceHolderID="MainContent" runat="server">

    <h2>Add new blocked IP</h2>
    <% using (Html.BeginForm("Save", "BlockedIps")) { %>
        <%= Html.ValidationSummary(true, "Adding blocked IP failed.") %>
        <div>
            <fieldset>
                <legend>IP</legend>
                
                <div class="editor-label">
                    <%= Html.LabelFor(m => m.IP) %>
                </div>
                <div class="editor-field">
                    <%= Html.TextBoxFor(m => m.IP) %>
                    <%= Html.ValidationMessageFor(m => m.IP) %>
                </div>
                
                 <div class="editor-label">
                        <%=Html.LabelFor(m => m.Category)%>
                    </div>
                    <div class="editor-field">
                        <%=Html.DropDownListFor(m => m.Category, Model.Categories)%>
                        <%=Html.ValidationMessageFor(m => m.Category)%>
                    </div>
                
                <div class="editor-label">
                    <%= Html.LabelFor(m => m.Reason) %>
                </div>
                <div class="editor-field">
                    <%= Html.TextAreaFor(m => m.Reason) %>
                    <%= Html.ValidationMessageFor(m => m.Reason) %>
                </div>
                <div class="editor-label">
                    <%= Html.LabelFor(m => m.Source) %>
                </div>
                <div class="editor-field">
                    <%= Html.TextBoxFor(m => m.Source) %>
                    <%= Html.ValidationMessageFor(m => m.Source) %>
                </div>
                <p>
                    <input type="submit" value="Save" />
                </p>
            </fieldset>
        </div>
    <% } %>

</asp:Content>
