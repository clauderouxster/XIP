package test;

import com.xerox.jxip.*;
import java.util.*;

/**
 *  Description of the Class
 *
 * @author     grondeau
 * @created    26 juin 2006
 */
public class C1 extends C0 {

    public C1(XipResultManager xipResultManager) {
        super(xipResultManager);
    }

    /**
     *  Description of the Method
     */
    public void myCB2() {
        try {
            System.out.println("\n ---------------------------- Test.myCB2 --------------------\n");
            String str = "";
            XipResult xipRes = xipResultManager.getXipResult();
            boolean withFeature = true;
            str = xipRes.toString(withFeature);
        //System.out.println("\n ---- the xipResult ----\n" + str + "\n\n");
//            for (Enumeration enumeration = xipRes.getSentences().elements(); enumeration.hasMoreElements();) {
//                XipUnit xipUnit = (XipUnit) enumeration.nextElement();
//                System.out.println("\nxipUnit.getSentenceString() = " + xipUnit.getSentenceString());
//                //str += xipUnit.toString(withFeature);
//                XipNode xipNode = xipUnit.getRoot();
//                System.out.println("\nxipNode.getCategory() = " + xipNode.getCategory());
//                System.out.println("\nxipNode.getSurface() = " + xipNode.getSurface());
//            }
        } catch (Exception ex) {
            System.out.println(ex);
            ex.printStackTrace();
        }
    }

    /**
     *  Description of the Method
     */
//    public void myCBXml() {
//        try {
//            System.out.println("\n ---------------------------- Test.myCBXml ----------------------\n");
//        } catch (Exception ex) {
//            System.out.println(ex);
//            ex.printStackTrace();
//        }
//    }
}

