package xrce.xipws.server;

import javax.jws.WebMethod;
import javax.jws.WebService;
import javax.ejb.Stateless;
import com.sun.xml.ws.developer.StreamingDataHandler;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.OutputStream;
import javax.activation.DataHandler;
import javax.xml.bind.annotation.XmlMimeType;
import javax.xml.ws.WebServiceException;
import javax.xml.ws.soap.MTOM;

/**
 *
 * @author grondeau
 */
@WebService()
@Stateless()
@MTOM
public class XipWS {

//    @Resource
//WebServiceContext context;

    /**
     * Web service operation
     */
    @WebMethod(operationName = "parseString")
    public String parseString(String grammarName, String input, String options) {
        return input + "+++";
    }

    // Use @XmlMimeType to map to DataHandler on the client side
    @WebMethod(operationName = "fileUpload")
    public void fileUpload(String fileName, @XmlMimeType("application/octet-stream") DataHandler data) {
        System.out.println("fileUpload: " + fileName);
        try {
            OutputStream os = data.getOutputStream();

            StreamingDataHandler dh = (StreamingDataHandler) data;
            File file = File.createTempFile(fileName, "");
            System.out.println("Creating file = " + file);
            dh.moveTo(file);
            dh.close();
        } catch (Exception e) {
            throw new WebServiceException(e);
        }
    }

    //@Override
//public InputStream getInputStream() throws IOException {
//return new ProgressMonitorInputStream(null,"uploading file...",super.getInputStream());
}
