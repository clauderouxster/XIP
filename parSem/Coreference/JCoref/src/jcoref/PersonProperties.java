/*
 * PersonName.java
 *
 * Created on June 15, 2007, 5:08 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package jcoref;

import com.xerox.jxip.XipNode;

/**
 *
 * @author ait
 */
public class PersonProperties {
    
    static public String FIRSTNAME_DEP = "INTERNALS_FIRSTNAME1";
    static public String MIDDLENAME_DEP = "INTERNALS_FIRSTNAME2";
    static public String LASTNAME_DEP = "INTERNALS_LASTNAME";
    static public String PTITLE_DEP = "INTERNALS_TITLE";
    static public String MASC = "MASC";
    static public String FEM = "FEM";
    
    static public enum Gender {MASC,FEM, UNDEF};
    
    String nameForm;
    
    String firstname;
    String firstnameInitial;
    String middlename;
    String middlenameInitial;
    String lastname;
    String lastnameInitial;
    Gender gender = Gender.UNDEF;
    String pTitle;
    
    /** Creates a new instance of PersonName */
    public PersonProperties(XipNode node) {
        nameForm = node.getSurface();
        updateWith(node);
    }

        /** Creates a new instance of PersonName */
    public PersonProperties(Gender gender, String fname, String mname, String lname) {
        this.gender = gender;
        this.firstname = fname;
        this.middlename = mname;
        this.lastname = lname;
    }

    
    /** Updates properties of this Person with those available in node
     */
    protected void updateWith(XipNode node) {
        String nameform;
        XipNode namenode = null;
        
        if (this.firstname==null) {
            namenode = node.getDependentIn(FIRSTNAME_DEP);
            if (namenode!=null) {
                nameform = namenode.getSurface();
                if (isInitial(namenode)) {
                    firstnameInitial = nameform;
                } else {
                    firstname = nameform;
                }
            }
        }
        
        if (this.middlename==null) {
            namenode = node.getDependentIn(MIDDLENAME_DEP);
            if (namenode!=null) {
                nameform = namenode.getSurface();
                if (isInitial(namenode)) {
                    middlenameInitial = nameform;
                } else {
                    middlename = nameform;
                }
            }
        }
        
        if (this.lastname==null) {
            namenode = node.getDependentIn(LASTNAME_DEP);
            if (namenode!=null) {
                nameform = namenode.getSurface();
                if (isInitial(namenode)) {
                    lastnameInitial = nameform;
                } else {
                    lastname = nameform;
                }
            }
        }
        
        if (this.pTitle == null) {
            namenode = node.getDependentIn(PTITLE_DEP);
            if (namenode!=null) {
                pTitle = namenode.getSurface();
            }
        }
        
        if (gender == Gender.UNDEF) {
            if (node.fMatches(MASC)) {
                gender = Gender.MASC;
            } else if (node.fMatches(FEM)) {
                gender = Gender.FEM;
            }
        }
    }
    
    /** Updates properties of this Person with those available in node
     */
    protected void updateWith(Referent r) {
        String nameform;
        XipNode namenode = null;
        
        if (this.firstname==null) {
            this.firstname = r.getFirstname();
        }
        if (this.firstnameInitial==null) {
            this.firstnameInitial = r.getFirstnameInitial();
        }
        
        if (this.middlename==null) {
            this.middlename = r.getMiddlename();
        }
        if (this.middlenameInitial==null) {
            this.middlenameInitial = r.getMiddlenameInitial();
        }
        
        if (this.lastname==null) {
            this.lastname = r.getLastname();
        }
        if (this.lastnameInitial==null) {
            this.lastnameInitial = r.getLastnameInitial();
        }
        
        if (this.pTitle == null) {
            pTitle = r.getPTitle();
        }
        
        if (gender == Gender.UNDEF) {
            if (r.fMatches(MASC)) {
                gender = Gender.MASC;
            } else if (r.fMatches(FEM)) {
                gender = Gender.FEM;
            }
        }
    }
    
    
    public boolean nameIsCompatibleWith(PersonProperties pp) {
        
        boolean complname=false, compfname1=false, compfname2=false;
        
        //XipNode last2 = node.getDependentIn("INTERNALS_LASTNAME");
        String last2 = pp.getLastname();
        
        if (lastname!=null && last2!=null &&
                !lastname.contentEquals(last2)) {
            // Both have a lastname, and the lastnames are different
            return false;
        }
        // check firstnames
        //XipNode f2 = node.getDependentIn("INTERNALS_FIRSTNAME1");
        String f2 = pp.getFirstname();
        if (firstname!=null && f2!=null &&
                !firstname.contentEquals(f2) &&
                !Util.isNameInitialOf(f2,firstname) &&
                !Util.isNameInitialOf(firstname,f2)   ) {
            return false;
        }
        
        if (lastname==null && last2==null && firstname==null && f2==null ) {
            return false;
        }
        
        if (f2==null && last2==null &&
                firstname!=null && !pp.getNameForm().contentEquals(firstname) )
            // if N2 has no firstname and no lastname, and N1 has a firstname
            // different from the N2 surface form, then FALSE
            return false;
        
        if (f2==null && last2==null &&
                lastname!=null && !pp.getNameForm().contentEquals(lastname) )
            // if N2 has no firstname and no lastname, and N1 has a lastname
            // different from the N2 surface form, then FALSE
            return false;
        
        if (firstname==null && lastname==null &&
                f2!=null && !nameForm.contentEquals(f2) )
            // if N1 has no firstname and no lastname, and N2 has a firstname
            // different from the N1 surface form, then FALSE
            return false;
        
        if (firstname==null && lastname==null &&
                last2!=null && !nameForm.contentEquals(last2) )
            // if N1 has no firstname and no lastname, and N2 has a lastname
            // different from the N1 surface form, then FALSE
            return false;
        
        // check middlenames
        //f2 = node.getDependentIn("INTERNALS_FIRSTNAME2");
        f2 = pp.getMiddlename();
        if (middlename!=null && f2!=null &&
                !middlename.contentEquals(f2) &&
                !Util.isNameInitialOf(f2,middlename) &&
                !Util.isNameInitialOf(middlename,f2) ) {
            return false;
        }
        return true;
    }

