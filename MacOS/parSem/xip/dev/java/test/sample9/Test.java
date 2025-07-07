
import com.xerox.jxip.*;

public class Test {

    JXip jxip;
    JXip2 jxip2;

    public Test(String kifFileName) throws Exception {
        jxip = new JXip();
        jxip2 = new JXip2();
        jxip2.KifExecuteProgram(kifFileName, "");
    }

    public String analyze(String sentence) throws Exception {
        String[] parameters = new String[1];
        parameters[0] = sentence;
        String kiffFunctionName = "check";
        String res = jxip2.KifExecuteFunction(kiffFunctionName, parameters);
        return res;
    }

    public final static void main(String args[]) {
        if (args.length != 1) {
            System.out.println("Provide the filename of your KiF program");
            System.exit(-1);
        }
        try {
            Test test = new Test(args[0]);
            System.out.println(test.analyze("#readthebook"));
        } catch (Exception ex) {
            System.out.println(ex);
            System.exit(-1);
        }
    }
}
