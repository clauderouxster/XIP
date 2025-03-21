import com.xerox.jxip.*;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Vector;

public class XipParser {

    XipResultManager xipResultManager;
    int gHandler;
    JXip jxip;


    /**
     *
     * @param grmFile the main file of a grammar
     */
    public XipParser(String grmFile) {
        try {
            jxip =  new JXip();
            //load the grammar
            System.out.println("Loading grammar file: " + grmFile);
            gHandler = jxip.XipGrmFile(grmFile, 0, true);
            // set option: UFT8 charSet
            jxip.XipSetDisplayMode(gHandler, Const.UTF8_INPUT, 80);
        } catch (Exception ex) {
            System.out.println(ex);
            System.exit(-1);
        }
    }

    /**
     * callBack method invoked for each sentence
     */
    public void callBack() {
        try {
            System.out.println("\n ---------------------------- callBack --------------------\n");
            XipResult xipRes = xipResultManager.getXipResult();
            boolean withFeature = true;
            String str = xipRes.toString(withFeature);
            System.out.println("\n ---- the xipResult ----\n" + str);
            String str1 = xipRes.getSentenceString();
            System.out.println("\n ---- the sentence ----\n" + str1);
            Vector<XipUnit> xipUnits = xipRes.getXipUnits();
            for (Enumeration enum1 = xipUnits.elements(); enum1.hasMoreElements();) {
                XipUnit xipUnit = (XipUnit) enum1.nextElement();
                Vector<XipDependency> dependencies = xipUnit.getDependencies();
                for (Enumeration enum2 = dependencies.elements(); enum2.hasMoreElements();) {
                    XipDependency dependency = (XipDependency) enum2.nextElement();
                    System.out.println("dependency: " + dependency.getName() + " " + dependency.getFeatures());
                }
                XipNode xipNode = xipUnit.getRoot();
                Vector<XipNode> daughters = xipNode.getDaughters();
                for (Enumeration enum3 = daughters.elements(); enum3.hasMoreElements();) {
                    xipNode = (XipNode) enum3.nextElement();
                    System.out.println("xipNode: " + xipNode.getCategory() + " " + xipNode.getSurface());
                }
            }
        } catch (Exception ex) {
            System.out.println(ex);
            System.exit(-1);
        }
    }

    public void xmlCallBack() {
        try {
            Hashtable<String, String> xmlTagI = xipResultManager.getXmlTagInfo();
            System.out.println("xmlTagInfo = " + xmlTagI.toString());
        } catch (Exception ex) {
            System.out.println(ex);
            System.exit(-1);
        }
    }

    public void parse(String inputFileName) throws Exception {
        xipResultManager = new XipResultManager();
        // callback usage
        //xipResultManager.xipResultBuilderFromString(gHandler, "This is sentence to parse.", this,  "callBack");
        //xipResultManager.xipResultBuilderFromTextFile(gHandler, inputFileName, this, "callBack");
        xipResultManager.xipResultBuilderFromXmlFile(gHandler, inputFileName, this, "callBack", "xmlCallBack", 0);
    }

    public static void main(String args[]) {
        if (args.length != 1) {
            System.out.println("One parameters is required: the path of the file to parse");
            System.exit(-1);
        }
        try {
            XipParser xipParser = new XipParser("grammars/FRENCH/APPLICATIONS/SCOOP/gram_sentim/french_generique.grm");
            //XipParser xipParser = new XipParser("grammars/ENGLISH/APPLICATIONS/SCOOP/GRMFILES/scoop-complete.grm");
            xipParser.parse(args[0]);
        } catch (Exception ex) {
            System.out.println(ex);
            System.exit(-1);
        }
    }
}
