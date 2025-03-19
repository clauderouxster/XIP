import com.xerox.jxip.*;
import java.io.*;
import java.util.*;

/**
*  Description of the Class
*
* @author     grondeau
* @created    26 juin 2006
*/
public class Testage {
     JXip2 jxip2;

     /**
     *  Description of the Method
     *
     * @param  grmFile  Description of the Parameter
     */
     public Testage(String kifFile) {
          try {
               jxip2 = new JXip2();
               jxip2.KifExecuteProgram(kifFile,"");
               String[] vs=new String[1];
               vs[0]="#readthebook";
               String name="check";
               String res=jxip2.KifExecuteFunction(name,vs);
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
               Testage test = new Testage(args[0]);
          }
          catch (Exception ex) {
               System.out.println(ex);
               ex.printStackTrace();
          }
     }
}







