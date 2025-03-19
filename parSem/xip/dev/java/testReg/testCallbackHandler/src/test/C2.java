package testCallbackHandler.src.test;

import com.xerox.jxip.XipResult;
import com.xerox.jxip.XipResultManager;

/**
 * Description of the Class
 *
 * @author grondeau @created 26 juin 2006
 */
public class C2 extends C1 {

    static String cbName2 = "C2.myCB2";
    static String cbName22 = "C2.myCBXml";

    public C2(XipResultManager xipResultManager) {
        super(xipResultManager);
        //System.out.println("constructor: C2");
    }

    /**
     * attention C2 extends C1 attention C1 extends C0 => myCBXml est definie
     * dans C0
     */
    public void myCB2() {
        try {
            res.append(cbName2);
            res.append("\n");
            XipResult xipRes = xipResultManager.getXipResult();
            String str = xipRes.getSentenceString();
            res.append(str);
            res.append("\n");
            //System.out.println(res);
        } catch (Exception ex) {
            System.out.println(ex);
            System.exit(-1);
        }
    }

    /*
     * overload C0 myCBXml method
     */
    @Override
    public void myCBXml() {
        try {
            res.append(cbName22);
            res.append("\n");
            //System.out.println(res);
        } catch (Exception ex) {
            System.out.println(ex);
            System.exit(-1);
        }
    }
}
