package xrce.xipws.client;

//import com.sun.xml.internal.ws.developer.JAXWSProperties;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;
//import java.util.Map;
import javax.activation.DataHandler;
import javax.activation.FileDataSource;
//import javax.xml.ws.BindingProvider;
import javax.xml.ws.WebServiceFeature;
import javax.xml.ws.soap.MTOMFeature;

class Test {

    public static void main(String[] args) {
        try {
            xrce.xipws.server.XipWSService service = new xrce.xipws.server.XipWSService();
            xrce.xipws.server.XipWS port = service.getXipWSPort();
            String result = port.parseString("gram", "aaaa", "opt");
            System.out.println("Result = " + result);
            //async request
            // Response response = port.parseFile("gram", "aaaa", "opt");

            MTOMFeature feature = new MTOMFeature();
            WebServiceFeature features = feature;
            xrce.xipws.server.XipWS port1 = service.getXipWSPort(features);
          //  Map<String, Object> ctxt = ((BindingProvider) port1).getRequestContext();
            // Enable HTTP chunking mode, otherwise HttpURLConnection buffers
        //    ctxt.put(JAXWSProperties.HTTP_CLIENT_STREAMING_CHUNK_SIZE, 8192);
            File file = getFile();
            DataHandler data = new DataHandler(new FileDataSource(file));
            port1.fileUpload("file.bin", data);
//            StreamingDataHandler sdh = (StreamingDataHandler) dh;
//            InputStream in = sdh.readOnce();
//            in.close();
//            sdh.close();
        } catch (Exception ex) {
            System.out.println(ex.toString());
            ex.printStackTrace();
            System.exit(-1);
        }
    }

    // create a file with data
    public static File getFile() throws IOException {
        File file = File.createTempFile("jaxws", ".bin");
        OutputStream out = new FileOutputStream(file);
        byte buf[] = new byte[100];
        for (int i = 0; i < buf.length; i++) {
            buf[i] = (byte) i;
        }
        for (int i = 0; i < 100; i++) {
            out.write(buf);
        }
        out.close();
        System.out.println(file.getName() + " " + file.length());
        return file;
    }
}