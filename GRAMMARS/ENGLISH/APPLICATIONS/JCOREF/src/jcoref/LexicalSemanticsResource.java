/*
 * LexicalSemanticsResource.java
 *
 * Created on August 25, 2006, 5:00 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package jcoref;

import java.io.FileInputStream;
import java.util.Vector;
import net.didion.jwnl.JWNL;
import net.didion.jwnl.JWNLException;
import net.didion.jwnl.data.IndexWord;
import net.didion.jwnl.data.POS;
import net.didion.jwnl.data.PointerUtils;
import net.didion.jwnl.data.Synset;
import net.didion.jwnl.data.list.PointerTargetNode;
import net.didion.jwnl.data.list.PointerTargetNodeList;
import net.didion.jwnl.dictionary.Dictionary;

/**
 *
 * @author ait
 */
public class LexicalSemanticsResource {
    
    
    
    /** Creates a new instance of LexicalSemanticsResource */
    public LexicalSemanticsResource() {
        String propsFile = "jwnl_file_props.xml";
        try {
            // initialize JWNL (this must be done before JWNL can be used)
            JWNL.initialize(new FileInputStream(propsFile));
        } catch (Exception ex) {
            ex.printStackTrace();
            System.exit(-1);
        }
    }
    
    /** Returns the list of synonyms of this word */
    public Vector<String> getSynonymsOf(String w, POS cat, int ambignb) {
        IndexWord indword;
        try {
            indword = Dictionary.getInstance().lookupIndexWord(cat, w);
            if (indword==null) {
                return null;
            } else {
                Vector<String> hypers=new Vector<String>(9);
                for(int i=1; i<=ambignb && i<=indword.getSenses().length; i++) {
                    for(int j=0; j < indword.getSense(i).getWords().length; j++) {
                        hypers.add(indword.getSense(i).getWords()[j].getLemma());
                    }
                    hypers.add("<END_SENSE>");
                }
                return hypers;
            }
        } catch (JWNLException ex) {
            ex.printStackTrace();
            return null;
        }        
    }
    
    /** Returns the list of semantic types of this word */
    public Vector<String> getSemTypesOf(String w, POS cat, int ambignb) {
        IndexWord indword;
        try {
            indword = Dictionary.getInstance().lookupIndexWord(cat, w);
            if (indword==null) {
                return null;
            } else {
                Vector<String> hypers=new Vector<String>(9);
                for(int i=1; i<=ambignb && i<=indword.getSenses().length; i++) {
                    getHypernyms(indword.getSense(i),hypers);
                    hypers.add("<END_SENSE>");
                }
                return hypers;
            }
        } catch (JWNLException ex) {
            ex.printStackTrace();
            return null;
        }
    }
    
    /** Returns true iff word with POS pos has the semantic type semtype */
    public boolean isSemTypeOf(String word, POS pos, String semtype) {
        Vector<String> hypers = getSemTypesOf(word, pos, 2);
        for(int i=0; i<hypers.size(); i++)
            if (hypers.elementAt(i).contentEquals(semtype))
                return true;
        return false;
    }
    
    
    private void getHypernyms(Synset sense, Vector<String> hypers)
    throws JWNLException {
        PointerTargetNodeList directhypers =
                PointerUtils.getInstance().getDirectHypernyms(sense);
        if (!directhypers.isEmpty()) {
            for(int ind=0; ind<directhypers.size(); ind++) {
                PointerTargetNode ptn = (PointerTargetNode) directhypers.get(ind);
                for(int i=0; i<ptn.getSynset().getWords().length;i++) {
                    addUnique(hypers, ptn.getSynset().getWords()[i].getLemma() );
                }
                getHypernyms( ptn.getSynset(), hypers);
            }
        }
    }
    
    public void printSenseParents(String word) {
        // Get all of the hypernyms (parents) of the first sense of <var>word</var>
        Vector<String> hypers = getSemTypesOf(word, POS.NOUN, 2);
        if (hypers==null)
            System.out.println("!!! Unknown word: "+ word );
        else {
            System.out.println("Hypernyms of \"" + word + "\":");
            for(int i=0; i<hypers.size();i++) {
                System.out.println(hypers.elementAt(i));
            }
        }
    }
    
    private void addUnique(Vector<String> svect, String s) {
        for(int i = 0; i<svect.size(); i++) {
            if (svect.elementAt(i).contentEquals(s))
                return;
        }
        svect.add(s);
    }
    
}

