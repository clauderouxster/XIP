/*
 * Relation.java
 *
 * Created on May 22, 2007, 3:09 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package jcoref;

import java.util.Vector;


/**
 *
 * @author ait
 */
public class Relation {
    
    String name;
    
    Referent arg1;
    Referent arg2;
    Vector<Relation> dependsOn; 
    
    /** Creates a new instance of Relation */
    public Relation(String relname, Referent arg1, Referent arg2) {
        this.name = relname;
        this.arg1 = arg1;
        this.arg2 = arg2;
    }
    
}






