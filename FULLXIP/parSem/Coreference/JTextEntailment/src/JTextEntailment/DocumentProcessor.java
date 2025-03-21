/*
 * DocumentProcessor.java
 *
 * Created on May 22, 2007, 4:00 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package JTextEntailment;

import com.xerox.jxip.*;
import java.io.File;
import java.io.PrintStream;
import java.util.Arrays;
import java.util.Hashtable;
import java.util.Vector;

/**
 *
 * @author ait
 */
public class DocumentProcessor {

    //Set<String> passageCategories = new Set<String> {};
    static final private Vector<String> passageCategories = new Vector<String>(Arrays.asList("NV", "PV", "GN", "GP", "GA", "GR"));

    private class CatInfo {

        public long globalNb = 0;
        public long localNb = 0;

        public CatInfo() {
            globalNb = 0;
            localNb = 0;
        }

        private void incrementNb() {
            localNb++;
            globalNb++;
        }
    }
    Hashtable<String, CatInfo> catInfo = new Hashtable<String, CatInfo>(15);

    private class RelationInfo {

        String arg1name;
        String arg2name;
        String arg3name;
        long globalNb = 0;
        long localNb = 0;

        public RelationInfo(String n1, String n2, String n3) {
            arg1name = n1;
            arg2name = n2;
            arg3name = n3;
        }

        private void incrementNb() {
            localNb++;
            globalNb++;
        }
    }
    Hashtable<String, RelationInfo> relInfo = new Hashtable<String, RelationInfo>(30);
    private String grmFile;
    private JXip jxip;
    private int gHandler;
    private XipResultManager xipResultManager;
    private int sentenceNumber = 0;
    private int chunkNumber = 0;
    private int formNumber = 0;
    private String inputFileName = "";
    private String sentenceId = "E1";
    private XipResult xipResult;
    private PrintStream resultFile = System.out;

    /**
     * Creates a new instance of DocumentProcessor
     */
    public DocumentProcessor(String xip_grammar_file) {
        this.setGrammar(xip_grammar_file);
        this.sentenceNumber = 0;
        initInfo();
    }

    private void initInfo() {
        catInfo.put("GN", new CatInfo());
        catInfo.put("GP", new CatInfo());
        catInfo.put("NV", new CatInfo());
        catInfo.put("GA", new CatInfo());
        catInfo.put("PV", new CatInfo());
        catInfo.put("GR", new CatInfo());

        relInfo.put("SUJ-V", new RelationInfo("sujet", "verbe", ""));
        relInfo.put("AUX-V", new RelationInfo("auxiliaire", "verbe", ""));
        relInfo.put("COD-V", new RelationInfo("cod", "verbe", ""));
        relInfo.put("CPL-V", new RelationInfo("complement", "verbe", ""));
        relInfo.put("MOD-V", new RelationInfo("modifieur", "verbe", ""));
        relInfo.put("COMP", new RelationInfo("complementeur", "verbe", ""));
        relInfo.put("ATB-SO", new RelationInfo("attribut", "verbe", ""));  //!! 3eme arg
        relInfo.put("MOD-N", new RelationInfo("modifieur", "nom", ""));
        relInfo.put("MOD-A", new RelationInfo("modifieur", "adjectif", ""));
        relInfo.put("MOD-R", new RelationInfo("modifieur", "adverbe", ""));
        relInfo.put("MOD-P", new RelationInfo("modifieur", "preposition", ""));
        relInfo.put("COORD", new RelationInfo("coordonnant", "coord-g", "coord-d"));
        relInfo.put("APPOS", new RelationInfo("premier", "appose", ""));
        relInfo.put("JUXT", new RelationInfo("premier", "suivant", ""));
    }

    private void resetInfo() {
        for (CatInfo cinfo : catInfo.values()) {
            cinfo.localNb = 0;
        }
        for (RelationInfo rinfo : relInfo.values()) {
            rinfo.localNb = 0;
        }
    }

    public void printGlobalInfo() {
        for (String key : catInfo.keySet()) {
            CatInfo cinfo = catInfo.get(key);
        }
        for (String key : relInfo.keySet()) {
            RelationInfo rinfo = relInfo.get(key);
        }
    }

    public void setGrammar(String xip_grammar_file) {
        xipResultManager = new XipResultManager();
        this.grmFile = xip_grammar_file;
        // loads the xip grammar
        try {
            System.out.println("JPASSAGE: Loading grammar file: " + grmFile);
            // create a JXIP object to load the dynamic library in the VM
            jxip = new JXip();
            System.out.println(jxip.GetVersion());
            // load the grammar
            gHandler = jxip.XipGrmFile(grmFile, 0, true);
        } catch (Exception ex) {
            System.out.println(ex);
            ex.printStackTrace();
        }
    }



