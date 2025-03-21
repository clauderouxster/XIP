
<%@ taglib uri="/WEB-INF/struts-html.tld" prefix="html" %>
<%@ taglib uri="/WEB-INF/struts-bean.tld" prefix="bean" %>

<html:html>
<head>
   <title>Xerox Login</title>
   <link href="css/exercice.css" type="text/css" rel="stylesheet"/>
</head>
<bean:define id="loginLabel">
<bean:message key="label.login" />
</bean:define>
<bean:define id="resetLabel">
<bean:message key="label.reset" />
</bean:define>
<bean:parameter id="project" name="PROJECT" value="XRCE"/>  
<body>

<center> <!-- the form placed on center -->

<div class="upperleftshort">
  <div class="upperright">
     <div class="lowerleft">
        <div class="lowerright">
<table  cellspacing="2" cellpadding="2">
<!-- line for login -->
<html:form action="/doLogin" method="GET" target="center">
<html:hidden property="project" value="<%=project%>"/>
<tr><td align="center" class="title">
   <img src="./images/user.png" border="0" align="middle"/>   

<bean:message key="title.identification"/>
</td></tr>
<tr><td align="center" > 
  <table cellspacing="0" cellpadding="0">
     <tr><td class="label"><bean:message key="label.login"/></td>
         <td ><html:text property="login" value=""/></td>
    </tr>
    <!--- line for password -->
    <tr><td class="label"><bean:message key="label.password"/></td>
        <td ><html:password property="password" value=""/></td>
    </tr>
    </table>
    </td></tr>
<!-- line for buttons -->
<tr>
<td align="center" > 
    <html:submit value="Login" />
     &nbsp;&nbsp;
    <html:reset  value="<%=resetLabel%>" />
</td>
</tr>
</html:form>

</table>

</div></div></div></div>
</center>
<html:errors/>
</body>
</html:html>
