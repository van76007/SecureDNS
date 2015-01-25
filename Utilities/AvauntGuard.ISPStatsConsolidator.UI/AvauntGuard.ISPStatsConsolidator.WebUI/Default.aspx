<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="Default.aspx.cs" Inherits="AvauntGuard.ISPStatsConsolidator.WebUI._Default" %>

<%@ OutputCache Duration="3600" VaryByParam="None" %>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <title>Untitled Page</title>
    <style type="text/css">
        html
        {
            font-size: 0.75em;
        }
        body
        {
            /*font: 12px/1.2em arial,helvetica,verdana,helvetica-neue,sans-serif; */
            font-family: arial,helvetica,verdana,helvetica-neue,sans-serif;
            color: #6C8497;
            margin: 0;
            padding: 0; /* background: #F9F9F9;*/
        }
        p
        {
            line-height: 1.5;
        }
    </style>
</head>
<body>
    <!-- <%=DateTime.Now %> -->
    <form id="form1" runat="server">
    <div>
        <fieldset style="text-align: left; width: 162px; border-color: #FFFFFF; border-style: hidden;">
            <table cellpadding="0" cellspacing="0" border="0" width='160px'>
                <tr>
                    <td colspan='3'>
                        <b>Infected hosts</b>&nbsp;
                        <img src='./_images/help.png' height='10' width='10' style="vertical-align: middle;"
                            alt="Represents the average number of infected hosts per ISP, using the formula (Number of infected hosts for ISP / Global number of hosts for the ISP)."
                            title="Represents the average number of infected hosts per ISP, using the formula (Number of infected hosts for ISP / Global number of hosts for the ISP)" />
                    </td>
                </tr>
                <tr>
                    <td colspan='3' height='5'>
                    </td>
                </tr>
                <tr>
                    <td>
                        Orange (<%=GetHostCount("", "23889") %>
                        hosts)
                    </td>
                    <td>
                    </td>
                    <td>
                    </td>
                </tr>
                <tr>
                    <td colspan='3' height='3'>
                    </td>
                </tr>
                <tr>
                    <td>
                        <%=OrangeBarImage %>
                    </td>
                    <td>
                    </td>
                    <td>
                    </td>
                </tr>
                <tr>
                    <td colspan='3' height='3'>
                    </td>
                </tr>
                <tr>
                    <td>
                        Emtel (<%=GetHostCount("", "30999") %>
                        hosts)
                    </td>
                    <td>
                    </td>
                    <td>
                    </td>
                </tr>
                <tr>
                    <td colspan='3' height='3'>
                    </td>
                </tr>
                <tr>
                    <td>
                        <%=EmtelBarImage%>
                    </td>
                    <td>
                    </td>
                    <td>
                    </td>
                </tr>
                <tr>
                    <td colspan='3' height='15px'>
                    </td>
                </tr>
                <tr>
                    <td>
                        <b>Compromised websites</b>&nbsp;
                        <img src='./_images/help.png' height='9' width='9' style="vertical-align: middle;"
                            alt="Represents the average total number of compromised websites e.g. Sites victims of defacement, for all ISPs in Mauritius."
                            title="Represents the average total number of compromised websites e.g. Sites victims of defacement, for all ISPs in Mauritius." />
                    </td>
                    <td>
                    </td>
                    <td>
                    </td>
                </tr>
                <tr>
                    <td colspan='3' height='5'>
                    </td>
                </tr>
                <tr>
                    <td>
                        All ISPs (<%=(GetHostCount("defacement", "23889") + GetHostCount("defacement", "30999"))%>
                        hosts)
                    </td>
                    <td>
                    </td>
                    <td>
                    </td>
                </tr>
                <tr>
                    <td colspan='3' height='3'>
                    </td>
                </tr>
                <tr>
                    <td>
                        <%=DefacedBar%>
                    </td>
                    <td>
                    </td>
                    <td>
                    </td>
                </tr>
                <tr>
                    <td colspan='3' height='10px'>
                    </td>
                </tr>
            </table>
        </fieldset>
        <!-- Legend -->
        <fieldset style="text-align: left; width: 162px; border-color: #FFFFFF; border-style: hidden;">
            <table cellpadding="0" cellspacing="0" border="0" width='160px'>
                <tr>
                    <td>
                        <img src='./_images/help.png' height='9' width='9' />
                    </td>
                    <td width='10'>
                    </td>
                    <td>
                        Hover for help tooltip
                    </td>
                </tr>
                <tr>
                    <td colspan='3'>
                    </td>
                </tr>
                <tr>
                    <td>
                        &nbsp;<img src='./_images/level1.png' height='6' width='6' />
                    </td>
                    <td width='10'>
                    </td>
                    <td>
                        Low threat level
                    </td>
                </tr>
                <tr>
                    <td colspan='3' height='1px'>
                    </td>
                </tr>
                <tr>
                    <td>
                        &nbsp;<img src='./_images/level2.png' height='6' width='6' />
                    </td>
                    <td>
                    </td>
                    <td>
                        Medium threat level
                    </td>
                </tr>
                <tr>
                    <td colspan='3' height='1px'>
                    </td>
                </tr>
                <tr>
                    <td>
                        &nbsp;<img src='./_images/level3.png' height='6' width='6' />
                    </td>
                    <td>
                    </td>
                    <td>
                        High threat level
                    </td>
                </tr>
                <tr>
                    <td colspan='3' height='1px'>
                    </td>
                </tr>
                <tr>
                    <td>
                        &nbsp;<img src='./_images/level4.png' height='6' width='6' />
                    </td>
                    <td>
                    </td>
                    <td>
                        Very high threat level
                    </td>
                </tr>
                <tr>
                    <td colspan='3' height='1px'>
                    </td>
                </tr>
                <tr>
                    <td>
                        &nbsp;<img src='./_images/level5.png' height='6' width='6' />
                    </td>
                    <td>
                    </td>
                    <td>
                        Critical threat level
                    </td>
                </tr>
            </table>
        </fieldset>
    </div>
    </form>
</body>
</html>
