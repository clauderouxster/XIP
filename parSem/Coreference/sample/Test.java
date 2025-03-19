package jcoref;

import java.io.IOException;
import com.xerox.jxip.*;

public class Test {

    public static void main(String[] args) throws IOException {
        CorefResolver coref_resolver = new CorefResolver("../gram/coref.grm");
        coref_resolver.setTrace(false);
        String in = "By choosing Paul Wolfowitz for the post of World Bank president right" +
                " after he nominated John Bolton US ambassador to the United Nations, President George W. Bush" +
                " has signaled his determination to send his administration's hardliners " +
                "to the forefront of the international arena.";
        ReferentList referents = coref_resolver.getReferentListFromString(in, true);
        //ReferentList referents = coref_resolver.getReferentListFromFile("input.txt", true);
        for (Referent r : referents) {
            System.out.println(r.toString());
            //XipNode xipNode = r.getNode();
            //System.out.println("---" + xipNode.toString() + "---");
        }
    }
}