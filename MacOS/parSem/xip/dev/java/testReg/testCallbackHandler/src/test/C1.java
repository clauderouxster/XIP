package testCallbackHandler.src.test;

import com.xerox.jxip.XipResult;
import com.xerox.jxip.XipResultManager;

/**
 * Description of the Class
 *
 * @author grondeau @created 26 juin 2006
 */
public class C1 extends C0 {

    static String cbName1 = "C1.myCB1";

    public C1(XipResultManager xipResultManager) {
        super(xipResultManager);

        //System.out.println("constructor: C1");
    }

    /**
     * attention C1 extends C0 => myCBXml est definie dans C0
     */
    public void myCB1() {
        try {
            res.append(cbName1);
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
}
