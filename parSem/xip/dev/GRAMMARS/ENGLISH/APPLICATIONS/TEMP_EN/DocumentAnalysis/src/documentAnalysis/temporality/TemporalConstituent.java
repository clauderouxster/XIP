/*
 * TemporalElement.java
 *
 * Created on December 12, 2006, 9:09 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package documentAnalysis.temporality;

import common.NotImplementedException;
import java.util.Vector;
import com.xerox.jxip.*;
import documentAnalysis.Element;
import java.util.HashMap;
import java.util.Map;

/**
 * Abstract class representing dates and durations
 * @author xtannier
 * @see documentAnalysis.temporality.TemporalGraph
 */
public abstract class TemporalConstituent extends Element implements TemporalElement {
    private Vector<TemporalRelation> temporalRelations;
    private int mod;
    
    
    
    /********************
     * temporal mod
     ********************/
    /** exact */
    public static int EXACT = 1;
    /** approximate */
    public static int APPROX = 2;
    /** start */
    public static int START = 3;
    /** end */
    public static int END = 4;
    /** mid */
    public static int MID = 5;
    /** less than, equal or less */
    public static int LESS_THAN = 6;
    /** equal or less than */
    public static int EQUAL_OR_LESS = 7;
    /** more than, equal or more */
    public static int MORE_THAN = 8;
    /** equal or more then */
    public static int EQUAL_OR_MORE = 9;
    /** undefined */
    public static int UNDEFINED = 0;
    private static HashMap<Integer, String> mapMod;
    
    static {
        mapMod = new HashMap<Integer, String>();
        mapMod.put(new Integer(EXACT), "EXACT");
        mapMod.put(new Integer(APPROX), "APPROX");
        
        mapMod.put(new Integer(START), "START");
        mapMod.put(new Integer(END), "END");
        mapMod.put(new Integer(MID), "MID");
        
        mapMod.put(new Integer(LESS_THAN), "LESS_THAN");
        mapMod.put(new Integer(MORE_THAN), "MORE_THAN");
        mapMod.put(new Integer(EQUAL_OR_MORE), "EQUAL_OR_MORE");
        mapMod.put(new Integer(EQUAL_OR_LESS), "EQUAL_OR_LESS");
    }
    
    
    /********************
     * temporal granularity
     ********************/
    /** MILLENNIUM */
    public static int MILLENNIUM = -3;
    /** CENTURY */
    public static int CENTURY = -2;
    /** DECADE */
    public static int DECADE = -1;
    /** YEAR */
    public static int YEAR = 1;
    /** MONTH */
    public static int MONTH = 2;
    /** WEEK */
    public static int WEEK = 3;
    /** DAY */
    public static int DAY = 5;
    /** HOUR */
    public static int HOUR = 10;
    /** MINUTE */
    public static int MINUTE = 12;
    /** SECOND */
    public static int SECOND = 13;
    
    private static HashMap<Integer, Integer> mapGranularity;
    
    static {
        mapGranularity = new HashMap<Integer, Integer>();
        mapGranularity.put(MILLENNIUM, 0);
        mapGranularity.put(CENTURY, 10);
        mapGranularity.put(DECADE, 10);
        mapGranularity.put(YEAR, 10);
        mapGranularity.put(MONTH, 12);
        mapGranularity.put(WEEK, 4);
        mapGranularity.put(DAY, 30);
        mapGranularity.put(HOUR, 24);
        mapGranularity.put(MINUTE, 60);
        mapGranularity.put(SECOND, 60);
    }
    
    
    /**
     * Builds an undefined temporal element.
     */
    public TemporalConstituent(){
        super();
        this.temporalRelations = new Vector<TemporalRelation>();
        this.mod = UNDEFINED;
    }
    
