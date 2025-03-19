/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package JTextEntailment;

import java.io.BufferedWriter;
import java.io.FileOutputStream;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.util.Vector;
import java.util.Collections;
//import java.util.List;
import java.util.Hashtable;
import jcoref.CorefResolver;
import jcoref.Referent;
import jcoref.ReferentList;
import jcoref.SentenceParse;
import com.xerox.jxip.*;

/**
 *
 * @author sam & gj
 */
public class AddaptedOutput {
    private CorefResolver corefResolver;
    private String pairId = "-1";
    private String grmFile;

    public AddaptedOutput(String grmFile) {
    	this.grmFile = grmFile;
        corefResolver = new CorefResolver(grmFile);
        corefResolver.setTrace(true);
    }

    public String getDependenciesAsXmlOutput(Pair pair, boolean corefFlag) {
        String output = "";
        String t = "";
        if (pair.getText().charAt(pair.getText().length()-1) == '.'){
            t = pair.getText().substring(0, pair.getText().length()-1)+"\n.\n";
        }else{
        	t = pair.getText()+"\n.\n";
        }
    	ReferentList referents = corefResolver.getReferentListFromString(t+pair.getHypothese(), true);
        Vector<SentenceParse> sentparses = corefResolver.getSentenceParses();
        
        if (corefFlag){
        	output = xmlTreeConstruction(sentparses, referents, pair);
        }else{
        	output = xmlTreeConstruction(sentparses, null, pair);
        }
        return output;
    }
    
    public void simpleCorefForTest(String pathname){
        ReferentList referents = corefResolver.getReferentListFromFile(pathname, true, false);
        try{
        	String[] splitedPathName = this.grmFile.split("/");
        	//Long[] listIdSentence=new Long[referents.size()];
        	Vector<Long> listIdSentence=new Vector<Long>();
        	
	        OutputStream ops = new FileOutputStream("/home/gjacquet/tempo/test_"+ splitedPathName[splitedPathName.length -1] +".txt");
	        OutputStreamWriter opsr = new OutputStreamWriter(ops);
	        BufferedWriter bw = new BufferedWriter(opsr);
	        for (Referent r : referents){
	        	if (r.hasAntecedents()){
	        		listIdSentence.addElement(r.getSentenceId());
	        	}
	        }
	        Collections.sort(listIdSentence);
	        for (int i = 0; i < listIdSentence.size(); i++){
	        	bw.write(String.valueOf(listIdSentence.get(i))+"\n");
	        }
	        bw.close();
        }
        catch (Exception e){
        	System.out.println("error in file" + pathname +"\n" + e.toString());
        }
    	
    }
    

