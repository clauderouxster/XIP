
import com.xerox.xrce.lingdexip.*;

import com.xerox.xrce.lingdexip.ui.run.graph.*;

import com.xerox.jxip.*;

import com.xerox.xrce.lingdexip.util.*;

import java.util.*;

import java.util.logging.*;

import java.io.*;

import java.awt.*;


/**

 *

 * @author csaillet

 */





public class Test {

    

    private Xip xip; 

    private XipUnit xipUnit ;

    private SelectedObject sObject;

    private XipNode root;

    private RuleBuilder ruleBuilder;

    private String fileName;

    private String encoding;

    private String adrFile;

    private String category;

    private String ruleString;   

    private String cat;

    private String lang;

    private String result;

    private Vector selectedObjs;

    private Vector selectedO;

    private Vector focus;

    private Vector leav;

    private Vector dep;

    private Vector features;

    private Vector categoryDependencies;



    private java.lang.Object object;

    

    /** Creates a new instance of Rules */

    public Test() {

    }

    

      public static boolean containsFeature(XipFeature feat) {

        Vector features = new Vector();

        Iterator ite = features.iterator();

        boolean stop = false;

        String toCompare = feat.getName() + ":" + feat.getValue();

        while ( ite.hasNext() && !stop) {

            XipFeature curFeat = (XipFeature)ite.next();

            String value = curFeat.getName() + ":" + curFeat.getValue();

            if ( value.equals(toCompare) ) {

                stop = true;

            }

        }

       return stop;

    }  

           

        /** XipNode xipNod est un noeud

         * verifie si ce noeud a un fils 

         * et si le fils est une feuille

         * @return vrai si le fils est une feuille faux sinon

         */

    public static boolean herDaughterIsLeaf(XipNode xipNod) {

            boolean res = false;

            Vector vec = xipNod.getDaughters();

            Iterator it= vec.iterator();

            while (it.hasNext() && !res){

             XipNode xNod = (XipNode)it.next();

              if(xNod.isLeaf()) 

                  res = true;

          }

        return res;  

    }



    /** recupere les feuilles du xipNode et les mets dans le vector vec */

    public static void readXipNode(XipNode xipNode, Vector vec){

        boolean bool;

        Vector features = xipNode.getFeatures();  

        int size = features.size(); 

        bool = herDaughterIsLeaf(xipNode); 

        if (bool) {   //si xipNode a un fils qui est une feuille

              XipLeaf leaf = (XipLeaf)xipNode.getDaughters().get(0);

              XipFeature surfaceFeature = new XipFeature("surface",leaf.getSurface(),size);

              XipFeature lemmaFeature = new XipFeature("lemme",leaf.getLemma(),size+1);

              if ( !containsFeature(surfaceFeature)) {

                 features.add(surfaceFeature);

              }

               if ( !containsFeature(lemmaFeature)) {

                  features.add(lemmaFeature);

              }

        }

        if(xipNode.isLeaf()){

            vec.add(xipNode);

        }

        else{    

        vec.add(xipNode);

        Vector vOfChild = xipNode.getDaughters();

        for (int i=0; i<vOfChild.size();i++){

            XipNode node = (XipNode)vOfChild.get(i);

            readXipNode(node, vec);

            }

        }

    }

        

    /** ecrit (dans la console) les XipNode compris dans le vector vec */  

    public static void printXipNode(Vector vec){

          Iterator it= vec.iterator();

          while (it.hasNext()){

              XipNode xNode = (XipNode)it.next();

              System.out.println(xNode.getIndex() + "#"+ xNode.toString(true,"\t",false));

          }

      }  

      

     /** Vector v est un vector de XipNode  

      * int[] indexs est un tableau d`index

      * return un vector de SelectedObject  */

