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

<%@ taglib uri="http://jakarta.apache.org/struts/tags-bean" prefix="bean"%>



<bean:define id="pageTest" name="PAGE_TEST"  scope="session" type="java.lang.String"/>

<jsp:forward page="<%=pageTest%>" />

