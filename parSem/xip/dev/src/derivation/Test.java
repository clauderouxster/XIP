
import com.xerox.jxip.JXip;

class Test {

    public static void main(String args[]) {
        try {
            JXip jxip = new JXip();
            System.out.println(jxip.GetVersion());
            int gHandler = jxip.XipGrmFile(args[0], 0, true);
            if (gHandler == -1) {
                System.out.println("XipGrmFile call error");
                System.exit(-1);
            }
            String strToParse = "This is a test.";
            System.out.println("Input: \n" + strToParse);
            int col = 80;
            int displayMode = 0;
            jxip.XipSetDisplayMode(gHandler, displayMode, col);
            String res = jxip.parseString(gHandler, strToParse, (char) 0, (char) 1);
            System.out.println("\n\nOutput: \n" + res);
        } catch (Exception ex) {
            ex.printStackTrace();
            System.exit(-1);
        }
    }
}