    private String xmlTreeConstruction(Vector<SentenceParse> sentparses, ReferentList referents, Pair pair) {
        Vector<String> listEdge = new Vector<String>();
        Vector<XipNode> listNode = new Vector<XipNode>();
        Vector<XipNode> listArg2 = new Vector<XipNode>();
        Hashtable<XipNode,String> listCoreferent = new Hashtable<XipNode,String>();
    	Hashtable<XipNode,Vector<XipNode>> listNodePair = new Hashtable<XipNode,Vector<XipNode>>();
        String sentenceOutput = "";
        XipNode node1;
        XipNode node2;

        //list of referents with a coreferent relation
        if (referents != null){
	        for(Referent r:referents) {
	            if (r.hasAntecedents() && !r.getLemma().equals("it") && !r.getLemma().equals("they")){
	            	listCoreferent.put(r.getNode(), r.getAntecedent().getNode().getId());
	            	listCoreferent.put(r.getAntecedent().getNode(), r.getAntecedent().getNode().getId());
	            }
	        }
        }

        
        //sentence by sentence treatment
        for(SentenceParse sentparse:sentparses) {
        	listNode.removeAllElements();
        	listEdge.removeAllElements();
        	listNodePair.clear();
        	Vector<XipDependency> deps = sentparse.getDependencies();

        	//filtering redundant dependencies
        	deps = deleteSomeDependencies(deps);
        	deps = deleteDependenciesWithSameArguments(deps, "SUPPORT", "OBJ-N", false);
        	deps = deleteDependenciesWithSameArguments(deps, "POSSES", "MOD", true);
        	deps = deleteDependenciesWithSameArguments(deps, "QUALIF", "MOD", false);
        	deps = deleteDependenciesWithSameArguments(deps, "PREP", "OBJ-N", false);
        	deps = deleteDependenciesWithSameArguments(deps, "OBJ-N", "MOD", false);
        	deps = deleteDependenciesWithSameArguments(deps, "SUBJ-N", "MOD", true);
        	deps = deleteDependenciesWithSameArguments(deps, "QUANTD", "MOD", false);
        	deps = deleteDependenciesWithSameArguments(deps, "MOD", "NMODGEN", false);
        	deps = deleteDependenciesWithSameArguments(deps, "QUALIF", "ATTRIB", false);
        	deps = deleteDependenciesWithSameArguments(deps, "ATTRIB", "SUBJATTR", false);
        	deps = deleteDependenciesWithSameArguments(deps, "ATTRIB", "APPOS", false);
        	deps = deleteDependenciesWithSameArguments(deps, "SUBJ-N", "EMBED", false);
        	deps = deleteDependenciesWithSameArguments(deps, "EMBED", "MOD", true);
        	deps = deleteDependenciesWithSameArguments(deps, "MOD", "MOD", true);
        	deps = deleteDependenciesWithSameArguments(deps, "NUCL", "NUCL", true);
        	deps = deleteDependenciesWithSameArguments(deps, "MOD", "LOC", true);
        	deps = deleteDependenciesWithSameArguments(deps, "OBJ-N", "MOD", true);
        	deps = deleteDependenciesWithSameArguments(deps, "TEMP", "TEMP", true);
        	deps = deleteDependenciesWithSameArguments(deps, "EXPERIENCER","MOD", true);
        	deps = deleteDependenciesWithSameArguments(deps, "PREP","MOD", true);
        	
        	deps = deleteDependenciesWithSameArguments(deps, "SUBJ-N", "EMBED", true);
        	deps = deleteDependenciesWithSameArguments(deps, "ATTRIB", "MOD", false);
        	deps = deleteDependenciesWithSameArguments(deps, "PREP", "MOD", false);
        	deps = deleteDependenciesWithSameArguments(deps, "OBJ-N", "QUALIF", true);
        	deps = deleteDependenciesWithSameArguments(deps, "QUANTD", "NCOORD", true);
        	deps = deleteDependenciesWithSameArguments(deps, "TEMP", "PREP", true);
        	deps = deleteDependenciesWithSameArguments(deps, "TEMP", "MOD", true);
        	deps = deleteDependenciesWithSameArguments(deps, "MOD", "NCOORD", true);
        	deps = deleteDependenciesWithSameArguments(deps, "ATTRIB", "ATTRIB", false);
        	deps = deleteDependenciesWithSameArguments(deps, "LOCATION_OF", "MOD", true);
        	deps = deleteDependenciesWithSameArguments(deps, "SUBJ-N", "NUCL", true);
        	deps = deleteDependenciesWithSameArguments(deps, "EMBED", "EMBED", true);
        	deps = deleteDependenciesWithSameArguments(deps, "CONTENT", "MOD", true);
        	deps = deleteDependenciesWithSameArguments(deps, "NUCL", "NUCL", true);
        	deps = deleteDependenciesWithSameArguments(deps, "MOD", "NUCL", true);
        	deps = deleteDependenciesWithSameArguments(deps, "POSSES", "NMODGEN", true);
        	
        	
//        	//Elena
//        	//  (EMBED(#1,#2) & OBJ-N(#2,#3) & ^NUCL(#3,#1))
//        	deps = deleteLoopDependencies(deps, "EMBED", "OBJ-N", "NUCL");
//        	deps = deleteLoopDependencies(deps, "EMBED", "OBJ-N", "PREP");
//        	deps = deleteLoopDependencies(deps, "SUBJ-N", "APPOS", "NUCL");
//        	deps = deleteLoopDependencies(deps, "NUCL", "SUBJ-N", "LIXO");
//        	deps = deleteLoopDependencies(deps, "EMBED", "SUBJ-N", "NUCL");
//        	
//        	//if  (SUBJ-N(#1,#2) & ATTRIB(#2,#3) & ATTRIB(#3,#4) ^NUCL(#4,#1))
//    	    //    ~.
//        	deps = deleteLoopDependencies4funct(deps, "SUBJ-N", "ATTRIB", "ATTRIB", "NUCL");
//        	deps = deleteLoopDependencies4funct(deps, "SUBJ-N", "ATTRIB", "QUANTD", "NUCL");
//        	deps = deleteLoopDependencies4funct(deps, "EMBED", "OBJ-N", "ATTRIB", "NUCL");
//        	
//        	//if  (SUBJ-N(#1,#2) & PREP(#2,#3) & MOD(#3,#4) OBJ-N(#4,#5) ATTRIB(#5,#6) ^NUCL(#6,#1))
//        	//    ~.
//        	deps = deleteLoopDependencies6funct(deps, "SUBJ-N", "PREP", "MOD", "OBJ-N", "ATTRIB", "NUCL");
//        	
//        	//if (^PREP(#1,#2) & PREP(#3,#2))
//        	//	   ~.
//        	deps = deletePREP(deps, "PREP", "PREP");
//
//        	
//        	//if  ((^NCOORD(#3,#2) | ^NCOORD(#2,#3)) & $1(#1,#2) & $2(#1,#3) )
//            //   ~.
////        	deps = deleteNCoord(deps, "NCOORD");
////        	deps = deleteNCoord(deps, "AJCOORD");
        	
        	listEdge.addElement(controlDependenciesWithSameArguments(deps));

        	//construct the initial list of the leave nodes of the sentence
        	Vector<XipLeaf> listLeaves = sentparse.getLeaves();
        	Vector<XipNode> listMotherOfLeaves = new Vector<XipNode>();
        	for (XipLeaf l : listLeaves){
        		listMotherOfLeaves.addElement(l.getMother());
//        		System.out.println(l.getMother().getLemma());
        	}
        	
            for(XipDependency dep : deps) {
                String output = null;
                String func = dep.getName();
	            if (func.equals("HEAD")){
	            	continue;
	            }

                
                if (dep.getParameters().size() == 2){
                	//continue if arg1 equal arg2
                	if (dep.getArg(1).equals(dep.getArg(2))){
                		continue;
                	}
                	
                	//continue if arg1 and arg2 have a common mother which is in a coreferent relation
                	if (dep.getArg(1).getMother().equals(dep.getArg(2).getMother()) &&
                		listCoreferent.containsKey(dep.getArg(1).getMother())){
                		continue;
                	}
                	
                	//replace the NP node by their head except if they are in a coreferent relation
                	if (getHead(deps, dep.getArg(1)) != null && !listCoreferent.containsKey(dep.getArg(1))){
                		node1 = getHead(deps, dep.getArg(1));
                   	}else{
                		node1 = dep.getArg(1);
                	}
                	if (getHead(deps, dep.getArg(2)) != null && !listCoreferent.containsKey(dep.getArg(2))){
                		node2 = getHead(deps, dep.getArg(2));
                	}else{
                		node2 = dep.getArg(2);
                	}
                	
                	if (node1.equals(node2)){
                		continue;
                	}
                	
                	//display the preposition for the specific PREP dependency
                    if (func.equals("PREP") && !dep.getFeatures().isEmpty()){
                        func+= "_"+dep.getFeatures().toString().substring(1, dep.getFeatures().toString().length()-3);
                    }
                    
                    if (listNodePair.containsKey(node2) && listNodePair.get(node2).contains(node1)){
                    	continue;
                    }
    				if (listNodePair.containsKey(node1)){
    					listNodePair.get(node1).addElement(node2);
    				}else{
    					listNodePair.put(node1, new Vector<XipNode>());
    					listNodePair.get(node1).addElement(node2);
    				}
                    //display format of each dependency
                    output = "          <edge to=\""+node2.getId()+ "\" name=\"" +func+ "\" from=\"" +node1.getId()+ "\"/>";
                	listEdge.add(output);

                    //remove the treated leaves from the initial list of leaves
                    listMotherOfLeaves.removeAll(node1.getLeafNodes());
                    listMotherOfLeaves.removeAll(node2.getLeafNodes());

                    //store the nodes to display in the output
                    if (!listNode.contains(node1)){
                		listNode.addElement(node1);
                	}
                	if (!listNode.contains(node2)){
                		listNode.addElement(node2);
                	}
                	
                	//store the node that don't need to be linked to the root
                	if (!listArg2.contains(node2)){
                		listArg2.addElement(node2);
                	}
                }
            }
            
            //link to the root all the nodes that are never used as an arg2 
            Vector<XipNode> nodeToLinkToRoot = (Vector<XipNode>) listNode.clone();
            nodeToLinkToRoot.removeAll(listArg2);
            String output;
            for (XipNode node : nodeToLinkToRoot){
                output = "          <edge to=\""+node.getId()+ "\" name=\"ROOT-LINK\" from=\"" + sentparse.getRoot().getId() + "\"/>";
            	listEdge.add(output);
            }

            //add the root to the list of displayed nodes
            listNode.addElement(sentparse.getRoot());

            //link the orphan nodes to the previous linked node or to the root. 
            for (XipNode node : listMotherOfLeaves){
            	if (!node.isFicticious() && (node.getCategory().equals("NOUN") ||
            			node.getCategory().equals("ADJ") ||
            			node.getCategory().equals("NADJ") ||
            			node.getCategory().equals("VERB") ||
            			node.getCategory().equals("ADV"))){
            		long i = node.getGramIdNode()-1;
            		boolean nextPreviousNode = true;
            		XipNode previousNode = sentparse.getRoot();
            		while (nextPreviousNode && i > 0){
            			if (sentparse.getNodeByGramId(i) != null){
            				previousNode = sentparse.getNodeByGramId(i);
            			}
            			if (listNode.contains(previousNode)){
            				nextPreviousNode = false;
            			}
            			i--;
            		}
//<<<<<<< AddaptedOutput.java
//<<<<<<< AddaptedOutput.java
//                    output = "          <edge to=\""+node.getId()+ "\" name=\"ARTIF-LINK\" from=\"" + previousNode.getId() + "\"/>";
//                	listEdge.add(output);
//                	listNode.addElement(node);
//=======
//                    output = "          <edge to=\""+node.getId()+ "\" name=\"ROOT-TRUC\" from=\"" + previousNode.getId() + "\"/>";
//=======
            		
            		if (listCoreferent.containsKey(node.getMother())){
            			node = node.getMother();
            		}
                    output = "          <edge to=\""+node.getId()+ "\" name=\"ARTIF-LINK\" from=\"" + previousNode.getId() + "\"/>";
//>>>>>>> 1.15
                	if (!listEdge.contains(output)){
                		listEdge.add(output);
                	}
                	if (!listNode.contains(node)){
                		listNode.addElement(node);
                	}
//>>>>>>> 1.14
            	}
            }

            //contruct the xml tree
            sentenceOutput += displayXmlSentence(sentparse.sentenceId, (long) sentparse.getStart(), listNode, listEdge, listCoreferent, pair);
        }
        return sentenceOutput;
    }
    
