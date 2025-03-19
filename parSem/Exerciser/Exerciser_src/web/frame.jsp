<%--  
 ****************************************************************************************
 * Copyright (c) 2007  Xerox Corporation                                                 *
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
 --%>


<%@ taglib uri="/WEB-INF/struts-html.tld" prefix="html" %>
<%@ taglib uri="/WEB-INF/struts-bean.tld" prefix="bean" %>
<%@ taglib uri="/WEB-INF/struts-logic.tld" prefix="logic" %>
<%@ page import="java.util.*"%>


<html:html>
    <head>
        <title>Xerox</title>
        
    </head>
    <bean:parameter id="login"     name="LOGIN"      value="false"/>
    <bean:parameter id="page_"     name="PAGE"       value="exercice.jsp"/>
    <bean:parameter id="corrected" name="corrected"  value="false"/>
    
    
    <logic:present name="PAGE_TEST" scope="session">
        <logic:notEqual  name="page_" value="correction.jsp">
            
            <frameset frameborder="0" rows="*" border="0">
                <html:frame page="/import.jsp" frameName="main" />
            </frameset>
        </logic:notEqual>   
        
        
        <logic:equal  name="page_" value="correction.jsp">
             <frameset frameborder="0" rows="*" border="0">
                <html:frame page="/correction.jsp" frameName="main" />
            </frameset>
        </logic:equal>   
        
        
    </logic:present>
    
    <logic:notPresent name="PAGE_TEST" scope="session">
        <logic:equal name="login" value="false">
            <frameset frameborder="0" rows="50,*" border="0">
                <html:frame page="/menu.jsp" frameName="menu"/>
                <html:frame page="<%= "/" + page_%>" frameName="main" />
            </frameset>
        </logic:equal>
        
        <%---- Si on a eu une session invalide  --------------------%>
        <logic:equal name="login" value="true">
            <jsp:include page="/error.jsp" flush="true"/>
        </logic:equal>
    </logic:notPresent>
    
</html:html>
