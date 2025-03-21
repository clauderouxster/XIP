/*
 * Pair.java
 *
 * Created on February 26, 2007, 11:04 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package common;

/**
 * This class handles Pairs.
 * @author xtannier
 */
public class Pair<E1, E2>  {
    
    private E1 first;
    private E2 second;
    
    /** 
     * Creates a new instance of Pair 
     *
     * @param e1 the first element
     * @param e2 the second element
     */
    public Pair(E1 e1, E2 e2) {
        this.setFirst(e1);
        this.setSecond(e2);
    }

    /**
     * Returns the first element of the Pair
     *
     * @return the first element
     */
    public E1 getFirst() {
        return first;
    }

    /**
     * Sets the first element
     *
     * @param first the first element
     */
    public void setFirst(E1 first) {
        this.first = first;
    }

    /**
     * Returns the second element of the Pair
     *
     * @return the second element
     */
    public E2 getSecond() {
        return second;
    }

    /**
     * Sets the second element
     *
     * @param second the second element
     */
    public void setSecond(E2 second) {
        this.second = second;
    }

    /**
     * String representation of the Pair
     * @return a String representation of the Pair.
     */
    public String toString(){
        return "{" + this.getFirst().toString() + ", " + this.getSecond().toString() + "}";
    }
}
