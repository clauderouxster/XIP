/*
 * SentenceParse.java
 *
 * Created on June 9, 2006, 5:35 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package jcoref;

import java.util.Iterator;
import java.util.Vector;
import com.xerox.jxip.*;

/**
 *
 * @author ait
 */
public class SentenceParse {
    
    /** The Xip Parse of this sentence */
    private XipUnit xipunit=null;
    
    /** Sentence order number */
    public long sentenceId;
    
    /** Coreferent links */
    private Vector<Vector<CoreferenceLink>> nodeCorefLinks;
    
    private class Quotation {
        public long loffset=0;
        public long roffset=0;
        public ReferentNode sayerNode=null;
    }
    
    /** Span interval and Sayer Node of quotations occuring in this sentence */
    private Vector<Quotation> quotations = new Vector<Quotation>(0);
    
    /** Creates a new instance of SentenceParse */
    public SentenceParse(XipUnit xipunit, int number) {
        this.xipunit = xipunit;
        this.sentenceId = number;
        // Check and register quotation portions, if any
        Vector<XipLeaf> leaves = xipunit.getLeaves();
        XipLeaf lf;
        Quotation quot = null;
        for(int i=0; i < leaves.size() && leaves.elementAt(i).getRight()>=0; i++) {
            lf = leaves.elementAt(i);
            if (lf.fMatches("GUILLE","+")) {
                if (quot==null) {
                    quot = new Quotation();
                    quot.loffset = lf.getLeft();
                    quot.roffset = lf.getRight();
                    this.quotations.add(quot);
                } else {
                    quot.roffset = lf.getRight();
                }
            } else {
                if (quot!=null) {
                    quot = null;
                }
            }
        }
        // Now, for each quotation set the SAYER node if it is specified
        // within a "SAY" dependency
        Vector<XipDependency> deps = this.getDependencies();
        for(int i=0; i < this.quotations.size(); i++) {
            quot = this.quotations.elementAt(i);
            for(int j=0; j<deps.size() && quot.sayerNode==null; j++) {
                XipDependency dep = deps.elementAt(j);
                if (dep.getName().compareTo("SAY")==0) {
                    Vector<XipNode> args = dep.getParameters();
                    if (args.size()==2 &&
                            args.elementAt(1).getLeft() >= quot.loffset &&
                            args.elementAt(1).getRight() <= quot.roffset ) {
                        XipNode xsayer = args.elementAt(0);
                        ReferentNode rsayer = (ReferentNode) xsayer.getMiscInfo();
                        if (rsayer==null) {
                            rsayer = new ReferentNode(xsayer,this.sentenceId);
                            xsayer.setMiscInfo(rsayer);
                        }
                        quot.sayerNode = rsayer;
                    }
                }
            }
        }
        
    }
    
    
    /** */
    public  XipNode getRoot() {
        return this.xipunit.getRoot();
    }
    public java.util.Vector<XipDependency> getDependencies() {
        return this.xipunit.getDependencies();
    }
    public java.util.Vector<XipLeaf> getLeaves() {
        return this.xipunit.getLeaves();
    }
    public java.lang.String getSentenceString() {
        return this.xipunit.getRoot().getSurface();
    }
    
    
    /** returns true iff this sentence starts with a quotation, false otherwise */
    public boolean startsWithQuotes() {
        if (this.quotations.isEmpty() ||
                (this.quotations.elementAt(0).loffset > this.getLeaves().elementAt(0).getLeft() ) ) {
            return false;
        } else {
            return true;
        }
    }
    
    /** Returns true iff the given node occurs within quotes in this sentence */
    public boolean isInsideQuotes(XipNode node) {
        if (this.quotations.isEmpty()) {
            return false;
        } else {
            if (node.getRight()<=this.quotations.elementAt(0).loffset ||
                    node.getLeft()>=this.quotations.elementAt(0).roffset)
                return false;
            else
                return true;
        }
    }
    
