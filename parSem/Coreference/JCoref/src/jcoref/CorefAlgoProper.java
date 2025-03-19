/*
 * ProperResolver.java
 *
 * Created on May 28, 2007, 4:51 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package jcoref;

import com.xerox.jxip.*;



/**
 *
 * @author ait
 */
public class CorefAlgoProper extends CorefAlgoGeneral {
    
    
    /** Creates a new instance of ProperResolver */
    public CorefAlgoProper() {
    }
    
    
    /** Tries to resolve coreference in case of ProperNames */
    CoreferenceLink resolveProperCoreference(Referent ref, ReferentSet referents) {
        Tracer.traceTryingFindAntecedents(ref);
        Referent r=null;
        float score=0;
        int i=0;
        for(i=referents.size()-1; i>=0; i--) {
            r = referents.elementAt(i);
            Tracer.traceTryingWithReferent(r);
            score = r.isCoreferentWithProper(ref);
            if (score==1) {
                break;
            }
            Tracer.traceScore(0);
        }
        
        if (i>=0) {
            Tracer.traceScore(1);
            // ref.setAntecedentLink(r, 1, rule, referents);
            CoreferenceLink cl = new CoreferenceLink(r, ref, score, rule);
            referents.setCoreferenceLink(cl);
            Tracer.traceSuccess(r);
            return cl;
        } else {
            return null;
        }

    }
    
    


    
    
}
