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
               //int xiphandler= jxip.XipGrmFile(chemingrm,0,true);

               jxip=new JXip();
               jxip2 = new JXip2();
               String chemingrm="/opt/ParSem_db/users/Roux/Xi NLU/ENGLISH/APPLICATIONS/SCOOP/GRMFILES/sp.grm";
               int kifhandler=jxip2.KifExecuteProgram(kifFile,chemingrm);
               String[] vs={"I have a Samsung galaxy SII."};
               String name="detect";
               String res=jxip2.KifExecuteFunctionFrom(kifhandler,"detect",vs);
               System.out.println("Notre resultat:"+res);
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
