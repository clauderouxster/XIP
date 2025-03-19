import com.xerox.jxip.*;

public class Parser {

    private JXip jxip;
    private int gHandler;

    public Parser(String grmFile) throws Exception {
        jxip = new JXip();
	gHandler = jxip.XipGrmFile(grmFile, 0, true);
        System.out.println("\n\n------------" + jxip.GetVersion() + "------------\n\n");
    }

    public void parse(String inputXmlFileName, String outputXmlFileName) throws Exception {
        jxip.XipSetDisplayMode(gHandler, Const.CHECK_INPUT_UTF8, 80);
        jxip.XipParseFileXMLToFile(gHandler, inputXmlFileName, outputXmlFileName, 2, Const.XML_OUTPUT, (char)1);
    }

 public static void main(String args[]) throws Exception {
	//String grm = "../../../grammar/french/BASIC/french_entityOnly.grm";
	String grm = "../../../grammar/english/entity/gram_gen_entit.grm";
	Parser parser = new Parser(grm);
	parser.parse("input.xml", "ouput.xml");
 }
}
