
package xip2jena_risk;

import com.xerox.xrce.xip2jena.RdfDiff;

public class RunDiff {
    public static void main(String[] args) throws Exception {
        RdfDiff.main(new String[]{"dumpfile.old", "dumpfile"});
    }
}