    private XipNode getHead(Vector<XipDependency> deps, XipNode node){
    	for (XipDependency dep : deps){
    		if (dep.getName().equals("HEAD") &&
    				dep.getArg(1).equals(node)){
    					return dep.getArg(2);
    		}
    	}
    	return null;
    }
    
    
    public String displayXmlPair(String sentenceOutput){
    	String beginOutput = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    	beginOutput +=       "<entailment-corpus>\n";
    	
    	String endOutput =   "  </pair>\n";
    	endOutput +=   		 "</entailment-corpus>\n";
    	
    	
    	return beginOutput + sentenceOutput + endOutput;
    	
    }

    private Vector<XipDependency> deleteDependenciesWithSameArguments(Vector<XipDependency> deps, String func1, String func2, boolean reversedArguments){
    	//delete dependencies with same argument.
    	//equivalent to the xip rules like :
        //if  (ATTRIB(#1,#2) & ^MOD(#1,#2))
        //    ~.
    	//
    	// the boolean reversedArguments variable is true if the xip rule is
        //if  (ATTRIB(#1,#2) & ^MOD(#2,#1))
        //    ~.
    	

    	Vector<XipDependency> newDeps = (Vector<XipDependency>) deps.clone();
    	Hashtable<String,XipDependency> listDepsToControl = new Hashtable<String,XipDependency>();
    	String key;
    	for (XipDependency dep : deps){
    		if (dep.getName().equals(func1)){
    			key = dep.getArg(1).getLeftChar() + "-" + dep.getArg(1).getRightChar()+"_"+dep.getArg(2).getLeftChar() + "-" + dep.getArg(2).getRightChar();
    			listDepsToControl.put(key, dep);
    		}
    	}
    	for (XipDependency dep : deps){
    		if (dep.getName().equals(func2)){
    	    	if (reversedArguments) {
    	    		key = dep.getArg(2).getLeftChar() + "-" + dep.getArg(2).getRightChar()+"_"+dep.getArg(1).getLeftChar() + "-" + dep.getArg(1).getRightChar();
        			if (listDepsToControl.containsKey(key)){
        				newDeps.removeElement(dep);
    	    			listDepsToControl.remove(dep.getArg(1).getLeftChar() + "-" + dep.getArg(1).getRightChar()+"_"+dep.getArg(2).getLeftChar() + "-" + dep.getArg(2).getRightChar());
        			}
    	    	}else{
    	    		key = dep.getArg(1).getLeftChar() + "-" + dep.getArg(1).getRightChar()+"_"+dep.getArg(2).getLeftChar() + "-" + dep.getArg(2).getRightChar();
        			if (listDepsToControl.containsKey(key)){
        				newDeps.removeElement(dep);
    	    			listDepsToControl.remove(key);
        			}
    	    	}
    		}
    	}
    	return newDeps;
    }

