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
<bean:parameter id="title" name="TITLE"/>
<bean:define id="corrector" name="<%=Constants.CORRECTOR%>" scope="session" type="Corrector"/>
<bean:define id="analysis" name="corrector" property="analysis" type="Analysis" />

<head>
 <link rel="stylesheet" href="css/exercice.css" type="text/css"/>
 <title><bean:write name="title"/></title>
</head>
<body>
<center>
<div class="upperleft">
  <div class="upperright">
     <div class="lowerleft">
        <div class="lowerright">

<table cellspacing="0" cellpadding="2">
      <tr><td align="center" class="title">
          <html:img src="./images/test_form.png" border="0" align="middle"/>   
          <bean:message key="title.correction"/></td></tr>
      <tr>
       <td align="center" class="subtitle">
                   
           <bean:message key="title.exercice" /> : <bean:write name="title" ignore="true"/>
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
          for (int j=1; j <= nbSentences; j++){ %>
          <li><html:img src="./images/blue-ball-mid.gif" border="0" align="middle"/> 
              
              <%
                 Lexeme[] array = analysis.getLexemeInSentence(j);
                 for (int i=0 ; i <array.length ; i++){
                 Answer answer = corrector.getAnswer(array[i].getId());
                 if ( answer != null) { %>
                      <span class="verbChecked"><%= answer.getCorrectValue()%></span>   
              <% } else { 
                        if( analysis.isPunct(array[i])) { %>
                             <%= array[i].getSurface()%>
                    <%  } else { %>
                             &nbsp;<%= array[i].getSurface()%>
                      <% }
                }
                      }//fin du for %>
            <br><br></li>
            <%
             } //fin du for pour les phrases
            %>
             </ol></div></td></tr>
             </table></td></tr>
             </table>

</div></div></div></div>
</center>


</body>

</html:html>
