import com.xerox.jxip.*;
import java.io.*;
import java.util.*;

/**
*  Description of the Class
*
* @author     grondeau
* @created    26 juin 2006
*/
public class Test {
	 JXip jxip;
     JXip2 jxip2;

     /**
     *  Description of the Method
     *
     * @param  grmFile  Description of the Parameter
     */
     public Test(String kifFile) {
          try {
			   jxip=new JXip();
			   String grm1 = "/opt/ParSem/users/Roux/xip/parSem/xip/dev/GRAMMARS/FRENCH/BASIC/french.grm";	
			   String grm2 = "/opt/ParSem/users/Roux/xip/parSem/xip/dev/GRAMMARS/FRENCH/BASIC/french_entity.grm";	

			   int gfr1 = jxip.XipGrmFile(grm1, 0, true);
			   int gfr2 = jxip.XipGrmFile(grm2, 0, true);
               jxip2 = new JXip2();

			   String kifFile1 = "/opt/ParSem/users/Roux/xip/parSem/xip/dev/java/javakif/ajoute1.kif";
			   String kifFile2 = "/opt/ParSem/users/Roux/xip/parSem/xip/dev/java/javakif/ajoute2.kif";

               int kifhandler1=jxip2.KifExecuteProgram(kifFile1,"0");
			   int kifhandler2=jxip2.KifExecuteProgram(kifFile2,"1");
          }
          catch (Exception ex) {
               System.out.println(ex);
               ex.printStackTrace();
          }
     }




     /**
     *  Description of the Method
     *
     * @param  args  Description of the Parameter
     */
     public final static void main(String args[]) {
          if (args.length != 1) {
               System.out.println("Provide the filename of your KiF program");
               System.exit(-1);
          }
          try {
               Test test = new Test(args[0]);
          }
          catch (Exception ex) {
               System.out.println(ex);
               ex.printStackTrace();
          }
     }
}









