
import com.xerox.jxip.*;

class TestLoadGram {

    public final static void main(String args[]) {
        try {
            JXip jxip = new JXip();
	    System.out.println(jxip.GetVersion());
	    String grm = "../../../GRAMMARS/PORTUGUESE/GRMFILES/portuguese.grm";	
            int gHandler = jxip.XipGrmFile(grm, 0, true);
	    jxip.XipSetDisplayMode(gHandler, Const.DISPLAY_NONE, 80);
            if (gHandler == -1) {
                System.out.println("JXip::main() XipGrmFile call error");
                System.exit(-1);
            }
        } catch (Exception ex) {
            ex.printStackTrace();
            System.exit(-1);
        }
    }
}