    /* Process the document with this sentence processor (parsing)
     **/
    void processDocument(File file, boolean utf8flag, boolean xmlflag, String outputfilepath) {
        //System.err.println("Processing: " + file.getAbsolutePath() + "...");
        this.sentenceNumber = 0;
        this.chunkNumber = 0;
        this.formNumber = 0;
        if (utf8flag) {
            try {
                jxip.XipSetDisplayMode(gHandler, Const.UTF8_INPUT, 50);
            } catch (Exception ex) {
                ex.printStackTrace();
            }
        }

        try {
            this.inputFileName = file.getAbsolutePath();
            String resultDocHead = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" +
                    "<!DOCTYPE DOCUMENT SYSTEM \"easy.dtd\">\n" +
                    "<DOCUMENT fichier=\"" + file.getName() + "\" id=\"xrce\" xmlns:xlink=\"http://www.w3.org/1999/xlink\">";
            String resultDocEnd = "</DOCUMENT>";

            //jxip.XipSetDisplayMode(gHandler, Const.DISPLAY_LEMMA, 50);
            // Create the result file
            //this.resultFile = System.out;
            File outfile = new File(outputfilepath);
            File outparent = outfile.getParentFile();
            if (!outparent.exists()) {
            }
            this.resultFile = new PrintStream(outfile.getAbsolutePath(), "UTF-8");
            // call XIP with current grammar to parse the doc
            this.resultFile.println(resultDocHead);
            if (xmlflag) {
                xipResultManager.xipResultBuilderFromXmlFile(gHandler, this.inputFileName,
                        this, "processXmlSentence", "processXmlTag", 0);
            } else {
                xipResultManager.xipResultBuilderFromTextFile(gHandler, this.inputFileName,
                        this, "processTxtSentence");
            }

            this.resultFile.println(resultDocEnd);
            this.resultFile.close();
        } catch (Exception ex) {
            ex.printStackTrace();
        }

        if (utf8flag) {
            try {
                jxip.XipRemoveFlagDisplay(gHandler, Const.UTF8_INPUT);
            } catch (Exception ex) {
                ex.printStackTrace();
            }
        }

    }

    /** This method is invoked by the XIP callback mecanism for each
     * processed XML tag. Here used to print out the parsing results in the
     * PASSAGE format */
    public void processXmlTag() {
        Hashtable<String, String> xmlTag = this.xipResultManager.getXmlTagInfo();
        if (xmlTag.get("tagName").contentEquals("E")) {
            this.sentenceId = xmlTag.get("ID");
            if (this.sentenceId == null) {
                this.sentenceId = xmlTag.get("id");
            }
            if (this.sentenceId == null) {
                this.sentenceId = xmlTag.get("Id");
            }
            XipUnit xipunit = (XipUnit) this.xipResult.getXipUnits().elementAt(0);
            passagePrintSentence(xipunit);
        }
    }

    /** This method is invoked by the XIP callback mecanism for each
     * parsed sentence for XML input. */
    public void processXmlSentence() {
        this.sentenceNumber++;
        // this.sentenceId = "E" + this.numberOfSentences;
        this.chunkNumber = 0;
        this.formNumber = 0;
        this.xipResult = this.xipResultManager.getXipResult();
        //XipUnit xipunit = (XipUnit) this.xipResult.getXipUnits().elementAt(0);
        //System.err.println(xipunit.getRoot().toString(false, "  ", true));
    //System.err.println(xipunit.toString(true));

    }

    /** This method is invoked by the XIP callback mecanism for each
     * parsed sentence for txt input. */
    public void processTxtSentence() {
        this.sentenceNumber++;
        this.sentenceId = "E" + this.sentenceNumber;
        this.chunkNumber = 0;
        this.formNumber = 0;
        this.xipResult = this.xipResultManager.getXipResult();
        XipUnit xipunit = (XipUnit) this.xipResult.getXipUnits().elementAt(0);
        //System.err.println(xipunit.getRoot().toString(false, "  ", true));
        passagePrintSentence(xipunit);
    //System.err.println(xipunit.toString(true));

    }

    private void passagePrintSentence(XipUnit xipunit) {
        //System.err.println("E id="+this.sentenceId);
        this.resultFile.println("<E id=\"" + this.sentenceId + "\">");
        //this.resultFile.println("  <constituants>");
        passagePrintConstituents(xipunit.getRoot(), "  ", 0);
        //this.resultFile.println("  </constituants>");
        this.resultFile.println("  <relations>");
        passagePrintRelations(xipunit.getDependencies());
        this.resultFile.println("  </relations>");
        this.resultFile.println("</E>");


    }