    /**
     * Builds a temporal element with the specified XipNode.
     * @param unit the XipUnit associated to the temporal element.
     * @param node the XipNode associated to the temporal element.
     */
    public TemporalConstituent(XipUnit unit, XipNode node){
        super(unit, node);
        this.temporalRelations = new Vector<TemporalRelation>();
        this.mod = UNDEFINED;        
    }

    
    
    
    
//    /**
//     * Clone associated XipNode information only from specified element
//     * @param element the TemporalElement to clone
//     */
//    protected void cloneXipNode(TemporalElement element){
//        this.node = element.node;
//        this.sentenceNumber = element.sentenceNumber;
//    }
    
    
    /**
     * Returns a Vector containing temporal relations coming to or leaving the node.
     * @return a Vector containing temporal relations coming to or leaving the node.
     */
    public Vector<TemporalRelation> getTemporalRelations() {
        return this.temporalRelations;
    }
    
    
    /**
     * Returns a Vector containing temporal relations leaving the element
     * add having the specified relation identifier and with or without interval.
     * @param temporalElement the temporal element to look into
     * @param relation the int identifier of the excepted relation, or
     *  TemporalRelation.UNDEFINED if any relation identier is possible.
     * @param withInterval 1 if a non-null interval is expected,
     *  -1 if an withInterval is rejected, 0 if any relation is wanted.
     * @return a Vector containing temporal relations leaving the node and having the correct
     * attributes.
     */
    public static Vector<TemporalRelation> getTemporalRelations(TemporalElement temporalElement,
            int relation, int withInterval) {
        Vector<TemporalRelation> result = new Vector<TemporalRelation>();
        for (TemporalRelation temporalRelation : temporalElement.getTemporalRelations()){
            if ((relation == TemporalRelation.UNDEFINED ||
                    relation == temporalRelation.getRelation() ||
                    TemporalRelation.getInverseRelation(relation) == temporalRelation.getRelation()) &&
                    (withInterval == 0 ||
                    (withInterval == 1 && temporalRelation.getInterval() != null &&
                    !temporalRelation.getInterval().isNull()) ||
                    (withInterval == -1 && (temporalRelation.getInterval() == null ||
                    temporalRelation.getInterval().isNull()))))
                result.add(temporalRelation);
        }
        return result;
    }    
    
    /**
     * Returns a Vector containing temporal relations leaving the node
     * add having the specified relation identifier and with or without interval.
     * @param relation the int identifier of the excepted relation, or
     *  TemporalRelation.UNDEFINED if any relation identier is possible.
     * @param withInterval 1 if a non-null interval is expected,
     *  -1 if an interval is rejected, 0 if any relation is wanted.
     * @return a Vector containing temporal relations leaving the node and having the correct
     * attributes.
     * @see #getTemporalRelations(TemporalElement temporalElement,int relation,int withInterval)
     */
    public Vector<TemporalRelation> getTemporalRelations(int relation, int withInterval) {
        return getTemporalRelations(this, relation, withInterval);
    }
    
    
    /**
     * Adds a temporal relation to the specified temporal element.
     *
     * @param temporalElement the temporal element to which the relation
     * must be added
     * @param relation the temporal relation to add to the graph
     */
    public static void addTemporalRelation(TemporalElement temporalElement,
            TemporalRelation relation){
//        if (relation.getTemporalElement1().toString().startsWith("operation"))
//            System.err.println("");
        int i = temporalElement.getTemporalRelations().indexOf(relation);

        if (!relation.getTemporalElement1().equals(temporalElement))
            relation = relation.inverse();
        
        if (i == -1){
            temporalElement.getTemporalRelations().add(relation);
        } else {
            temporalElement.getTemporalRelations().set(i, relation);
        }
    } 
    
    
    /**
     * Adds a temporal relation to the temporal constituent.
     * @param relation the temporal relation to add to the graph
     * @see TemporalConstituent#addTemporalRelation(TemporalRelation relation)
     */
    public void addTemporalRelation(TemporalRelation relation){
        addTemporalRelation(this, relation);
    }

    
    
    
    /**
     * Replaces a relation by another in the element
     *
     * @param temporalElement the temporal element
     * @param oldRelation the old relation
     * @param newRelation the new relation
     */
    public static void replaceTemporalRelation(TemporalElement temporalElement,
            TemporalRelation oldRelation, 
            TemporalRelation newRelation) {
        if (oldRelation == null || newRelation == null)
            throw new NullPointerException();
        TemporalRelation relation2;
        for (int i = 0; i < temporalElement.getTemporalRelations().size(); i++) {
            relation2 = temporalElement.getTemporalRelations().get(i);
            if (oldRelation.same(relation2)){
                temporalElement.getTemporalRelations().remove(i);
                temporalElement.addTemporalRelation(newRelation);
                return;
            }
        }
        temporalElement.addTemporalRelation(newRelation);
    }    
    
    
    /**
     * Replaces a relation by another in the element
     *
     * @param oldRelation the old relation
     * @param newRelation the new relation
     * @see TemporalConstituent#replaceTemporalRelation(TemporalElement temporalElement,
            TemporalRelation oldRelation, 
            TemporalRelation newRelation)
     */
    public void replaceTemporalRelation(TemporalRelation oldRelation, 
            TemporalRelation newRelation) {
        replaceTemporalRelation(this, oldRelation, newRelation);
    }
    
    
    /**
     * Returns the int value of the MOD attribute (EXACT, APPROX, etc.).
     * @return the int value of the MOD attribute
     */
    public int getMod() {
        return mod;
    }
    
