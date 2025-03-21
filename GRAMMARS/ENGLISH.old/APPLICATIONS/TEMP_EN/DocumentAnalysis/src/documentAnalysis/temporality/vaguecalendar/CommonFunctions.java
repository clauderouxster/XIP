/*
 * CommonFunctions.java
 *
 * Created on November 29, 2006, 10:49 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package documentAnalysis.temporality.vaguecalendar;

/**
 * Common static functions
 * @author xtannier
 */
public class CommonFunctions {
   
    /**
     * Builds a String from an int with n characters minimum. Completes with 0s.
     * @param i the number to format
     * @param n the minimum number of characters
     * @return a String representation of an int with n characters minimum. Completes with 0s.
     */
    public static String formatInt(int i, int n)
    {
	String r = String.valueOf(i);
	if (r.length() < n)
	    {
		int l = r.length();
		for (int j = 0 ; j < (n-l) ; ++j)
		    {
			r = "0" + r;
		    }
	    }
	return r;
    }    
}
