package xip2jena_risk;

import com.xerox.xrce.xip2jena.DataStore;
import java.io.FileOutputStream;
import java.io.OutputStream;

public class Dump {
    public static void main(String[] args) throws Exception {
        DataStore db = StorageConfigRisk.connect();
        OutputStream out;
        out = System.out;
        //out = new FileOutputStream("dumpfile");
        db.dump(out, "N3");
    }
}
