package test;

import com.xerox.jxip.*;

/**
 *  Description of the Class
 *
 * @author     grondeau
 * @created    26 juin 2006
 */
public class C2 extends C1{

    public C2(XipResultManager xipResultManager) {
        super(xipResultManager);
    }
	
	/**
	 *  Description of the Method
	 */
	public void myCB1() {
		try {
			System.out.println("\n ---------------------------- Test.myCB1 --------------------\n");
			XipResult xipRes = xipResultManager.getXipResult();
			boolean withFeature = true;
			//String str = xipRes.toString(withFeature);
			//System.out.println("\n ---- the xipResult ----\n" + str);
			String str1 = xipRes.getSentenceString();
			System.out.println("\n ---- the sentence ----\n" + str1);
		} catch (Exception ex) {
			System.out.println(ex);
			ex.printStackTrace();
		}
	}
}