    private void passagePrintConstituents(XipNode node, String indent, int depth) {
        if (node.isPreTerminalNode()) {
            this.passagePrintForms(node, indent);
        } else {
            CatInfo catinfo = catInfo.get(node.getCategory());
            if (depth == 0 && catinfo != null) {
                catinfo.incrementNb();
                this.chunkNumber++;
                node.setMiscInfo(this.sentenceId + "G" + this.chunkNumber);
                resultFile.println(indent + "  <Groupe type=\"" + node.getCategory() +
                        "\" id=\"" + (String) node.getMiscInfo() + "\">");
                for (XipNode n : node.getDaughters()) {
                    this.passagePrintConstituents(n, indent + "  ", depth + 1);
                }
                resultFile.println(indent + "  </Groupe>");
            } else {
                for (XipNode n : node.getDaughters()) {
                    this.passagePrintConstituents(n, indent, depth);
                }
            }
        }
    }

    private void passagePrintForms(XipNode node, String indent) {
        this.formNumber++;
        node.setMiscInfo(this.sentenceId + "F" + this.formNumber);
        String s = node.getSurface();
        s =
                s.replace("<", "&lt;");
        s =
                s.replace(">", "&gt;");
        s =
                s.replace("&", "&amp;");
        resultFile.println(indent + "  <F id=\"" + node.getMiscInfo() + "\">" + s + "</F>");
    }

    private boolean checkFlatChunk(XipNode node) {
        for (XipNode n : node.getDaughters()) {
            if (!n.isPreTerminalNode()) {
                return false;
            }

        }
        return true;
    }

//////////////////////////////////////
    private void passagePrintRelations(Vector<XipDependency> deps) {
        RelationInfo relinfo;
        int i = 1;
        for (XipDependency d : deps) {

            relinfo = getPassageWellFormedRelationInfo(d);
            if (relinfo == null) {
                continue;
            }

//System.err.println("DEP:"+d.getName());
            resultFile.println("    <relation xlink:type=\"extended\" type=\"" +
                    d.getName() + "\" id=\"" + this.sentenceId + "R" + i + "\">");
            resultFile.println("      <" + relinfo.arg1name + " xlink:type=\"locator\" xlink:href=\"" +
                    d.getArg(1).getMiscInfo() + "\"/>");
            resultFile.println("      <" + relinfo.arg2name + " xlink:type=\"locator\" xlink:href=\"" +
                    d.getArg(2).getMiscInfo() + "\"/>");

            // ATB-SO: cas particulier, preciser le s-o
            if (d.getName().contentEquals("ATB-SO")) {
                if (d.hasFeature("spred", "+")) {
                    resultFile.println("      <s-o valeur=\"sujet\"/>");
                } else {
                    resultFile.println("      <s-o valeur=\"objet\"/>");
                }

            } else if (d.getName().contentEquals("MOD-N")) {
                // MOD-N : preciser a-propager
                if (d.hasFeature("apropager", "+")) {
                    resultFile.println("      <a-propager booleen=\"vrai\"/>");
                } else {
                    resultFile.println("      <a-propager booleen=\"faux\"/>");
                }

            }

            // Relation avec 3 args (COORD)
            if (d.getParameters().size() == 3) {
                resultFile.println("      <" + relinfo.arg3name + " xlink:type=\"locator\" xlink:href=\"" +
                        d.getArg(3).getMiscInfo() + "\"/>");
            }

            resultFile.println("    </relation>");
            i++;

        }




    }

    private RelationInfo getPassageWellFormedRelationInfo(XipDependency d) {
        RelationInfo relinfo;

        relinfo = this.relInfo.get(d.getName());
        relinfo.incrementNb();
        

// one arg node is equal to null

        // one arg node ID is equal to null
        if (!checkArgId(d, 1) || !checkArgId(d, 2) || !checkArgId(d, 3)) {
            return null;
        }

        return relinfo;
    }

// Check if arg i of dep d has a passage ID
    private boolean checkArgId(XipDependency d, int i) {
        if (i == 3 && d.getParameters().size() < 3) {
            return true;
        }

            return true;

    }

    private String getArgString(XipDependency d, int i) {
        if ((i == 3) && d.getParameters().size() == 2) {
            return "No_3rd_Arg";
        }

        return (d.getArg(i) == null ? "NULL" : (d.getArg(i).getId() +
                ":" + d.getArg(i).getSurface()));
    }
}
