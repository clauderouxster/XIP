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
<%@ page errorPage="error.jsp"%>
<%@ page import="java.util.*" %>
<%@ page import="com.xerox.xrce.exills.*" %>
<html:html>
    
<bean:define id="checkSubmit" scope="page">
   <bean:message key="label.check"/>
</bean:define>
<bean:define id="resetLabel" scope="page">
   <bean:message key="label.reset"/>
</bean:define>
<bean:define id="generateSubmit" scope="page">
   <bean:message key="label.generate"/>
</bean:define>

<head>
 <title>Xerox, create exercice with verb</title>
 <link rel="stylesheet" href="css/exercice.css" type="text/css"/>
 <script language="javascript" src="scripts/divers.js"></script>
</head>
<body>
<center>
   <html:form action="/doGenerate" method="get" target="center"> 
<div class="upperleft">
  <div class="upperright">
     <div class="lowerleft">
        <div class="lowerright">
          <table border="0" cellspacing="0" cellpadding="2">
             <tr><td align="center" class="title">
                     <html:img src="./images/check.png" border="0" /> 
                     <bean:message key="title.fill.generate" />
                     <html:submit title="Next" value="Next"/><html:img src="./images/next.png" border="0"/>
             </td></tr>
<!-- from to GENERATE the exercie -->
         <tr><td>
              <table border="0" cellspacing="0" cellpadding="2">
                  <tr><td  class="subtitle">
                      <html:img src="./images/checkForm.png" border="0" align="middle"/>
                      <html:img src="./images/article.gif" border="0" align="middle"/>                        
                      <bean:message key="title.exercice"/>
                       
                  </td></tr>
                  <tr><td>
                        <div class="panel"> 
                        <bean:write name="<%=Constants.PRE_EXTRACT_FORM%>" scope="session" filter="false" ignore="true"/>		
                       </div>
                  </td></tr>
                </table>    
              </html:form>
      </td></tr>
          <%--   <tr><td class="subtitle">
                   <a href="#"  onclick='expand("checkID","visible","relative");' title="label.expand" class="expand">&nbsp;&nbsp;&nbsp;</a>
                   <a href="#"  onclick='expand("checkID","hidden","absolute");' title="collapse" class="collapse">&nbsp;&nbsp;&nbsp;</a>   
                   <html:img src="./images/checkForm.png" border="0" /> 
                    &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
                   <bean:message key="title.check"/>                      
                     
             </td></tr>--%>
            <tr id="checkID" class="show"><td align="center">
            <html:form action="/doCheck" method="get" target="center">
            <table cellspacing="0" cellpadding="2">
                 <tr>
                    <logic:notEmpty name="<%=Constants.TEMPS_LIST%>" scope="session">
                       <td class="nogradient" align="center">
                    </logic:notEmpty>
                    <logic:empty name="<%=Constants.TEMPS_LIST%>" scope="session">
                        <td align="center" >
                    </logic:empty>
                
                  <table>
                             <tr valign="top"><td class="subtitle" align="center" valign="top">
                                 <html:img src="./images/suggestion.gif" border="0" /> 
                                 <bean:message key="title.lemma"/>
                             </td></tr>
                             <tr>
                              <logic:notEmpty name="<%=Constants.TEMPS_LIST%>" scope="session">
                                   <td class="nogradient" align="center">
                              </logic:notEmpty>
                               <logic:empty name="<%=Constants.TEMPS_LIST%>" scope="session">
                               <td align="center">
                               </logic:empty>
                               <html:select property="lemma" multiple="true" size="8">
                                 <html:optionsCollection name="<%=Constants.LEMMA_LIST%>" label="label" value="value" />
                               </html:select>
                             </td></tr>
                             
                        </table>
                     </td>
                    <logic:notEmpty name="<%=Constants.TEMPS_LIST%>" scope="session">
                     <td align="center">
                         <table>
                             <tr valign="top"><td class="subtitle" align="center" valign="top">
                                   <html:img src="./images/suggestion.gif" border="0" align="middle"/> 
                                   <bean:message key="title.temp"/>
                             </td></tr>
                             <tr><td align="center">
                                    <html:select property="temps" multiple="true" size="8">
                                       <html:optionsCollection name="<%=Constants.TEMPS_LIST%>" label="label" value="value" />
                                    </html:select>
                         
                                </td></tr>                                        
                         
                        </table>
                   </td>
                  </logic:notEmpty>
                </tr>
                <tr>
               <logic:empty name="<%=Constants.TEMPS_LIST%>" scope="session">
                    <td  align="center">
                </logic:empty>
                <logic:notEmpty name="<%=Constants.TEMPS_LIST%>" scope="session">
                    <td colspan="2" align="center">
                </logic:notEmpty>            
   
                    <html:submit styleClass="ok" title="check">&nbsp;&nbsp;</html:submit>
                    
               </td></tr>
          </table>    
          </html:form>

    </table>
</div></div></div></div>
</center>
<html:errors/>
</body>
</html:html>