    public boolean personIsCompatibleWith(PersonProperties pp) {
        return ((this.nameIsCompatibleWith(pp) || this.nameIsSimilar(pp, 0.25, 0.4, 0.01))
                && genderIsCompatibleWith (pp) );
    }

    public boolean personIsCompatibleWith(PersonProperties pp, Double fNameThreshold, Double mNameThreshold, Double lNameThreshold) {
        return ((this.nameIsCompatibleWith(pp) || this.nameIsSimilar(pp, fNameThreshold, mNameThreshold, lNameThreshold))
                && genderIsCompatibleWith (pp) );
    }

    public boolean genderIsCompatibleWith (PersonProperties pp){
        return (this.gender==pp.gender ||
                     this.gender==Gender.UNDEF ||
                     pp.gender == Gender.UNDEF);
    }

    
    
    protected boolean nameIsCompatibleWith(Referent r) {
        boolean complname=false, compfname1=false, compfname2=false;
        XipNode node = r.getNode();
        
        XipNode last2 = node.getDependentIn("INTERNALS_LASTNAME");
        
        if (lastname!=null && last2!=null &&
                !lastname.contentEquals(last2.getSurface())) {
            // Both have a lastname, and the lastnames are different
            return false;
        }
        // check firstnames
        XipNode f2 = node.getDependentIn("INTERNALS_FIRSTNAME1");
        if (firstname!=null && f2!=null &&
                !firstname.contentEquals(f2.getSurface()) &&
                !Util.isNameInitialOf(f2.getSurface(),firstname) &&
                !Util.isNameInitialOf(firstname,f2.getSurface())   ) {
            return false;
        }
        
        if (lastname==null && last2==null && firstname==null && f2==null ) {
            return false;
        }
        
        if (f2==null && last2==null &&
                firstname!=null && !node.getSurface().contentEquals(firstname) )
            // if N2 has no firstname and no lastname, and N1 has a firstname
            // different from the N2 surface form, then FALSE
            return false;
        
        if (f2==null && last2==null &&
                lastname!=null && !node.getSurface().contentEquals(lastname) )
            // if N2 has no firstname and no lastname, and N1 has a lastname
            // different from the N2 surface form, then FALSE
            return false;
        
        if (firstname==null && lastname==null &&
                f2!=null && !node.getSurface().contentEquals(f2.getSurface()) )
            // if N1 has no firstname and no lastname, and N2 has a firstname
            // different from the N1 surface form, then FALSE
            return false;
        
        if (firstname==null && lastname==null &&
                last2!=null && !node.getSurface().contentEquals(last2.getSurface()) )
            // if N1 has no firstname and no lastname, and N2 has a lastname
            // different from the N1 surface form, then FALSE
            return false;
        
        // check middlenames
        f2 = node.getDependentIn("INTERNALS_FIRSTNAME2");
        if (middlename!=null && f2!=null &&
                !middlename.contentEquals(f2.getSurface()) &&
                !Util.isNameInitialOf(f2.getSurface(),middlename) &&
                !Util.isNameInitialOf(middlename,f2.getSurface()) ) {
            return false;
        }
        return true;
    }
    
    
    
    
    
    /** Returns the Initial form of this person's firstname
     */
    public String getFirstnameInitial() {
        if (firstnameInitial != null)
            return firstnameInitial;
        else if (firstname!=null)
            return getInitialFromName(firstname);
        else
            return null;
    }
    