      public static Vector constructSelectedObject(Vector v, int[] indexs){

          Vector res = new Vector();

          for(int i=0;i<indexs.length;i++){

              XipNode node = (XipNode)v.get(indexs[i]);

              SelectedObject s = new SelectedObject(node);

              Vector w = new Vector();

              w.add("DET");

              w.add("NOUN");

              w.add("surface");

              w.add("lemme");

              Iterator it = constructFeatureSet(node,w).iterator();

              while (it.hasNext()){

                  XipFeature feature = (XipFeature) it.next();

                  s.addSelectedFeature(feature);

              }

              res.add(s);   

          }

          return res;

      }

 

        public static Vector constructSelectedObjectFromDependency(Vector v, int[] indexes){

          Vector res = new Vector();

          String strg;

        for(int i=0;i<indexes.length;i++){

             XipDependency xipdependency=(XipDependency)v.get(indexes[i]);

             SelectedObject catDep = new SelectedObject(xipdependency);

             

             Vector x = new Vector();

             x.add("PRE");

             x.add("POST");

            Iterator ite = constructFeatureSet(xipdependency,x).iterator();

             while (ite.hasNext()){

                  XipFeature xfeat = (XipFeature) ite.next();

                  catDep.addSelectedFeature(xfeat);

             }

             System.out.println(xipdependency);

             res.add(catDep);   

          }

          return res;

      }

      

      /**recupere la liste des traits*/

      public static Vector constructFeatureSet(Object xnode, Vector nameFeatures){

          Vector res = new Vector();

          XipNode node = null;

          XipDependency dependency = null;

          Vector features=null;

          if (xnode instanceof XipDependency ){

              dependency=(XipDependency)xnode;

              features=dependency.getFeatures();

          }

          else if(xnode instanceof XipNode){

              node = (XipNode) xnode;

              features=node.getFeatures();

          }

          if (features!=null){

           Iterator it=features.iterator();

              while(it.hasNext()){



              XipFeature feature = (XipFeature)it.next();

              if(nameFeatures.contains(feature.getName()) ){

                  res.add(feature);

              }

          }

          }

          return res;

      } 

          



      /** concatenation de deux Vector vec1 et vec2 dans vec1 */

      public static Vector concVector(Vector vec1, Vector vec2){

          

          

         for (int i=1;i<vec2.size();i++){

              vec1.addElement(vec2.get(i));

         }

          return vec1;

      }



     public static Vector constructSelectedObjectFromSequence(Vector v, int[]indexbody){

        Vector vec = new Vector();

        for (int i=0;i<indexbody.length;i++){

            XipNode node = (XipNode) v.get(indexbody[i]);

            SelectedObject s = new SelectedObject(node);

            //System.out.println(node);

             vec.add(s);   

        }

        return vec;

    }

      

      /** creation des regles de dependences */

      private static void dependency(Vector vect, XipUnit unit){

          try{ 

          Vector focus = new Vector();

           String ruleString = new String();

           RuleBuilder ruleBuilder = new RuleBuilder();

           

           // recherche et affichage (dans la console) des dependences possibles de la phrase

       // selectionne et ajoute les dependences selectionnees

       String depToString = new String();

       // vectRoot contient la liste des  dependances d`un noeud

       Vector vectRoot = unit.getDependencies();

       Iterator it= vectRoot.iterator();

       while (it.hasNext()){

                  XipDependency xipdependency = (XipDependency)it.next();

                  System.out.println(" xipdependency: "+ xipdependency.toString(true));

              }

       

       //selection des SelectedObject

       int[] index = {2,5};

       int[] index2= {0,1,2,3};

       Vector selection = constructSelectedObject(vect,index);

       Vector selection2 = constructSelectedObjectFromDependency(vectRoot,index2);

       Vector selectionFinal = concVector(selection,selection2);

     

       //focus = liste d index de noeuds ou de dependances...Vector of long  

       Long ind0 = new Long(3);

       Long ind1 = new Long(6);



       focus.add(0,ind0); 

       focus.add(1,ind1); 



       ruleString= ruleBuilder.buildRule(selection, focus, -1, "person", Const.DEPENDENCYRULE, 2);

       System.out.println(ruleString);

          }

          catch(Exception e){

              System.err.println(e);

          }

      }

      

