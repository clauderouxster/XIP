<%@ taglib uri="/WEB-INF/struts-html.tld" prefix="html" %>
<%@ taglib uri="/WEB-INF/struts-bean.tld" prefix="bean" %>
<%@ taglib uri="/WEB-INF/struts-logic.tld" prefix="logic" %>
<%@ page import="com.xerox.xrce.exills.*" %>
<html:html>
  <head>
    <title>Xerox menu</title>
    <link rel="stylesheet" href="css/exercice.css" type="text/css"/>
  </head>
  <body>
  
  <table cellspacing="0" cellpadding="0" border="0">
     <tr>
         <td class="borderLeft">&nbsp;</td>
         <td class="subtitle">
             <html:img src="./images/user.png" border="0" align="middle"/>
               <logic:empty name="<%=Constants.USER%>" property="firstname" scope="session">
                  <logic:empty name="<%=Constants.USER%>" property="name" scope="session">
                        <bean:write name="<%=Constants.USER%>" property="login" scope="session"/>:&nbsp;   
                  </logic:empty>
               </logic:empty>
               <bean:write name="<%=Constants.USER%>" ignore="true" property="firstname" scope="session"/>&nbsp;
               <bean:write name="<%=Constants.USER%>" ignore="true" property="name" scope="session"/>    
         </td>
         <td class="borderLeft">&nbsp;</td>         
         <td class="subtitle">
          <bean:message key="title.menu"/>&nbsp;:&nbsp;

           <html:link action="/doLogout"  target="center" titleKey="label.logout" styleClass="menu">
             <img src="./images/stop.png" border="0" align="middle"/> 
              <bean:message key="label.logout"/></html:link>
           &nbsp;&nbsp;
          
           <logic:notEqual name="<%=Constants.USER%>" property="profile" scope="session" value="student">
           <html:link action="/doClean" target="center" title="create another exercice" styleClass="menu">
            <img src="./images/new.gif" border="0" align="middle"/> <bean:message key="label.create"/>
          </html:link>
          </logic:notEqual>
           &nbsp;&nbsp;
              
           <html:link action="/doShowExercises" target="center" styleClass="menu">
              <img src="./images/gotosource.png" border="0" align="middle"/> 
              <bean:message key="label.exercices"/>
          </html:link>
         
         </td>
     </tr>
  </table>
  
  
  </body>
</html:html>
