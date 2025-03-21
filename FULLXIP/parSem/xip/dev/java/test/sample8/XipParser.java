
import com.xerox.jxip.*;

public class XipParser {

    private static int gHandler;
    private static JXip jxip;
    private static JXip2 jxip2;

    /**
     *
     * @throws Exception
     */
    public XipParser(String grm) throws Exception {
        jxip = new JXip();
        jxip2 = new JXip2();
        System.out.println(jxip.GetVersion());
        gHandler = jxip.XipGrmFile(grm, 0, true);
        jxip2.KifExecuteProgram("featuresExtraction.kif", String.valueOf(gHandler));
    }
    
    public void callBack() throws Exception {}

    /**
     *
     * @param sentence
     * @return a list of featureName (String) separate by "\n"
     * @throws Exception
     */
    public String parseSentence(String sentence) throws Exception {
        String[] params = new String[1];
        params[0] = sentence;
        String featuresStr = jxip2.KifExecuteFunction("parseString", params);
        return featuresStr;
    }

    public final static void main(String args[]) throws Exception {
        XipParser xipParser = new XipParser(args[0]);
        System.out.println(xipParser.parseSentence("This is a test."));
    }
}
