
/**
 * RemoteExceptionException0.java
 *
 * This file was auto-generated from WSDL
 * by the Apache Axis2 version: 1.4.1  Built on : Aug 19, 2008 (10:13:39 LKT)
 */

package eu.project.Cacao.WS_Clients;

public class RemoteExceptionException0 extends java.lang.Exception{
    
    private eu.project.Cacao.WS_Clients.CACAO_nlpWS_stub.RemoteExceptionE faultMessage;
    
    public RemoteExceptionException0() {
        super("RemoteExceptionException0");
    }
           
    public RemoteExceptionException0(java.lang.String s) {
       super(s);
    }
    
    public RemoteExceptionException0(java.lang.String s, java.lang.Throwable ex) {
      super(s, ex);
    }
    
    public void setFaultMessage(eu.project.Cacao.WS_Clients.CACAO_nlpWS_stub.RemoteExceptionE msg){
       faultMessage = msg;
    }
    
    public eu.project.Cacao.WS_Clients.CACAO_nlpWS_stub.RemoteExceptionE getFaultMessage(){
       return faultMessage;
    }
}
    