/*
 * NominalResolver.java
 *
 * Created on May 31, 2007, 5:06 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package jcoref;

import com.xerox.jxip.XipNode;
import java.util.Collections;
import java.util.Vector;

/**
 *
 * @author ait
 */
public class CorefAlgoNoun extends CorefAlgoGeneral {

    
    /** Creates a new instance of NominalResolver */
    public CorefAlgoNoun() {
    }
    
    
    
    
    CoreferenceLink resolveNominalCoreference(Referent ref, ReferentSet referents) {
        Tracer.traceTryingFindAntecedents(ref);
        Referent r=null;
        Vector<CoreferenceLink> clcandidates = new Vector<CoreferenceLink>(3);
        float totalscores = 0;
        int i=0;
        for(i=referents.size()-1; i>=0; i--) {
            r = referents.elementAt(i);
            Tracer.traceTryingWithReferent(r);
            float score = this.getNominalCoreferenceScore(r,ref, referents );
            Tracer.traceScore(score);
            if (score>0) {
                this.rule = "N2 is a definite NP the head of which is a semantic property of N1";
                CoreferenceLink cl = new CoreferenceLink(r,ref,score,this.rule);
                clcandidates.add(cl);
                totalscores = totalscores + score;
            }
        }
        if (clcandidates.isEmpty()) {
            Tracer.traceFail();
            return null;
        } else {
            // this.normalizeScores(clcandidates, totalscores);
            Collections.sort(clcandidates);
            referents.setCoreferenceLinkCandidates(clcandidates);
            Tracer.traceSuccess(clcandidates.firstElement().antecedent);
            return clcandidates.firstElement();
        }
    }
    
    
    private float getNominalCoreferenceScore(Referent r, Referent r2, ReferentSet referents) {
        float score = referents.getSemanticMatchingScore(r,r2);
        if (score==0)
            return 0;
        score = (this.W_SEMANTIC_MATCHING*score +
                this.W_RELATIVE_SALIENCE*
                referents.getRelativeSalienceOf(r,r2) ) /
                (this.W_SEMANTIC_MATCHING + this.W_SEMANTIC_MATCHING ) ;
        return score;
    }
    
    
    
}
