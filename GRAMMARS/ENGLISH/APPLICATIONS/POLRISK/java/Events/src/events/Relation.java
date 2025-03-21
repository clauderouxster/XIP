/*
 * Relation.java
 * 
 * Created on Jul 20, 2007, 2:12:58 PM
 * 
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package events;

/**
 *
 * @author arebotie
 */
public abstract class Relation {

    private Object entity1;
    private Object entity2;
    
    public Relation(Object entity1, Object entity2) {
      this.entity1 = entity1;
      this.entity2 = entity2;
    }
public Object getEntity1(){
return this.entity1;
}

public Object getEntity2(){
return this.entity2;
}

public void setEntities(Object entity1, Object entity2){
    this.entity1 = entity1;
    this.entity2 = entity2;
}


}