    private Vector<XipDependency> deleteDependenciesWithSameArguments_old(Vector<XipDependency> deps, String func1, String func2, boolean reversedArguments){
    	//delete dependencies with same argument.
    	//equivalent to the xip rules like :
        //if  (ATTRIB(#1,#2) & ^MOD(#1,#2))
        //    ~.
    	//
    	// the boolean reversedArguments variable is true if the xip rule is
        //if  (ATTRIB(#1,#2) & ^MOD(#2,#1))
        //    ~.
    	

    	Vector<XipDependency> newDeps = (Vector<XipDependency>) deps.clone();
    	Hashtable<String,XipDependency> listDepsToControl = new Hashtable<String,XipDependency>();
    	String key;
    	for (XipDependency dep : deps){
    		if (dep.getName().equals(func1)){
    			key = dep.getArg(1).getId()+"_"+dep.getArg(2).getId();
    			listDepsToControl.put(key, dep);
    		}
    	}
    	for (XipDependency dep : deps){
    		if (dep.getName().equals(func2)){
    	    	if (reversedArguments) {
    	    		key = dep.getArg(2).getId()+"_"+dep.getArg(1).getId();
        			if (listDepsToControl.containsKey(key)){
        				newDeps.removeElement(dep);
    	    			listDepsToControl.remove(dep.getArg(1).getId()+"_"+dep.getArg(2).getId());
        			}
    	    	}else{
    	    		key = dep.getArg(1).getId()+"_"+dep.getArg(2).getId();
        			if (listDepsToControl.containsKey(key)){
        				newDeps.removeElement(dep);
    	    			listDepsToControl.remove(key);
        			}
    	    	}
    		}
    	}
    	return newDeps;
    }
    
