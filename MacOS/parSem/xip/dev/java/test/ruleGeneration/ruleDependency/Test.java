import com.xerox.jxip.*;
import java.io.*;
import java.util.*;

/**
 *  Description of the Class
 *
 * @author     grondeau
 * @created    25 avril 2006
 */
public class Test {

	/**
	 *  Description of the Method
	 *
	 * @param  node             Description of the Parameter
	 * @param  selectedObjects  Description of the Parameter
	 */
	static void selectXipNodes(XipNode node, Vector selectedObjects) {
		//System.out.println("RuleBuilder::selectXipNodes() node: " + node.getCategory());
		SelectedObject selectedObject = new SelectedObject(node);
		selectedObject.setAny(false);
		// select all the features of this node
		for (Enumeration enumeration = (node.getFeatures()).elements(); enumeration.hasMoreElements(); ) {
			XipFeature xipFeature = (XipFeature) enumeration.nextElement();
			selectedObject.addSelectedFeature(xipFeature);
		}
		selectedObjects.add(selectedObject);
		// process recursively the daughters
		for (Enumeration enumeration = (node.getDaughters()).elements(); enumeration.hasMoreElements(); ) {
			XipNode daughter = (XipNode) enumeration.nextElement();
			selectXipNodes(daughter, selectedObjects);
		}
	}


	/**
	 *  Description of the Method
	 *
	 * @param  args  Description of the Parameter
	 */
	public final static void main(String args[]) {
		// Vector of SelectedObject
		Vector selectedObjects = new Vector();
		try {
			if (args.length != 2) {
				System.out.println("Two parameters are required: the fullpath of the grm file, and the string to parse");
				System.exit(-1);
			}
			System.out.println("RuleBuilder::main() Loading grammar file: " + args[0]);
			System.out.println("RuleBuilder::main() parsing the string = " + args[1]);
			// create a JXIP object to load the dynamic library in the VM
			JXip jxip = new JXip();
			XipResultManager xipResultManager = new XipResultManager();
			int gHandler = jxip.XipGrmFile(args[0], 0, true);
			int res = xipResultManager.buildXipResult(gHandler, args[1]);
			if (res != 0) {
				System.out.println("RuleBuilder::main() buildXipResult ERROR");
				System.exit(-1);
			}
			XipResult xipRes = xipResultManager.getXipResult();
			RuleBuilder ruleBuilder = new RuleBuilder();
			// build a vector of selected object
			for (Enumeration enumeration = (xipRes.getSentences()).elements(); enumeration.hasMoreElements(); ) {
				XipUnit xipUnit = (XipUnit) enumeration.nextElement();
				XipNode rootNode = xipUnit.getRoot();
				// select the xipNodes and xipLeaves with all their features
				selectXipNodes(rootNode, selectedObjects);
				// select the dependencies with all their features
				for (Enumeration enumeration1 = (xipUnit.getDependencies()).elements(); enumeration1.hasMoreElements(); ) {
					XipDependency dependency = (XipDependency) enumeration1.nextElement();
					SelectedObject selectedObject = new SelectedObject(dependency);
					// select all the features of this dependency
					for (Enumeration enumeration2 = (dependency.getFeatures()).elements(); enumeration2.hasMoreElements(); ) {
						XipFeature xipFeature = (XipFeature) enumeration2.nextElement();
						selectedObject.addSelectedFeature(xipFeature);
					}
					selectedObjects.add(selectedObject);
				}
			}
			Vector focus = new Vector();
			focus.add(new Long(2));
			focus.add(new Long(5));
			int ruleType;
			ruleType = Const.DEPENDENCYRULE;
			//ruleType = Const.SEQUENCERULE;
			//ruleType = Const.LEFTCONTEXT;
			//ruleType = Const.RIGHTCONTEXT;
			//ruleType = Const.IDRULE;
			//ruleType = Const.TAGGINGRULE;
			int dependencyArity = 2;
			long boundaryIndex = 3;
			String category = "focus";
			//generate the rule String
			String ruleString = "";
			ruleString = ruleBuilder.buildRule(selectedObjects, focus, boundaryIndex, category, ruleType, dependencyArity);
			if (ruleString != null) {
				System.out.println("RuleBuilder::main() rule :\n" + ruleString);
			} else {
				System.out.println("RuleBuilder::main() rule generation error");
			}
			String fileName = "cat";
			Vector categoryDependencies = new Vector();
			ruleBuilder.generateRuleCategory(gHandler, fileName, categoryDependencies);
		} catch (Exception exception) {
			System.out.println("RuleBuilder::main() " + exception.toString());
			System.exit(-1);
		}
	}
}

