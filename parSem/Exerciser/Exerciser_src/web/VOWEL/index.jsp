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


<html:html>
<bean:parameter id="nickname" name="nickname" value="null"/>
<bean:parameter id="exercise" name="exercise" value="null"/>
    <head>
        <title>Xerox, index for exills exercice</title>
        <linl rel="stylesheet" href="css/exercice.css" type="text/css"/>
    </head>
    <frameset rows="10%,*,50" frameborder="0" border="0">
       <html:frame  frameName="header" page="/header.jsp"/>
       <logic:notEqual name="nickname" value="null">
            <logic:notEqual name="exercise" value="null">
                 <html:frame  frameName="center" action="<%= "/doGotoExercise?nickname=" + nickname + "&exercise=" + exercise+ "&PROJECT=VOWEL"%>" />        
            </logic:notEqual>
       </logic:notEqual>
       
       <logic:equal name="nickname" value="null">
           <html:frame  frameName="center" page="/login.jsp?PROJECT=VOWEL"/>
       </logic:equal>
        <html:frame  frameName="footer" page="/footer.jsp"/>
    </frameset>
</html:html>