    private Vector<XipDependency> deleteSomeDependencies(Vector<XipDependency> deps){
	    Vector<XipDependency> newDeps = (Vector<XipDependency>) deps.clone();
	    for(XipDependency dep : deps) {
	        String func = dep.getName();
	        //list of hidden dependencies
	        if (func.equals("MAIN") ||
	            func.equals("SIMPLE") ||
	            func.equals("PREPD") ||
	            func.equals("SUBJ") ||
	            func.equals("OBJ") ||
	            func.equals("SREL") ||
	            func.equals("SAY")||
	            func.startsWith("S0") ||
	            func.startsWith("S1") ||
	            func.startsWith("S2") ||
	            func.startsWith("S3") ||
	            func.startsWith("COREF")||
	            func.equals("VDOMAIN")||
	            func.equals("SREL")||
	            func.equals("COORD")||
	            func.equals("AGEO")||
	            func.equals("MOD_OF")||
	            func.equals("FRM")||
	            func.equals("ASL")||
	            func.equals("INTERNALS_TITLE")||
	            func.equals("PARTICLE")||
	            func.startsWith("INTERNALS_LASTNAME")||
	            func.startsWith("INTERNALS_FIRSTNAME")||
	            func.startsWith("NA1") ||
	            func.startsWith("VA1")){
	        	newDeps.removeElement(dep);
	        }
	    }
	    return newDeps;
    }

