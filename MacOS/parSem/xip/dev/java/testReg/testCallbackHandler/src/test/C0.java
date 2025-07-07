package testCallbackHandler.src.test;

import com.xerox.jxip.XipResultManager;

/**
 * Description of the Class
 *
 * @author grondeau @created 26 juin 2006
 */
public class C0 {

    XipResultManager xipResultManager;
    StringBuffer res;
    static String cbName0 = "C0.myCBXml";

    public C0(XipResultManager xipResultManager) {
        this.xipResultManager = xipResultManager;
        //System.out.println("constructor: C0");
    }

    public void clrBuf() {
        res = new StringBuffer();
    }

    public String getRes() {
        return res.toString();
    }

    public void myCBXml() {
        try {
            res.append(cbName0);
            res.append("\n");
            //System.out.println(res);
        } catch (Exception ex) {
            System.out.println(ex);
            System.exit(-1);
        }
    }
}
