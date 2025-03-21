/*
 * Util.java
 *
 * Created on July 26, 2006, 11:54 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package jcoref;

import com.xerox.jxip.XipDependency;
import com.xerox.jxip.XipFeature;
import com.xerox.jxip.XipLeaf;
import com.xerox.jxip.XipNode;
import java.util.Enumeration;
import java.util.Iterator;
import java.util.Vector;
import javax.swing.event.ChangeEvent;

/**
 *
 * @author ait
 */
public abstract class Util {
    
    /** Creates a new instance of Util */
    public Util() {
    }
    
    public static String dependenciesToString(Vector deps) {
        String s = "";
        for(int i=0; i<deps.size();i++) {
            s = s.concat( dependencyToString( (XipDependency) deps.elementAt(i)) );
            s = s.concat("\n");
        }
        return s;
    }
    
    public static String dependencyToString(XipDependency dep) {
        String s = dep.getName() + '(' ;
        Vector args=dep.getParameters();
        for(int j=0 ;  j < args.size(); j++) {
            s = s.concat( Util.nodeValueToString( (XipNode) args.elementAt(j)) );
            if ( (j+1) < args.size() ) {
                s = s.concat(", ");
            }
        }
        s = s.concat(")");
        return s;
    }
    
    
    static long getAbsoluteTokenDistanceBetween(XipNode n1, XipNode n2) {
        return Math.abs( n2.getLeftTokOffset() - n1.getLeftTokOffset() );
    }
    
    static long getAbsoluteTokenDistanceBetween(ReferentNode n1, ReferentNode n2) {
        return Math.abs( n2.getLeftTokOffset() - n1.getLeftTokOffset() );
    }
    
    
    static boolean matchesAbbr(String abbr1, String abbr2) {
        if (abbr1==null || abbr2==null || abbr1.length()<=1 || abbr2.length()<=1)
            return false;
        else if (abbr1.contentEquals(abbr2))
            return true;
        else {
            int i=0, j=0;
            while (i<abbr1.length() && j<abbr2.length()) {
                if (abbr1.charAt(i)!=abbr2.charAt(j)) {
                    if ( i==0 || abbr2.charAt(j)!='.' )
                        return false;
                    else
                        j++;
                } else {
                    i++;
                    j++;
                }
            }
            if (i==abbr1.length() &&
                    ( j==abbr2.length() ||
                    (j==(abbr2.length()-1) && abbr2.charAt(j)=='.'  ) ) )
                return true;
            else
                return false;
        }
    }
    
    
    
    /** Returns true iff s1 is a name initial for s2. E.g. "A." is a name
     * initial for "Audrey", "Ch." is a name initial for "Charles".
     * Returns false otherwise.
     * @param s1 the expected initial
     * @param s2 the name of which s1 could be the initial
     */
    static boolean isNameInitialOf(String s1, String s2) {
        int i=0, j=0;
        while (i<s1.length() && j<s2.length()) {
            if (!(Character.isUpperCase( s1.charAt(i) )) ||
                    s1.charAt(i)!=s2.charAt(j) ) {
                return false;
            }
            i++; j++;
            if (i<s1.length()) {
                if (Character.isLowerCase(s1.charAt(i))) {
                    if (j==s2.length() || s1.charAt(i)!=s2.charAt(j)) {
                        return false;
                    } else {
                        i++; j++;
                    }
                }
                if (i<s1.length()) {
                    if (Character.isUpperCase(s1.charAt(i))) {
                        return false;
                    } else if (s1.charAt(i)=='.') {
                        while (j<s2.length() && s2.charAt(j)!=' ' &&
                                s2.charAt(j)!='-' )
                            j++;
                        i++;
                        if (j<s2.length() && s2.charAt(j)=='-') {
                            if (i==s1.length() || s1.charAt(j)!='-')
                                return false;
                            else
                                i++;
                        }
                        j++;
                    }
                }
            }
        }
        if (i>=s1.length())
            return true;
        else
            return false;
    }
    
    public static boolean isUpper(String s) {
        int i=0;
        while (i<s.length()) {
            if (!Character.isUpperCase(s.charAt(i)))
                return false;
            i++;
            if (i<s.length() && s.charAt(i)=='.')
                i++;
        }
        return true;
    }
    
    public static String nodeToString(XipNode node) {
        return node.chunkTreeToString(true, "     ", true) ;
    }
    
    public static String nodeValueToString(XipNode node) {
        return node.getId() + ":" + node.getSurface() ;
    }
    
    
    
}