    private String controlDependenciesWithSameArguments(Vector<XipDependency> deps){
    	//avoid simple loops

    	Vector<XipDependency> newDeps = (Vector<XipDependency>) deps.clone();
    	Hashtable<String,XipDependency> listDepsToControl = new Hashtable<String,XipDependency>();
    	String key;
    	for (XipDependency dep : deps){
    		if(dep.getParameters().size() == 2 && !dep.getName().equals("HEAD")){
    			key = dep.getArg(1).getId()+"_"+dep.getArg(2).getId();
    			listDepsToControl.put(key, dep);
    		}
    	}
    	for (XipDependency dep : deps){
    		if(dep.getParameters().size() == 2 && !dep.getName().equals("HEAD")){
	    	    key = dep.getArg(2).getId()+"_"+dep.getArg(1).getId();
	    		if (listDepsToControl.containsKey(key)){
	    			XipDependency badDep = listDepsToControl.get(key);
	    			newDeps.removeElement(dep);
	    			return "**********<edge to=\""+badDep.getArg(1).getId()+ "\" name=\"" + badDep.getName() + "\" from=\"" + badDep.getArg(2).getId() + "\"/>\n"+
	    			       "**********<edge to=\""+dep.getArg(1).getId()+ "\" name=\"" + dep.getName() + "\" from=\"" + dep.getArg(2).getId() + "\"/>";
	    		}
    		}
    	}
    	return "";
    }
    
    //Elena
    //if  (EMBED(#1,#2) & OBJ-N(#2,#3) & ^NUCL(#3,#1))
    //    ~.
    
/*    private Vector<XipDependency> deleteLoopDependencies(Vector<XipDependency> deps, String func1, String func2, String func3){
    	Vector<XipDependency> newDeps = (Vector<XipDependency>) deps.clone();
    	Hashtable<String,XipDependency> dep1arg1 = new Hashtable<String,XipDependency>();
    	Hashtable<String,XipDependency> dep1arg2 = new Hashtable<String,XipDependency>();
    	Hashtable<String,XipDependency> dep2arg2= new Hashtable<String,XipDependency>();
    	String key;
    	for (XipDependency dep : deps){
    		if (dep.getName().equals(func1)){
    			key = dep.getArg(1).getId();
    			dep1arg1.put(key, dep);
    			key = dep.getArg(2).getId();
    			dep1arg2.put(key, dep);
    		}
    	}
    	
    	for (XipDependency dep : deps){
    		if (dep.getName().equals(func2)){
    			key = dep.getArg(1).getId();
    			if (dep1arg2.containsKey(key)){
    				dep2arg2.put(dep.getArg(2).getId(), dep);
    			}
    		}
    	}
    	
    	for (XipDependency dep : deps){
    		if (dep.getName().equals(func3)){
    			if 	(dep1arg1.containsKey(dep.getArg(2).getId()) &&
    				 dep2arg2.containsKey(dep.getArg(1).getId())){
    				newDeps.removeElement(dep);
    			}
    		}
    	}
    	return newDeps;
    }
*/    
    
   //if  (SUBJ-N(#1,#2) & ATTRIB(#2,#3) & ATTRIB(#3,#4) ^NUCL(#4,#1))
    //    ~. Elena (to be checked)
   
/*    private Vector<XipDependency> deleteLoopDependencies4funct(Vector<XipDependency> deps, String func1, String func2, String func3, String func4){
    	Vector<XipDependency> newDeps = (Vector<XipDependency>) deps.clone();
    	Hashtable<String,XipDependency> dep1arg1 = new Hashtable<String,XipDependency>();
    	Hashtable<String,XipDependency> dep1arg2 = new Hashtable<String,XipDependency>();
    	Hashtable<String,XipDependency> dep2arg2= new Hashtable<String,XipDependency>();
    	Hashtable<String,XipDependency> dep3arg2= new Hashtable<String,XipDependency>();
    	String key;
    	for (XipDependency dep : deps){
    		if (dep.getName().equals(func1)){
    			key = dep.getArg(1).getId();
    			dep1arg1.put(key, dep);
    			key = dep.getArg(2).getId();
    			dep1arg2.put(key, dep);
    		}
    	}
    	
    	for (XipDependency dep : deps){
    		if (dep.getName().equals(func2)){
    			key = dep.getArg(1).getId();
    			if (dep1arg2.containsKey(key)){
    				dep2arg2.put(dep.getArg(2).getId(), dep);
    			}
    		}
    	}
    	
    	for (XipDependency dep : deps){
    		if (dep.getName().equals(func3)){
    			key = dep.getArg(1).getId();
    			if (dep2arg2.containsKey(key)){
    				dep3arg2.put(dep.getArg(2).getId(), dep);
    			}
    		}
    	}
    	
    	for (XipDependency dep : deps){
    		if (dep.getName().equals(func4)){
    			if 	(dep1arg1.containsKey(dep.getArg(2).getId()) &&
    				 dep3arg2.containsKey(dep.getArg(1).getId())){
    				newDeps.removeElement(dep);
    			}
    		}
    	}
    	return newDeps;
    }
*/    
    
