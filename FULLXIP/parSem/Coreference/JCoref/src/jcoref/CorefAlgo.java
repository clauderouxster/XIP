/*
 * CorefAlgo.java
 *
 * Created on 10 aout 2007, 23:51
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package jcoref;

/**
 *
 * @author sam
 */
public class CorefAlgo {
    
    private CorefAlgoProper properResolver = new CorefAlgoProper();
    private CorefAlgoPronoun pronounResolver = new CorefAlgoPronoun();
    private CorefAlgoNoun nounResolver = new CorefAlgoNoun();
    
    
    /** Creates a new instance of CorefAlgo */
    public CorefAlgo() {
    }
        
    //
    protected CoreferenceLink resolveCoreferenceFor(Referent ref, ReferentSet referents) {
        CorefResolver.RefType reftype = ref.getRefType();
        if (reftype==CorefResolver.RefType.NEW_REFERENT) {
            return null;
        } else if (reftype==CorefResolver.RefType.PROPER) {
            // PROPER
            return properResolver.resolveProperCoreference(ref, referents);
        } else if (reftype==CorefResolver.RefType.ANAPHORIC_PRONOMINAL) {
            // ANAPHORIC PRONOUN
            // check if node occurs inside a quotation and
            // is "said" by another node
            return pronounResolver.resolvePronominalCoreference(ref, referents);
        } else if (reftype==CorefResolver.RefType.ANAPHORIC_NOMINAL) {
            // ANAPHORIC NPs and NOUNs
            return nounResolver.resolveNominalCoreference(ref, referents);
        } else {
            return null;
        }
    } 
    
    
}