    /** Returns the Initial form of this person's firstname
     */
    public String getMiddlenameInitial() {
        if (middlenameInitial != null)
            return middlenameInitial;
        else if (middlename!=null)
            return getInitialFromName(middlename);
        else
            return null;
    }
    
    public String getLastnameInitial() {
        if (lastnameInitial != null)
            return lastnameInitial;
        else if (lastname!=null)
            return getInitialFromName(lastname);
        else
            return null;
    }
    
    // Utilities
    /** Returns the Initial form of a name
     */
    private String getInitialFromName(String name) {
        StringBuffer s = new StringBuffer(8);
        if (! Character.isUpperCase(name.charAt(0)) )
            return null;
        s.append(name.charAt(0));
        /* Special case of names starting with Ch or Ph or Th, like Philip */
        if ( (name.charAt(0)=='C' || name.charAt(0)=='P' || name.charAt(0)=='T')
        && (name.charAt(1)=='h' || name.charAt(0)=='H') ) {
            s.append('h');
        }
        s.append('.');
        return s.toString();
    }
    
    private boolean isInitial(XipNode node ) {
        return node.fMatches("INITIAL");
    }
    
    
    String getLastname() { return this.lastname;}
    String getFirstname() { return this.firstname;}
    String getMiddlename() { return this.middlename;}
    String getNameForm() { return this.nameForm;}
    String getPTitle() {
        return this.pTitle;
    }



    public String toString() {
        StringBuilder sb = new StringBuilder(50);
        sb.append("FNAME="+this.getFirstname());
        sb.append("\nLNAME="+this.getLastname());
        sb.append("\n");
        return sb.toString();
    }
    
    public boolean nameIsSimilar(PersonProperties pp, double fNameThreshold,
            double mNameThreshold, double lNameThreshold) {

        boolean fNameCompatibility = false, mNameCompatibility = false, lNameCompatibility = false;

        //Check compatibility of each name atom separately
        if (firstname != null && pp.getFirstname() != null
                && getLevenshteinDistance(firstname, pp.getFirstname()) < fNameThreshold) {
            fNameCompatibility = true;
        }
        if (middlename != null && pp.getMiddlename() != null
                && getLevenshteinDistance(middlename, pp.getMiddlename()) < mNameThreshold) {
            mNameCompatibility = true;
        }
        if (lastname != null && pp.getLastname() != null
                && getLevenshteinDistance(lastname, pp.getLastname()) < lNameThreshold) {
            lNameCompatibility = true;
        }

        //Consider cases that we have no middle or first names
        if ((middlename == null || pp.getMiddlename() == null)
                && lNameCompatibility == true && fNameCompatibility == true) {
            return true;
        } else if ((firstname == null || pp.getFirstname() == null)
                && lNameCompatibility == true) {
            return true;
        } else if (fNameCompatibility && mNameCompatibility && lNameCompatibility) {
            return true;
        } else {
            return false;
        }
    }

    private double getLevenshteinDistance(String s, String t) {

        /*
        * Adapted from
        * http://www.merriampark.com/ldjava.htm (06/10/2009)
        * Added normalisation step and the condition below
        */
        if (s == null || t == null) {
            return 1;
        } else {

            int n = s.length(); // length of s
            int m = t.length(); // length of t

            if (n == 0) {
                return m;
            } else if (m == 0) {
                return n;
            }

            double p[] = new double[n + 1]; //'previous' cost array, horizontally
            double d[] = new double[n + 1]; // cost array, horizontally
            double _d[]; //placeholder to assist in swapping p and d

            // indexes into strings s and t
            int i; // iterates through s
            int j; // iterates through t

            char t_j; // jth character of t

            double cost; // cost

            for (i = 0; i <= n; i++) {
                p[i] = i;
            }

            for (j = 1; j <= m; j++) {
                t_j = t.charAt(j - 1);
                d[0] = j;

                for (i = 1; i <= n; i++) {
                    cost = s.charAt(i - 1) == t_j ? 0 : 1;
                    // minimum of cell to the left+1, to the top+1, diagonally left and up +cost
                    d[i] = Math.min(Math.min(d[i - 1] + 1, p[i] + 1), p[i - 1] + cost);
                }

                // copy current distance counts to 'previous row' distance counts
                _d = p;
                p = d;
                d = _d;
            }

            // our last action in the above loop was to switch d and p, so p now
            // actually has the most recent cost counts

            //Normalised distance according to theory from
            //L. Yujian and L. Bo. A normalized Levenshtein distance metric
            //IEEE Transactions on Pattern Analysis and Machine Intelligence, 29(6):1091–1095, 2007.
            //Should use log maybe? - make distance negative in that case
            double normalisedCost = 2.0 * p[n] / (n + m + p[n]);

            return normalisedCost;
        }
    }
    
    
}