    //if  (SUBJ-N(#1,#2) & PREP(#2,#3) & MOD(#3,#4) OBJ-N(#4,#5) ATTRIB(#5,#6) ^NUCL(#6,#1))
     //   ~. (Elena to be checked)
    
/*    private Vector<XipDependency> deleteLoopDependencies6funct(Vector<XipDependency> deps, String func1, String func2, String func3, String func4, String func5, String func6){
    	Vector<XipDependency> newDeps = (Vector<XipDependency>) deps.clone();
    	Hashtable<String,XipDependency> dep1arg1 = new Hashtable<String,XipDependency>();
    	Hashtable<String,XipDependency> dep1arg2 = new Hashtable<String,XipDependency>();
    	Hashtable<String,XipDependency> dep2arg2= new Hashtable<String,XipDependency>();
    	Hashtable<String,XipDependency> dep3arg2= new Hashtable<String,XipDependency>();
    	Hashtable<String,XipDependency> dep4arg2= new Hashtable<String,XipDependency>();
    	Hashtable<String,XipDependency> dep5arg2= new Hashtable<String,XipDependency>();
    	String key;
    	for (XipDependency dep : deps){
    		if (dep.getName().equals(func1)){
    			key = dep.getArg(1).getId();
    			dep1arg1.put(key, dep);
    			key = dep.getArg(2).getId();
    			dep1arg2.put(key, dep);
    		}
    	}
    	
    	for (XipDependency dep : deps){
    		if (dep.getName().equals(func2)){
    			key = dep.getArg(1).getId();
    			if (dep1arg2.containsKey(key)){
    				dep2arg2.put(dep.getArg(2).getId(), dep);
    			}
    		}
    	}
    	
    	for (XipDependency dep : deps){
    		if (dep.getName().equals(func3)){
    			key = dep.getArg(1).getId();
    			if (dep2arg2.containsKey(key)){
    				dep3arg2.put(dep.getArg(2).getId(), dep);
    			}
    		}
    	}
    	
    	for (XipDependency dep : deps){
    		if (dep.getName().equals(func4)){
    			key = dep.getArg(1).getId();
    			if (dep3arg2.containsKey(key)){
    				dep4arg2.put(dep.getArg(2).getId(), dep);
    			}
    		}
    	}
    	
    	for (XipDependency dep : deps){
    		if (dep.getName().equals(func5)){
    			key = dep.getArg(1).getId();
    			if (dep4arg2.containsKey(key)){
    				dep5arg2.put(dep.getArg(2).getId(), dep);
    			}
    		}
    	}
    	for (XipDependency dep : deps){
    		if (dep.getName().equals(func4)){
    			if 	(dep1arg1.containsKey(dep.getArg(2).getId()) &&
    				 dep5arg2.containsKey(dep.getArg(1).getId())){
    				newDeps.removeElement(dep);
    			}
    		}
    	}
    	return newDeps;
    }
*/    
    //if (PREP(#3,#2) & ^PREP(#1,#2))
    //	   ~. (Elena, to be checked)
    
/*    private Vector<XipDependency> deletePREP(Vector<XipDependency> deps, String func1, String func2){
    	Vector<XipDependency> newDeps = (Vector<XipDependency>) deps.clone();
    	Hashtable<String,XipDependency> dep1arg2 = new Hashtable<String,XipDependency>();
    	Hashtable<String,XipDependency> dep2arg2= new Hashtable<String,XipDependency>();
    	String key;
    	for (XipDependency dep : deps){
    		if (dep.getName().equals(func1)){
    			key = dep.getArg(2).getId();
    			dep1arg2.put(key, dep);
    		}
    	}
    	
    	
    	for (XipDependency dep : deps){
    		if (dep.getName().equals(func2)){
    			if 	(dep1arg2.containsKey(dep.getArg(2).getId()) &&
    				 dep2arg2.containsKey(dep.getArg(1).getId())){
    				newDeps.removeElement(dep);
    			}
    		}
    	}
    	return newDeps;
    }
*/    
     
