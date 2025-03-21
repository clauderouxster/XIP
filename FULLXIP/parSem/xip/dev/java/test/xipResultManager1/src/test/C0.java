package test;

import com.xerox.jxip.*;
import java.util.*;

/**
 *  Description of the Class
 *
 * @author     grondeau
 * @created    26 juin 2006
 */
public class C0 {

    XipResultManager xipResultManager;

    public C0(XipResultManager xipResultManager) {
        this.xipResultManager = xipResultManager;
    }

    /**
     *  Description of the Method
     */
    public void myCBXml() {
        try {
            System.out.println("\n ---------------------------- Test.myCBXml ----------------------\n");
        } catch (Exception ex) {
            System.out.println(ex);
            ex.printStackTrace();
        }
    }
}

