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

<%@ page import="java.util.*" %>
<%@ page import="com.xerox.xrce.exills.*" %>
<%@ page import="com.xerox.xrce.exills.exercice.*" %>



<html:html>
<bean:define id="user" name="<%=Constants.USER%>" scope="session" type="com.xerox.xrce.exills.User"/>
<bean:define id="page_" name="PAGE_CORRECTED" scope="session" type="java.lang.String"/>
<bean:define id="title_" name="TITLE_CORRECTED" scope="session" type="java.lang.String"/>
<bean:define id="author_" name="AUTHOR_CORRECTED" scope="session" type="java.lang.String"/>
<bean:define id="corrector" name="<%=Constants.CORRECTOR%>" scope="session" type="Corrector"/>
<bean:define id="analysis" name="corrector" property="analysis" type="Analysis" />

<head>
 <title>Xerox,Exerciser Correction</title>
 <link rel="stylesheet" href="css/exercice.css" type="text/css"/>
 <script language="javascript" src="scripts/divers.js">
      
     
 </script>



</head>
<body onload="initMouseEvent()">
<center>
<div class="upperleft">
  <div class="upperright">
     <div class="lowerleft">
        <div class="lowerright">

<table cellspacing="0" cellpadding="2">
      <tr><td align="center" class="title">
     <html:img src="./images/correctionView.gif" border="0" align="middle"/>  
    <bean:message key="title.correction"/>&nbsp;&nbsp;
      <html:link action="<%= "/doTest?PAGE=" + page_%>" target="main" >
                 <html:img src="./images/previous.png" border="1" align="middle" titleKey="label.retry"/>
         </html:link>&nbsp;&nbsp;
         <a href='javascript:openPopup("<%= "correctionView.jsp?TITLE="+ title_%>","");'  >
              &nbsp;&nbsp;
              <html:img src="./images/test_form.png" border="1" align="middle" title="view Correction"/>
           </a>
      </td></tr>
      <tr>
       <td  class="subtitle">
         <a href="#"  onclick='expand("inform","visible","relative");' title="expand" class="expand">&nbsp;&nbsp;&nbsp;</a>
         <a href="#"  onclick='expand("inform","hidden","absolute");' title="collapse" class="collapse">&nbsp;&nbsp;&nbsp;</a>
          &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;           
         <bean:message key="title.information" />&nbsp;&nbsp;
       
       </td>
      </tr>
      <tr id="inform"><td>
        <table cellspacing="0" cellpadding="2">
        <tr>
            <td class="nogradient" width="50%">
                <bean:write name="corrector" property="correctCpt"/> / <bean:write name="corrector" property="nbAnswers"/>  <bean:message key="label.right.answer"/>
           </td>
           <td  width="50%">
               <bean:write name="corrector" property="percent"/> % <bean:message key="label.correct.percentage" />
           </td>
        <tr>
       
        </table>
      </td>
      </tr>
      <tr>
       <td  class="subtitle">
         <a href="#"  onclick='expand("correction","visible","relative");' title="expand" class="expand">&nbsp;&nbsp;&nbsp;</a>
         <a href="#"  onclick='expand("correction","hidden","absolute");' title="collapse" class="collapse">&nbsp;&nbsp;&nbsp;</a>
         &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;         
<bean:message key="title.exercice" /> : <bean:write name="title_" ignore="true"/>
       </td>
      </tr>

      <tr id="correction">
          <td>
             <table cellspacing="0" cellpadding="0">
             <tr>
             <td >
             <div class="sentence"><ol>
          <% 
             
             int nbSentences = analysis.countSentences();
             for (int j=1; j <= nbSentences; j++){
          %>
              <li><html:img src="./images/blue-ball-mid.gif" border="0" align="middle"/> 
              <%
                 Lexeme[] array = analysis.getLexemeInSentence(j);
                 for (int i=0 ; i <array.length ; i++){
                 Answer answer = corrector.getAnswer(array[i].getId());
                 if ( answer != null) {
                     if ( answer.isCorrect()) { %>
                      <span class="verbChecked"><%= answer.getValue()%></span>   
                 <% } else { %>
                     <a href="#" class="error"
                                  onmouseover='error("<%=array[i].getId()%>","visible","absolute")' 
                                  onmouseout='error("<%=array[i].getId()%>","hidden","absolute")'>
                     <%=answer.getValue()%></a>
                      <div id="<%=array[i].getId()%>" class="errorAnswer"><ul>
                 <% 
                     Vector v = answer.getErrors();
                     Enumeration eV = v.elements();
                     while(eV.hasMoreElements()) { %>
                         <html:img src="./images/error-badge.gif" border="0" align="middle"/> <bean:message key="label.error"/> <%=eV.nextElement()%>    
                     <%}%>
                      </ul></div>
                   <%
                     }
                     
                 } else { 
                     
                     if( analysis.isPunct(array[i])) { %>
                      <%= array[i].getSurface()%>
               <%  } else { %>
                    &nbsp;<%= array[i].getSurface()%>
           <% }}
 } %>
              <br><br></li>   
             
<%}%>
             </ol></div></td></tr>
             </table></td></tr>
             </table>

</div></div></div></div>
</center>


</body>

</html:html>