    /** Returns the sayer node of quotation i of this sentence */
    public ReferentNode getSayerNode(int i) {
        if (this.quotations.isEmpty() || this.quotations.size() <= i) {
            return null;
        } else {
            return this.quotations.elementAt(i).sayerNode;
        }
    }
    
    /** Sets the sayer node of quoation i to sayer_node */
    public void setSayerNode(int i, ReferentNode sayer_refnode) {
        if (this.quotations.isEmpty() || this.quotations.size() <= i) {
            return;
        } else {
            this.quotations.elementAt(i).sayerNode = sayer_refnode;
        }
    }
    
    
    /** returns a string representation of the sentence parse */
        public String toString() {
        String s = this.sentenceId + ": ";
        s = s + this.getSentenceString() + "\n";
        s = s + this.getRoot().toString2() + "\n"
              + this.getQuotationString();
        return s;
    }
    
    private static final String csshead =
        "<head> \n<style type=\"text/css\">\n" +
        ".category {background-color:yellow;font-size:120%;font-weight:bold} \n" +
        ".index {color:red;font-weight:bold} \n" +
        ".indexindep {color:black;font-weight:bold} \n" +
        ".surface {font-weight:bold} \n" +
        ".surfaceindep {font-style:italic} \n" +
        ".depname {color:blue;font-weight:bold} \n" +
        "</style> \n</head> \n\n";
    
    public String toHtmlString() {
        String s = "<HTML>\n" + csshead + "<BODY>\n";
        s = s + "<H3>" + this.getSentenceString() + "</H3>";
        s = s + "<BR>\n"
              + this.getRoot().toHtmlString(true,true)
              + "<BR>\n"
              + this.getQuotationString();
        s = s + "</BODY> </HTML>";
        return s;
    }
    
    
    private String getLeavesString() {
        Vector<XipLeaf> leaves = this.xipunit.getLeaves();
        XipLeaf lf = null;
        String s = "";
        for(int i=0; i<leaves.size(); i++) {
            lf = leaves.elementAt(i);
            s = s + lf.getSurface() + " " + lf.getCategory() + " "
                    + lf.featuresToString(true) + "\n";
        }
        return s;
    }
    
    private String getQuotationString() {
        Quotation q;
        XipLeaf lf = null;
        long roffset=0;
        String s = "";
        String sayer="";
        Iterator<Quotation> qiter = quotations.iterator();
        while (qiter.hasNext()) {
            s = s + "\"";
            q = qiter.next();
            roffset = 0;
            Iterator<XipLeaf> lfiter = this.getLeaves().iterator();
            while ( lfiter.hasNext() && ((lf=lfiter.next())!=null) && lf.getLeft() < q.loffset ) ;
            while ( lfiter.hasNext() && lf.getRight() <= q.roffset ) {
                if (lf.getRight() > roffset) { // to avoid form duplications due to POS ambiguity
                    s = s + lf.getSurface() + " ";
                    roffset = lf.getRight();
                }
                lf = lfiter.next();
            }
            if (q.sayerNode!=null) {
                sayer = q.sayerNode.toLightValueString();
            } else {
                sayer = "";
            }
            s = s + "\" (" + q.loffset + "-" + q.roffset + ") SaidBy: "
                    +  sayer + "\n";
        }
        return s;
    }
    
    ReferentNode getSayerNodeOf(XipNode n) {
        for(int i=0; i<this.quotations.size();i++) {
            if (n.getLeft()>=this.quotations.elementAt(0).loffset &&
                    n.getRight()<=this.quotations.elementAt(0).roffset)
                return this.quotations.elementAt(i).sayerNode;
        }
        return null;
    }

    public String toFullParseString() {
        return toString();
    }

    public long getLeft() {
        return this.xipunit.getRoot().getLeft();
    }
    
    public long getRight() {
        return this.xipunit.getRoot().getRight();
    }
    
    public int getStart() {
        return (int) getLeft();
    }
    
    public int getEnd() {
        return (int) getRight();
    }
    
    public String getCtrlClickText() {
        return toHtmlString();
    }
    
    public XipUnit getXipUnit() {
	return xipunit;
    }

}



