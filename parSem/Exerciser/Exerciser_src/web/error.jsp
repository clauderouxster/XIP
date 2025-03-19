<%@ taglib uri="/WEB-INF/struts-html.tld" prefix="html" %>
<%@ taglib uri="/WEB-INF/struts-bean.tld" prefix="bean" %>
<%@ taglib uri="/WEB-INF/struts-logic.tld" prefix="logic" %>
<%@ page isErrorPage="true"%>
<%@ page import="com.xerox.xrce.exills.*" %>
<%@ page import="org.apache.struts.*" %>
<%@ page import="org.apache.struts.action.*" %>
<html:html>
<head>
   <title>Xerox Error page</title>
   <link href="css/exercice.css" type="text/css" rel="stylesheet"/>
</head>

<body>

<center> <!-- the form placed on center -->

<div class="error_upperleft">
  <div class="error_upperright">
     <div class="error_lowerleft">
        <div class="error_lowerright">
          <table cellsapcing="0" cellpadding="2">             
              <tr><td align="center" class="error_title">
                  <bean:message key="title.error"/>
                  &nbsp; <html:link href="mailto:exillsort-techsupport@xrce.xerox.com" title="send an email">
                           exillsort-techsupport@xrce.xerox.com
                         </html:link>    
        
              </td></tr>
              <logic:present name="<%=Constants.ERRORS%>" scope="session">
               <logic:iterate id="errorElement" name="<%=Constants.ERRORS%>" scope="session" type="ErrorObject">
                 <tr><td>
                     <bean:write name="errorElement" property="name"/><br>
                     <bean:write name="errorElement" property="exception"/><br>
                     <bean:define id="exceptionID" name="errorElement" type="java.lang.Exception"  property="exception"/>
                      <ul>
                      <% 
                           StackTraceElement[] stack = exceptionID.getStackTrace();
                           for (int i=0;i < Constants.DEBUG_LEVEL;i++ ) {
                       %>
                           <li><i><%=stack[i]%></i></li>
                      <%}%>
                      </ul>
                     <bean:write name="exceptionID" property="localizedMessage" ignore="true"/>                         
                 </tr></td>
              </logic:iterate>
              </logic:present>
          </table>
        
        
                     
        </div></div></div></div>
</center>
<html:errors />
<% 
//nettoyage 
Errors errors_ = (Errors)session.getAttribute(Constants.ERRORS);
errors_.clear();
%>
              
</body>
</html:html>
