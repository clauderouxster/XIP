
import com.xerox.jxip.*;

public class Test {

    String grm;
    String input;
    JXip jxip;

    public Test(String grm, String input) throws Exception {
        jxip = new JXip();
        System.out.println("JXip::main() getVersion = " + jxip.GetVersion());
        this.grm = grm;
        this.input = input;
        System.out.println("JXip::main() Loading grammar file: " + grm);
    }

    private void createThreads() {
        UnThread thread1 = new UnThread(grm, input);
        UnThread thread2 = new UnThread(grm, input);
        UnThread thread3 = new UnThread(grm, input);
        UnThread thread4 = new UnThread(grm, input);
        UnThread thread5 = new UnThread(grm, input);
        thread1.start();
        thread2.start();
        thread3.start();
        thread4.start();
        thread5.start();
    }

    public static void main(String[] args) throws Exception {
        String grm1 = "../../../GRAMMARS/ENGLISH/GRMFILES/GRAM_NORM/gram_norm_entit.grm";
        String input1 = "The supplied ant shell scripts all support an ANT_OPTS environment "
                + "variable which can be used to supply extra options to ant.";
        Test test = new Test(grm1, input1);
        test.createThreads();
    }

    public class UnThread extends Thread {

        String grm;
        String input;

        public UnThread(String grm, String input) {
            this.grm = grm;
            this.input = input;
        }

        @Override
        public void run() {
            System.out.println("name:" + this.getName() + " priority:" + this.getState());
            try {
                int gHandler = jxip.XipGrmFile(grm, 0, true);
                if (gHandler == -1) {
                    System.out.println("XipGrmFile error: ");
                    System.exit(-1);
                }
                long displayMode = Const.DISPLAY_TREE;
                int col = 80;
                jxip.XipSetDisplayMode(gHandler, displayMode, col);
                String res = jxip.parseString(gHandler, input, (char) 1, (char) 1);
                System.out.println("\nXip::main() resultat after parsing = " + res);
            } catch (Exception ex) {
                System.out.println("Exception: " + ex);
                System.exit(-1);
            }
        }
    }
}
