/*
 * PronounResolver.java
 *
 * Created on May 30, 2007, 12:34 PM
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
public class CorefAlgoPronoun extends CorefAlgoGeneral {
    

    
    /** Creates a new instance of PronounResolver */
    public CorefAlgoPronoun() {
    }
    
    
    CoreferenceLink resolvePronominalCoreference(Referent ref, ReferentSet referents) {
        Tracer.traceTryingFindAntecedents(ref);
        Referent r=null;
        Vector<CoreferenceLink> clcandidates = new Vector<CoreferenceLink>(3);
        float score=0, totalscores = 0;
        int i=0;
        for(i=referents.size()-1; i>=0; i--) {
            r = referents.elementAt(i);
            Tracer.traceTryingWithReferent(r);
            score = corefConstraintsSatisfiedBy(r,ref);
            if (score>0 && score<1) {
                // compute score more precisely
                score = getPronominalCoreferenceScore(r,ref,referents);
            }
            Tracer.traceScore(score);
            if (score>0) {
                rule = "N2 is a pronoun that co-refers with N1";
                CoreferenceLink cl = new CoreferenceLink(r,ref,score,rule);
                if (score==1) {
                    // reliable link found : keep only this and stop searching
                    clcandidates.clear();
                    clcandidates.add(cl);
                    break;
                } else {
                    clcandidates.add(cl);
                }
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
    
    
    
    
    
    
    /** Checks coreference constraints
     */
    private float corefConstraintsSatisfiedBy(Referent r, Referent r2) {
        // Number, gender and person
        if (!r.agreesInNumberWith(r2) || !r.agreesInGenderWith(r2) )
            return 0;
        
        // quotation constraints
        if ( r.isSayerOf( r2 ) ) {
            if (!r2.fMatches("P1"))
                return 0;
            else
                return 1;
        } else {
            if (!r.agreesInPersonWith(r2)) {
                return 0;
            }
        }
        
        // Lexical-Semantic constraints
        if (!this.lexsemConstraintsSatisfiedBy(r,r2)) {
            return 0;
        }
        
        // Syntactic constaints
        if (!this.syntacticConstraintsSatisfiedBy(r,r2)) {
            return 0;
        }
        return 0.5f;
    }
    
    static private float getPronominalCoreferenceScore(Referent r, Referent r2, ReferentSet referents) {
        return referents.getRelativeSalienceOf(r,r2);
    }
    
    private boolean lexsemConstraintsSatisfiedBy(Referent r, Referent r2) {
        this.failRule = "Lexical/semantic agreement: ";
        if (r.fMatches("PERSON")) {
            if (r2.flMatches("PRON,SG,PERSON:~,C_PERSON:~")) {
                this.failRule = this.failRule + "N1 is a person, N2 is not.";
                return false;
            }
        } else if (r2.fMatches("PERSON") || r2.fMatches("C_PERSON")) {
            if (!r.fMatches("PERSON")) {
                this.failRule = this.failRule + "N2 is a person, N1 is not.";
                return false;
            }
        }
        return true;
    }
    
    private boolean syntacticConstraintsSatisfiedBy(Referent r, Referent r2) {
        // Syntactic constraints should occur between xip nodes
        if (r.getSentenceId()==r2.getSentenceId()) {
            long dist = r2.getLeftTokOffset() - r.getRightTokOffset();
            if (Math.abs(dist) <=1) {
                this.failRule = "Distance between N1 and N2 is <= 1";
                return false;
            } else if (dist < 0 && !r.isArg1("CATAPHOR",r2) ) {
                this.failRule = "N2 preceeds N1 but there's no potential cataphora between N1 and N2";
                return false;
            }
        }
        XipNode verb = r2.getArg1("OBJ");
        if (!r2.fMatches("REFL")) {
            if ( verb!=null && verb.isRelHeadOf(r.getNode(),"SUBJ-N") ) {
                this.failRule = "N2 is not REFL:+, and N2 is the OBJ of the verb of which N1 is the SUBJ";
                return false;
            }
        } else {
            if (verb!=null && !verb.isRelHeadOf(r.getNode(),"SUBJ-N")) {
                this.failRule = "N2 is REFL:+, and N2 is not the OBJ of the verb of which N1 is the SUBJ";
                return false;
            }
        }
        //
        verb = r.getArg2("MOD","verb");
        if (verb!=null) {
            if (verb.getDependentIn("MOD","PRON,REL")!=null &&
                    r2.isArg2(verb) ) {
                this.failRule = "N2 is dependent of the main verb of a relative clause that modifies N1";
                return false;
            }
        }
        return true;
    }
    
    
}
