import com.xerox.jxip.*;

class Test {

	public final static void main(String args[]) {
		try {
			if (args.length != 2) {
				System.out.println("Two parameters are required: the fullpath of the grm file, and the string to parse");
				System.exit(-1);
			}
			System.out.println("JXip::main() Loading grammar file: " + args[0]);
			System.out.println("Xip::main() parsing the string = " + args[1]);
			// create a JXIP object to load the dynamic library in the VM
			JXip jxip = new JXip();
			XipResultManager xipResultManager = new XipResultManager();
			int gHandler = jxip.XipGrmFile(args[0], 0, true);
			int res = xipResultManager.buildXipResult(gHandler, args[1]);
			if (res != 0) {
				System.out.println("XipResultManager::main() buildXipResult ERROR");
				System.exit(-1);
			}
			XipResult xipRes = xipResultManager.getXipResult();
			boolean withFeature = true;
			String str = xipRes.toString(withFeature);
			System.out.println("\n" + str);
		} catch (Exception ex) {
			ex.printStackTrace();
			System.exit(-1);
		}
	}
}


