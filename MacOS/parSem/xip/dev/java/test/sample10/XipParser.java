
import com.xerox.jxip.*;
import java.util.Arrays;
import java.util.HashSet;
import java.util.Hashtable;

public class XipParser {

    XipResultManager xipResultManager;
    int gHandler;
    JXip jxip;
    //private static final HashSet<String> catToRemove = new HashSet(Arrays.asList("OPINION-HOLDER", "EXPERIENCER", "SPEAKER", "OPINION-TARGET")); //for french
	private static final HashSet<String> catToRemove = new HashSet(Arrays.asList());

    /**
     *
     * @param grmFile the main file of a grammar
     */
    public XipParser(String grmFile) {
        try {
            jxip = new JXip();
            System.out.println("Loading grammar: " + grmFile);
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
            for (XipUnit xipUnit : xipResultManager.getXipResult().getXipUnits()) {
                for (XipLeaf xipLeaf : xipUnit.getLeaves()) {
                    if (!catToRemove.contains(xipLeaf.getCategory())) {
                        System.out.println(xipLeaf.getCategory() + "\t\t" + xipLeaf.getSurface() + "\t\t" + xipLeaf.getLemma());
                    }
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
            //System.out.println("xmlTagInfo = " + xmlTagI.toString());
        } catch (Exception ex) {
            System.out.println(ex);
            System.exit(-1);
        }
    }

    public void parse(String sentence) throws Exception {
        xipResultManager = new XipResultManager();
        xipResultManager.xipResultBuilderFromString(gHandler, sentence, this, "callBack");
        //xipResultManager.xipResultBuilderFromTextFile(gHandler, inputFileName, this, "callBack");
        //xipResultManager.xipResultBuilderFromXmlFile(gHandler, inputFileName, this, "callBack", "xmlCallBack", 0);
    }

    public static void main(String args[]) {
        if (args.length != 1) {
            System.out.println("One parameters is required: the path of the grm file");
            System.exit(-1);
        }
        try {
            XipParser xipParser = new XipParser(args[0]);
            String sentence = "Although it's a black art, at this point some kind of basic scheduling is important.";
            xipParser.parse(sentence);
        } catch (Exception ex) {
            System.out.println(ex);
            System.exit(-1);
        }
    }
}