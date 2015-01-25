<%@ Page Title="" Language="C#" MasterPageFile="~/Views/Shared/Site.Master" Inherits="System.Web.Mvc.ViewPage<Portal.Models.DomainModel>" %>

<asp:Content ID="Content1" ContentPlaceHolderID="TitleContent" runat="server">
	New
</asp:Content>

<asp:Content ID="Content2" ContentPlaceHolderID="MainContent" runat="server">
<a href="Search">Search</a> | <a href="Diag">Run diagnostics</a>
<%
    using (Html.BeginForm("New", "Domains"))
    {%>
        <%=Html.ValidationSummary(true, "Failed to register domain")%>
        
        <% if (Model != null && Model.Message != null)
           { 
         %>
         <%= Model.Message %>
         <%
           } 
        %>
        
        <div>
            <fieldset>
                <legend>New domain information</legend>
                <div class="editor-label">
                    <%=Html.LabelFor(m => m.Name)%>
                </div>
                <div class="editor-field">
                    <%=Html.TextBoxFor(m => m.Name)%>
                    <%=Html.ValidationMessageFor(m => m.Name)%>
                </div>
                
                <div class="editor-label">
                    <%=Html.LabelFor(m => m.Source)%>
                </div>
                <div class="editor-field">
                    <%=Html.TextBoxFor(m => m.Source)%>
                    <%=Html.ValidationMessageFor(m => m.Source)%>
                </div>
                
                <div class="editor-label">
                    <%=Html.LabelFor(m => m.TargetHost)%>
                </div>
                <div class="editor-field">
                    <%=Html.TextBoxFor(m => m.TargetHost)%>
                    <%=Html.ValidationMessageFor(m => m.TargetHost)%>
                </div>
                
                <div class="editor-label">
                        <%=Html.LabelFor(m => m.Type)%>
                    </div>
                    <div class="editor-field">
                        <%=Html.DropDownListFor(m => m.Type, Model.Types)%>
                        <%=Html.ValidationMessageFor(m => m.Type)%>
                    </div>
                
                <div class="editor-label">
                        <%=Html.LabelFor(m => m.Category)%>
                    </div>
                    <div class="editor-field">
                        <%=Html.DropDownListFor(m => m.Category, Model.Categories)%>
                        <%=Html.ValidationMessageFor(m => m.Category)%>
                    </div>
                
                
                <div class="editor-label">
                    <%=Html.LabelFor(m => m.Description)%>
                </div>
                <div class="editor-field">
                    <%=Html.TextAreaFor(m => m.Description)%>
                    <%=Html.ValidationMessageFor(m => m.Description)%>
                </div>
                
                <p>
                    <input type="submit" value="Save" />
                </p>
            </fieldset>
        </div>
    <%
    }%>
</asp:Content>
