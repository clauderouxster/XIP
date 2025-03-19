<%@ page import="java.io.*"%>
<%@ page import="java.lang.*"%>

<%@ page isErrorPage="true"%>

<html>
 <head>
  <title>Error Page</title>
 </head>
 <body bgcolor=white>
  <%@ include file="banner.jspf" %>
  <h1>Error page:</h1>
  <p>
   <%
   	out.print("The server encountered an internal error: <br>");
%>
Unspecified exception: <%=exception.getMessage()%>.<br>
<%
        exception.printStackTrace(new java.io.PrintWriter(out));

   	//ErrorData errorData = pageContext.getErrorData();
   	//out.print("URI = " + errorData.getRequestURI() + "<br><br>");
	//out.print("status code = " + errorData.getStatusCode() + "<br><br>");
	//Throwable th = errorData.getThrowable();
	//if (th != null) {
	//	out.print("root cause = " + th.toString() + "<br><br>");
	//	StackTraceElement[] stackTraceElementArray = th.getStackTrace();
	//	for (int ind = 0; ind <stackTraceElementArray.length; ind++) {
	//		out.print(stackTraceElementArray[ind].toString() + "<br>");
	//	}
	//}	
   %>
   </p>
   <%@ include file="footer.jspf" %>
 </body>
</html>
