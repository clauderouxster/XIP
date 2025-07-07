
import com.xerox.jxip.*;

class Test {

    public final static void main(String args[]) {
        try {
            if (args.length != 2) {
                System.out.println("Two parameters are required: the fullpath of the grm file, and the string to parse");
                System.exit(-1);
            }
            JXip jxip = new JXip();
            System.out.println("JXip::main() getVersion = " + jxip.GetVersion());
            System.out.println("JXip::main() Loading grammar file: " + args[0]);
            int gHandler = jxip.XipGrmFile(args[0], 0, true);
            if (gHandler == -1) {
                System.out.println("JXip::main() XipGrmFile call error");
                System.exit(-1);
            }
            String strToParse = args[1];
            System.out.println("Xip::main() parsing the string = " + strToParse);
            long displayMode = Const.DISPLAY_TREE;
            int col = 80;
            jxip.XipSetDisplayMode(gHandler, displayMode, col);
            String res = jxip.parseString(gHandler, strToParse, (char) 1, (char) 1);
            //String res = jxip.parseUtf8String(gHandler, strToParse, Const.XML_NONE, (char) 1);
            System.out.println("\nXip::main() resultat after parsing = " + res);
        } catch (Exception ex) {
            ex.printStackTrace();
            System.exit(-1);
        }
    }
}