    public String displayXmlSentence(Long sentenceID, Long offsetBeginSentence, Vector<XipNode> listNode, Vector<String> listEdge, Hashtable<XipNode,String> listCoreferent, Pair pair){
    	String output = "";
    	String outputForEachNode;
    	XipNode top = null;
    	for(XipNode n : listNode){
    		if (n.getCategory().equals("TOP")){
    			top = n;
    			listNode.remove(n);
        		break;
    		}
    	}
    			
    			//System.out.println(n.getFeature("#id_in_int"));
    	if (!pair.getId().equals(pairId)){
    		if (!pairId.equals("-1")){
    			output += "  </pair>\n";
    		}
    		output += "  <pair id=\""+ pair.getId() +"\" entailment=\""+ pair.getEntailment() +"\" task=\""+ pair.getTask() +"\">\n";
    		pairId = pair.getId();
    	}

		if (pair.getHBeginOffset() >offsetBeginSentence){
			output += "    <tAnnotation id=\""+ sentenceID +"\">\n";
		}else{
			output += "    <hAnnotation id=\""+ sentenceID +"\">\n";
		}
		String sentence = top.getSurface();
		output += "      <text>" +sentence.replaceAll("&", "&amp;")+ "</text>\n";
		output += "        <tree root=\"" +top.getId()+ "\">\n";
		output += "          <node id=\"" +top.getId()+ "\">\n";
		output += "            <word id=\"" +top.getId()+ "\">\n";
		output += "              <attribute name=\"lemma\">ROOT</attribute>\n";
		output += "              <attribute name=\"token\">ROOT</attribute>\n";
		output += "              <attribute name=\"pos\">" +top.getCategory()+ "</attribute>\n";
		output += "            </word>\n";
		output += "          </node>\n";
    	
    	for(XipNode n : listNode){
    		outputForEachNode =  "          <node id=\"" +n.getId()+ "\">\n";
    		outputForEachNode += "            <word id=\"" +n.getId()+ "\">\n";
    		if (n.getLemma().contains("&")){
    			outputForEachNode += "              <attribute name=\"lemma\">" +n.getLemma().replaceAll("&", "&amp;")+ "</attribute>\n";
    			outputForEachNode += "              <attribute name=\"token\">" +n.getSurface().replaceAll("&", "&amp;")+ "</attribute>\n";
    		}else{
    			outputForEachNode += "              <attribute name=\"lemma\">" +n.getLemma()+ "</attribute>\n";
    			outputForEachNode += "              <attribute name=\"token\">" +n.getSurface()+ "</attribute>\n";    			
    		}
    		outputForEachNode += "              <attribute name=\"pos\">" +n.getCategory()+ "</attribute>\n";
    		if (listCoreferent.containsKey(n)){
        		outputForEachNode += "              <attribute name=\"coref\">" +listCoreferent.get(n)+ "</attribute>\n";
    		}
    		outputForEachNode += "            </word>\n";
    		outputForEachNode += "          </node>\n";
    		output += outputForEachNode;
    	}
    	
    	String endOfTree = "";
    	endOfTree +=  "        </tree>\n";
		if (pair.getHBeginOffset() >offsetBeginSentence){
			endOfTree +=  "    </tAnnotation>\n";
		}else{
			endOfTree +=  "    </hAnnotation>\n";
		}
//    	endOfTree +=  "  </pair>\n";

    	//System.out.print(output);
    	for (String vs : listEdge){
    		output += vs + "\n";
    	}
    	output += endOfTree;
    	
    	return output;
    	
    	
    }
    
    
    

}

