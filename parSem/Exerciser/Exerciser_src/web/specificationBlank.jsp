<!-- 
 ****************************************************************************************
 * Copyright (c) 2007 Xerox Corporation                                                 *
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


<%@ page import="com.xerox.xrce.exills.*"%>
<%@ page import="com.xerox.xrce.exills.exercice.*"%>

<html:html>
<bean:define id="submitID" scope="page">
   <bean:message key="label.next"/>
</bean:define>
<bean:define id="resetID" scope="page">
   <bean:message key="label.reset"/>
</bean:define>

<head>
<title> Xerox, specification exercice </title>
<link rel="stylesheet" type="text/css" href="css/exercice.css"/>
<script language="javascript" src="scripts/divers.js"></script>
</head>
<body>

<center>
 <div class="upperleft">
  <div class="upperright">
     <div class="lowerleft">
        <div class="lowerright">
  
   <html:form action="/doExtract" target="center" method="get">
   <table cellspacing="0" cellpadding="2">
<!-- title of table -->
    <tr><td align="center" class="title">
    <html:img src="./images/form.gif" border="0"/>
    <bean:message key="title.fill"/>&nbsp;&nbsp;
      
      <html:reset  title="Reset" value="Reset"/><html:img src="./images/reset.gif" border="0"/>
      <html:submit title="Next" value="Next"/><html:img src="./images/next.png" border="0"/>
    </td><tr>
    <tr><td  class="subtitle"> 
         <html:img src="./images/article.gif" border="0" align="middle"/>                  
         <a href="#"  onclick='expand("textID","visible","relative");' title="expand" class="expand">&nbsp;&nbsp;&nbsp;</a>
         <a href="#"  onclick='expand("textID","hidden","absolute");' title="collapse" class="collapse">&nbsp;&nbsp;&nbsp;</a>   
         
         &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;         
          <bean:message key="title.text" />
         
	</td>
    </tr>
    <tr id="textID" style="position:relative;visibility:show">
      <td>
         <table cellspacing="0" cellpadding="0">
            <tr><td>
                <p title="your description" align="left" class="say">
                  <bean:write name="<%=Constants.EXERCICE%>" property="text" scope="session"/>
               </p>
            </td></tr>
	 </table>

      </td>
    </tr>
<!-- choose category to extract -->
    <tr><td class="subtitle" >
         <img src="./images/checkForm.png" border="0" align="middle"/>            
         <a href="#"  onclick='expand("categoryID","visible","relative");' title="expand" class="expand">&nbsp;&nbsp;&nbsp;</a>
         <a href="#"  onclick='expand("categoryID","hidden","absolute");' title="collapse" class="collapse">&nbsp;&nbsp;&nbsp;</a>  
               
         &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
         <bean:message key="title.choose.category"/> 
         
    </td></tr>
    <tr id="categoryID" class="show">
      <td>
       <table cellspacing="0" cellpadding="0">
            <tr><td class="nogradient"><html:multibox property="categories"  value="<%=ExerciceFill.VERB%>"/>
                     <span class="label"><bean:message key="label.verb"/></span>
               </td>
               <td class="nogradient"><html:multibox property="categories"  value="<%=ExerciceFill.AUX%>"/>
                     <span class="label"><bean:message key="label.auxiliaire"/></span>
               </td>
               <td ><html:multibox property="categories" value="<%=ExerciceFill.NOUN%>" />
                   <span class="label"><bean:message key="label.noun"/></span>
              </td></tr>
            <tr><td class="nogradient"><html:multibox property="categories" value="<%=ExerciceFill.ADJECTIVE%>" />
                   <span class="label"><bean:message key="label.adjective"/></span>
            </td>
            <td class="nogradient"><html:multibox property="categories" value="<%=ExerciceFill.ADVERB%>" />
                   <span class="label"><bean:message key="label.adverb"/></span>
            </td>
            <td><html:multibox property="categories" value="<%=ExerciceFill.PRONOUN%>" />
                   <span class="label"><bean:message key="label.pronoun"/></span>
            </td></tr>
             <tr><td class="nogradient"><html:multibox property="categories" value="<%=ExerciceFill.ARTICLE%>" />
                   <span class="label"><bean:message key="label.article"/></span>
            </td>
            <td class="nogradient"><html:multibox property="categories" value="<%=ExerciceFill.PREPOSITION%>" />
                   <span class="label"><bean:message key="label.preposition"/></span>
            </td>
            <td><html:multibox property="categories" value="<%=ExerciceFill.PONCTUATION%>" />
                   <span class="label"><bean:message key="label.punctuation"/></span>
            </td>
             
             </tr>
             <tr>
                 <td  class="nogradient"><html:multibox property="categories" value="<%=ExerciceFill.PROPER_NAME%>" />
                   <span class="label"><bean:message key="label.properName"/></span>
                 </td> 
                 <td colspan="2"><html:multibox property="categories" value="<%=ExerciceFill.QUANT%>" />
                   <span class="label"><bean:message key="label.quantifier"/></span>
                  </td> 
             </tr>
           
      </table>
     </td>
    </tr>

<!--  choose exercice level for blank -->
    <tr><td  class="subtitle">
<img src="./images/checkForm.png" border="0" align="middle"/>   
        <a href="#"  onclick='expand("levelID","visible","relative");' title="expand" class="expand">&nbsp;&nbsp;&nbsp;</a>
        <a href="#"  onclick='expand("levelID","hidden","absolute");' title="collapse" class="collapse">&nbsp;&nbsp;&nbsp;</a>  
         &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
             
        <bean:message key="title.choose.level"/>
        
    </td></tr>
    <tr id="levelID" class="show">
      <td>
        <table cellspacing="0" cellpadding="0">
          <tr><td><html:radio  property="level" value="<%=ExerciceFill.LEVEL0%>" /><span class="label"><bean:message key="label.level"/> 0</span> 
                   <div class="example"><bean:message key="message.example.fill.verb.level0"/></div>
          </td></tr>
         <tr><td><html:radio property="level" value="<%=ExerciceFill.LEVEL1%>"/><span class="label"><bean:message key="label.level"/> 1</span> 
      	         <div class="example"><bean:message key="message.example.fill.verb.level1"/>
                 </div>
	</td></tr>
        <tr><td><html:radio property="level" value="<%=ExerciceFill.LEVEL2%>"/><span class="label"><bean:message key="label.level"/> 2</span>
	    <div class="example"><bean:message key="message.example.fill.verb.level2"/></div>
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
</center>
<html:errors/>
</body>
</html:html>