    /**
     * Set the int value of the MOD attribute (EXACT, APPROX, etc.).
     * @param mod the int value of the MOD attribute
     */
    public void setMod(int mod) {
        this.mod = mod;
    }
    
    
    /**
     * Tests whether the element is really defined.
     *
     * @return true if the element is really defined, false otherwise.
     */
    public abstract boolean isDefined();
        
    
    
    /**
     * Returns the String value of the specified int mod.
     * @param mod the int value of the mod
     * @return the String value of the mod.
     */
    public static String modTypeToString(int mod){
        return mapMod.get(new Integer(mod));
    }
    
    /**
     * Returns the int value of the specified String mod.
     * @param mod the String value of the mod
     * @return the int value of the mod.
     */
    public static int stringToModType(String mod){
        String value;
        for (Map.Entry<Integer, String> entry : mapMod.entrySet()) {
            value = entry.getValue();
            if (value.equals(mod.toUpperCase())){
                return entry.getKey().intValue();
            }
        }
        throw new NotImplementedException("Mod " + mod + " is unknown.");
    }
    
    
    /**
     * Returns the granularity that is immediately bigger than the specified one.
     * @param granularity the granularity
     * @return the granularity that is immediately bigger than the specified one.
     */
    public static int getBiggerGranularity(int granularity){
        if (granularity == MILLENNIUM)
            return MILLENNIUM;
        else if (granularity == CENTURY)
            return MILLENNIUM;
        else if (granularity == DECADE)
            return CENTURY;
        else if (granularity == YEAR)
            return DECADE;
        else if (granularity == MONTH)
            return YEAR;
        else if (granularity == WEEK)
            return MONTH;
        else if (granularity == DAY)
            return MONTH;
        else if (granularity == HOUR)
            return DAY;
        else if (granularity == MINUTE)
            return HOUR;
        else if (granularity == SECOND)
            return MINUTE;
        else
            throw new NotImplementedException("Unknown granularity");
    }
    
    /**
     * Returns the granularity that is immediately lower than the specified one.
     * @param granularity the granularity
     * @return the granularity that is immediately lower than the specified one.
     */
    public static int getSmallerGranularity(int granularity){
        if (granularity == MILLENNIUM)
            return CENTURY;
        else if (granularity == CENTURY)
            return DECADE;
        else if (granularity == DECADE)
            return YEAR;
        else if (granularity == YEAR)
            return MONTH;
        else if (granularity == MONTH)
            return DAY;
        else if (granularity == WEEK)
            return DAY;
        else if (granularity == DAY)
            return HOUR;
        else if (granularity == HOUR)
            return MINUTE;
        else if (granularity == MINUTE)
            return SECOND;
        else if (granularity == SECOND)
            return SECOND;
        else
            throw new NotImplementedException("Unknown granularity");
    }
    
    
    /**
     * Returns an approximate maximum number of elements of a specified granularity
     * in a bigger element.
     * eg. for month, the answer is 12.
     * @param granularity the granularity
     * @return the approximate maximum number.
     */
    public static int getApproximateMaxNumber(int granularity){
        return mapGranularity.get(granularity).intValue();
    }


    /**
     * Tests if the specified mod corresponds to a moment (beginning, middle, end)
     * and not to a vagueness (exact, approx, less_than)
     *
     * @param mod the mod to test
     * @return true if the specified mod corresponds to a moment, false otherwise
     */
    public static boolean isModMoment(int mod) {
        return mod == END || mod == MID || mod == START;
    }
    

}