      private static void sequence(Vector vect, XipUnit unit){

         try{

         long left;

         long right;

       RuleBuilder ruleBuilder = new RuleBuilder();

       String ruleString = new String();

       String ruleString2 = new String();

       String ruleString3 = new String();

       String ruleResult = new String();

       

          //selection du body

       int[] indexbody={7,8};

       Vector vecBod = constructSelectedObjectFromSequence(vect,indexbody);

       ruleString= ruleBuilder.buildRule(vecBod, null, -1, "", Const.SEQUENCERULE, 1);

       System.out.println("ruleString body = " + ruleString);

       

       //recuperation des indices des bornes gauches et droites du body

       left = -1 ;

       right = -1;

              

//       //selection du contexte gauche

//       Vector vecLeft = new Vector(); //Vector de SelectedObject

//       int[] indexleft ={2,3};

//       vecLeft = constructSelectedObjectFromSequence (vect,indexleft);

//      

//       ruleString2= ruleBuilder.buildRule(vecLeft, null, left, "", Const.LEFTCONTEXT, -1);

//       System.out.println("ruleString left = " +ruleString2);

       

       //selection du contexte droit

       Vector vecRight = new Vector();

       int[] indexright ={13};

       vecRight = constructSelectedObjectFromSequence (vect,indexright);

//       Vector v = new Vector(vecBod);

//       v.addAll(vecRight);

       

       ruleString3= ruleBuilder.buildRule(vecRight, null, right, "", Const.RIGHTCONTEXT, -1); //RIGHTCONTEXT

       System.out.println("ruleString right = " +ruleString3);

      

       //concatenation et affichage de la regle

       ruleResult = ruleString2 + ruleString + ruleString3;

       System.out.println("ruleResult = "+ruleResult);

       

       

       // affichage de la regle la plus longue possible avec l`option  "longest match"

             

       

         } 

         catch(Exception e){

             System.err.println(e);

             e.printStackTrace();

         }

      }

      

      

    public static void main (String args[]){

        

       try{ 

       String cat = new String();

       String lang = new String();

       String adrFile = new String ();

       String category = new String();

       

       String result = new String();

       Vector selectedObjs = new Vector();

       Vector selectedO = new Vector();

      

       Vector leav = new Vector();

       Vector dep = new Vector();

       Object object = new Object();

       

       /**choix du type de regle:

        1-> regles de dependances

        2-> regles de sequences*/

       int choice = 2;

       int res;

       int numgHandler;

       int ruleType;

       int dependencyArity;

       long boundaryIndex;

       long lef; 

       long righ;       

       XipResult xresult;

       SelectedObject sObject;

       File toto = new File("toto.txt");

       toto.createNewFile();

       FileTools.storeString("A man buys a cat.",toto.getAbsolutePath(),FileTools.ENCODING_DEFAULT);

        

       Xip xip = new Xip();

       

       SelectedObject selectedObject = new SelectedObject(object);

        

       adrFile = "../../../GRAMMARS/ENGLISH/GRMFILES/GRAM_GEN/gram_gen_entit.grm";

       numgHandler = xip.XipGrmFile(adrFile, 0, true);

       res = xip.buildXipResultFromFile(numgHandler, toto.getAbsolutePath(), FileTools.ENCODING_DEFAULT); 

       if (res==1){

           System.out.println("le XipResult n`a pas ete construit");

         //  break;

       }   

       xresult = xip.getXipResult();

       Vector vOfXipUnit = xresult.getSentences();

       XipUnit unit = (XipUnit)vOfXipUnit.get(0);

       XipNode root =  unit.getRoot();

       Vector vect = new Vector();

       readXipNode(root,vect);

       printXipNode(vect);

       

       if(choice == 1)

        dependency(vect,unit);

       else if(choice ==2)

        sequence(vect,unit);

       else System.out.println("ERROR about type of rules");

               

       }

       catch (Exception e) {

                   System.err.println(e);

                   e.printStackTrace();

                }

        }

    

}//fin de la classe


