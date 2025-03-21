<!-- 
 ****************************************************************************************
 * Copyright (c) 2005 Xerox Corporation                                                 *
 * Xerox Research Centre Europe - Grenoble                                              *
 * All Rights Reserved                                                                  *
 * Copyright protection claimed includes all forms and matters of copyrightable         *
 * material and information now allowed by statutory or judicial law or                 *
 * hereinafter granted, including without limitation, material generated from           *
 * the software programs which are displayed on the screen such as icons,               *
 * screen display looks, etc. All rights reserved. No part of this document             *
 * may be reproduced, stored in a retrieval system, or transmitted, in any              *
 * form or by any means, electronic, mechanical, photocopied, recorded, or              *
 * otherwise, without prior written consent from XEROX corporation.                     *
 ****************************************************************************************
 * -->
<%@ taglib uri="/WEB-INF/struts-html.tld" prefix="html" %>
<%@ taglib uri="/WEB-INF/struts-bean.tld" prefix="bean" %>
<%@ taglib uri="/WEB-INF/struts-logic.tld" prefix="logic" %>

<%@ page import="com.xerox.xrce.exills.*" %>
<%@ page errorPage="error.jsp"%>
<!-- default author for the exercie : if name doesn't exist then we choose the login -->
<bean:define id="authorID" scope="page">
<logic:present name="<%=Constants.USER%>" scope="session">   
<logic:empty name="<%=Constants.USER%>" property="name" scope="session" >
       <bean:write name="<%=Constants.USER%>" property="login" scope="session" />
   </logic:empty>
   <logic:notEmpty name="<%=Constants.USER%>" property="name" scope="session">
       <bean:write name="<%=Constants.USER%>" property="name" scope="session"/>
   </logic:notEmpty>
</logic:present >
</bean:define>

<html:html>
<bean:define id="submitID" scope="page">
   <bean:message key="label.next"/>
</bean:define>
<bean:define id="resetID" scope="page">
   <bean:message key="label.reset"/>
</bean:define>


<head>
<title>Xerox, create an exercie</title>

<link rel="stylesheet" type="text/css" href="css/exercice.css"/>
<script language="javascript" src="scripts/divers.js"></script>
</head>
<body>
<center>
<p>
<div class="upperleft">
  <div class="upperright">
     <div class="lowerleft">
        <div class="lowerright">

<html:form action="/doAnalyse" method="get" target="center">
<table  cellspacing="0" cellpadding="0">
   <tr><td align="center" class="title">
     <html:img src="./images/new.gif" border="0" align="middle"/>
     <bean:message key="title.create.exercice" />
     &nbsp;&nbsp;       
      
      <html:reset  title="Reset" value="Reset"/><html:img src="./images/reset.gif" border="0"/>
      <html:submit title="Next" value="Next"/><html:img src="./images/next.png" border="0"/>
    </td></tr>
   <tr>
      <td class="subtitle"> 
        <html:img src="./images/form.gif" border="0" align="middle"/>        
        <a href="#"  onclick='expand("inform","visible","relative");' title="expand" class="expand">&nbsp;&nbsp;&nbsp;</a>
        <a href="#"  onclick='expand("inform","hidden","absolute");' title="collapse" class="collapse">&nbsp;&nbsp;&nbsp;</a>      

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
        <bean:message key="title.information" />
        
       </td>
      
   </tr>
   <tr id="inform" class="show" >
     <td >
       <table cellspacing="0" cellpadding="2">
      <!-- exercice name -->
       <tr>
          <td align="left" class="label"><bean:message key="label.title" /> </td>
          <td align="left">
             <bean:define id="titleExercice" scope="page">   
                  <bean:message key="value.type.exercice" />
             </bean:define>
            <html:text property="title" value="<%=titleExercice%>"/>
          </td>
       </tr>
     <!-- exercie author -->
     <tr>
       <td align="left" class="label"><bean:message key="label.author" /></td>
       <td align="left" > 
         <html:text property="author" value="<%=authorID%>"/>
             </td>
    </tr>
  <!--- Instruction for the exercise-->
     <tr>
   <td align="left" class="label"><bean:message key="label.instruction" /></td>
   <td align="left" >
      <bean:define id="instructionValue" scope="page">   
              <bean:message key="value.instruction" />
      </bean:define>
    <html:textarea property="instruction" rows="3" cols="80" value="<%=instructionValue%>"/>
  </td>
  </tr>   
    
<!-- description of this exercie -->
   <tr>
   <td align="left" class="label"><bean:message key="label.description" /></td>
   <td align="left" >
      <bean:define id="descriptionValue" scope="page">   
              <bean:message key="value.description" />
      </bean:define>
       <html:text property="comment" value="<%= descriptionValue %>"/>
  </td>
  </tr>
       </table>
     </td>
   </tr>
<!-- text of the exercice -->
   <tr >
      
	<td class="subtitle">
          <html:img src="./images/article.gif" border="0" align="middle"/>
          <a href="#"  onclick='expand("text","visible","relative");' title="expand" class="expand">&nbsp;&nbsp;&nbsp;</a>
          <a href="#"  onclick='expand("text","hidden","absolute");' title="collapse" class="collapse">&nbsp;&nbsp;&nbsp;</a>      

          &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
         <bean:message key="title.text" />
         
        </td>
     <td align="center" class="subtitle">
        </td>
   </tr>
   <tr id="text" class="show">
     <td>
        <table cellspacing="0" callpadding="0">
             <tr><td align="center" >
             <html:textarea property="text" rows="20" cols="100" />
          </td></tr>
        </table>
    </td>
  </tr>
  <tr>
    <td align="center"> 
    
    </td>
  </tr>

</table>
</html:form>
</div></div></div></div>
</p>
</center>
<html:errors/>
</body>

</html:html>
