import com.xerox.jxip.*;
import java.io.*;
import java.util.*;
import java.text.*;
import java.text.*;

/**
 *  Description of the Class
 *
 * @author     grondeau
 * @created    16 octobre 2006
 */
public class Test {
	public final static void main(String args[]) {
		if (args.length != 2) {
			System.out.println("Two parameters are required: the path of the grm file, and the directory of the files to parse");
			System.exit(-1);
		}
		try {
			final String suffix = ".txt";
			Test test = new Test(args[0]);
			String dirName = args[1];
			File dir = new File(dirName);
			FilenameFilter filter = new FilenameFilter() {
        			public boolean accept(File dir, String name) {
            				return name.endsWith(suffix);
        			}
			};
			String[] children = dir.list(filter);
			if (children == null) {
				System.out.println("diretory doesn't exist!:" + dirName);
				System.exit(-1);
			}
			if (children.length == 0) {
				System.out.println("diretory empty: " + dirName  + " or no file with the suffix: " + suffix);
				System.exit(-1);
			}
			CorefResolver coref = new CorefResolver("../../../GRAMMARS/ENGLISH/APPLICATIONS/JCOREF/GRM/coref.grm");
			long startTime = System.currentTimeMillis();
			for (int i=0; i<children.length; i++) {
				// Get filename of file or directory
				String fileName = args[1] + "/" + children[i];
				System.out.println("------ processing file: " +i +"   " + fileName);
				coref.init();
        			coref.resolveCoreferenceInFile();
				long endTime = System.currentTimeMillis();
				long elapsed = endTime - startTime;
				SimpleDateFormat dateFormat = new SimpleDateFormat("HH:mm:ss");
				dateFormat.setTimeZone(TimeZone.getTimeZone("GMT"));
				System.out.println("elapsed time:" + dateFormat.format(new Date(elapsed)));
			}
		} catch (Exception ex) {
			System.out.println(ex);
			ex.printStackTrace();
			System.exit(-1);
		}
	}
}
 
