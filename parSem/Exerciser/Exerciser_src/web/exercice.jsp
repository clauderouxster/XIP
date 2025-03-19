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
<%-- <%@ taglib uri="/WEB-INF/taglibs-xtags.tld" prefix="xtags" %> --%>

<%@ page import="java.util.*" %>
<%@ page import="com.xerox.xrce.exills.*" %>
<%@ page import="com.xerox.xrce.exills.exercice.*" %>
<%@ page import="java.io.*" %>

<html:html>
    <bean:define id="exercices" name="List_exercises"  scope="session"/>
    <bean:define id="user" name="<%=Constants.USER%>"  scope="session" type="com.xerox.xrce.exills.User"/>
    <head>
        <title>Xerox, create exercice with verb</title>
        <link rel="stylesheet" href="css/exercice.css" type="text/css"/>
        <script language="javascript" src="scripts/divers.js"></script>
    </head>
    <body>
        
        <center>
            <div class="upperleft">
                <div class="upperright">
                    <div class="lowerleft">
                        <div class="lowerright">
                            
                            <table cellspacing="0" cellpadding="2">
                                <tr><td align="center" class="title" colspan="3">
                                        <html:form action="/doShowExercises" method="get" target="center">
                                            <img src="./images/gotosource.png" border="0" align="middle"/> 
                                            <bean:message key="title.exercice.list"/>
                                            
                                            <html:select property="type">
                                                <html:option value="3">Default</html:option> 
                                                <html:option value="1">by Author</html:option> 
                                                <html:option value="5">by Title</html:option> 
                                                <html:option value="3%>">by Date</html:option> 
                                                <html:option value="4">by Level</html:option> 
                                                <html:option value="2">by Language</html:option> 
                                                <html:option value="6"><bean:write name="<%=Constants.USER%>" property="login" scope="session"/></html:option> 
                                            </html:select>
                                            <html:submit  value="sort"/><html:img src="./images/alphaSort.gif" border="0"/> 
                                        </html:form>
                                </td></tr>
                                
                                
                                <logic:iterate id="element" name="exercices" type="MetaExercise" scope="page">
                                    
                                    
                                    <tr>  
                                        <td class="bottomBorder"> 
                                            <html:img src="./images/jsp.gif" border="0" align="middle"/> 
                                            <bean:write name="element" property="title"/>
                                            <%-- <html:link page="<%=   "/" + ExerciceListener.FOLDER_EXERCICE  + "/" + user.getProject() + "/" +element.getPage()%>" target="main" styleClass="exercice">             
                                                <html:img src="./images/start.gif" border="1" align="middle"/> 
                                            </html:link>  --%>
                                            <html:link action="<%="/doTest?PAGE=" + element.getPage()%>" target="main" styleClass="exercice">             
                                                <html:img src="./images/start.gif" border="1" align="middle"/> 
                                            </html:link>  
                                            
                                            
                                        
                                            
                                            
                                            
                                            
                                        </td>
                                        <td class="bottomBorder">
                                            <span class="label"><bean:message key="label.topic"/> :</span> 
                                            <span class="value"><bean:write name="element" property="topic"/></span>
                                            
                                        </td>
                                        <td class="bottomBorderGradient"> 
                                            <span class="label">
                                                <html:img src="./images/user.png" border="0" align="middle"/>
                                                <bean:message key="label.author"/> :
                                            </span> 
                                            <span class="value"><bean:write name="element" property="author"/></span>
                                        </td>
                                    </tr>           
                                    <tr>
                                        <td class="nogradient"> 
                                            <span class="label">
                                                
                                                <bean:message key="label.level"/> :
                                            </span> 
                                            <span class="value"><bean:write name="element" property="level"/></span>
                                        </td>
                                        <td class="nogradient"> 
                                            <span class="label"> <bean:message key="label.language"/> :</span> 
                                            <span class="value"><bean:write name="element" property="language"/></span>
                                        </td>
                                        <td > 
                                            <span class="label">
                                                <html:img src="./images/date.png" border="0" align="middle"/>
                                                <bean:message key="label.date"/> :
                                            </span> 
                                            <span class="value"><bean:write name="element" property="date"/></span>
                                        </td>
                                    </tr>
                                    <tr>
                                        <td colspan="3"> 
                                            <span class="label">
                                                <bean:message key="label.description"/> :
                                            </span> 
                                            <span class="value"><bean:write name="element" property="description"/></span>
                                        </td>
                                    </tr>
                                    <tr>
                                        <td colspan="3"> 
                                            <span class="label">
                                                <bean:message key="label.categories"/> :
                                            </span> 
                                            <span class="value"><bean:write name="element" property="categories"/></span>
                                        </td>
                                        
                                    </tr>
                                    
                                </logic:iterate>
                                
                                
                                
                                
                            </table>
                            
                        </div>
                    </div>
                </div>
            </div>
        </center>
        
        
    </body>
    
</html:html>
