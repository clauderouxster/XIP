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


<html:html>
 <head>
      <title>Xerox</title>
      <link rel="stylesheet" type="text/css" href="css/exercice.css"/>
 </head>
 <body>
<center>
 <span class="copyright">
  
   <html:link href="mailto:exillsort-techsupport@xrce.xerox.com" title="send an email">
         Exerciser verion : <bean:write name="VERSION" scope="application"/>
   </html:link>  <BR>
   Copyright &copy; The Document Company - Xerox 2005, All Rights Reserved<br>
   
   
 </span>
 </center>
 </body>
 </html:html>
