/*
 * Person.java
 *
 * Created on June 1, 2007, 3:26 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package documentAnalysis.namedEntities;

import com.xerox.jxip.XipNode;
import com.xerox.jxip.XipUnit;
import java.util.Vector;

/**
 *
 * @author xtannier
 */
public class Person extends Entity {
    
    private String lastname;
    private String firstname;    
    private String title;
    private String fullName;
    
    
    /**
     * Creates a new instance of Person from a single String
     *
     * @param name the String representation of the person
     */
    public Person(String name){
        super();
        this.fullName = name;
    }
    
    /** Creates a new instance of Person
     *
     * @param unit the XipUnit associated to the person.
     * @param node the XipNode associated to the person.
     */
    public Person(XipUnit unit, XipNode node) {
        super(unit, node);
        // first name
        Vector<XipNode> children = node.getDescendantsByFeature("firstname");
        if (!children.isEmpty())
            this.firstname = children.get(0).getSurface();
        // lastname
        children = node.getDescendantsByFeature("last");
        if (!children.isEmpty())
            this.lastname = children.get(0).getSurface();
        // title
        children = node.getDescendantsByFeature("title");
        if (!children.isEmpty())
            this.title = children.get(0).getSurface();
        // fullname
        this.fullName = node.getSurface();
    }
    


    /**
     * Returns the last name
     *
     * @return the last name
     */
    public String getLastname() {
        return lastname;
    }

    /**
     * Returns the first name
     *
     * @return the first name
     */
    public String getFirstname() {
        return firstname;
    }

    /**
     * Returns the title
     *
     * @return the title
     */
    public String getTitle() {
        return title;
    }

    /**
     * Returns the full representation
     *
     * @return the fullrepresentation
     */
    public String getFullName() {
        return fullName;
    }
       
    /**
     * Returns a String representation of the current element.
     * @return a String representation of the current element.
     */
    public String toString(){
        return this.fullName;
    }    

    
    /**
     * Get a detailed String representation of the entity
     *
     * @return a detailed String representation of the entity
     */    
    public String toDetailedString() {
        String result = "PERSON: " + this.fullName;
        if (this.firstname != null) {
            result += "\n   first name: " + this.firstname;
        }
        if (this.lastname != null) {
            result += "\n    last name: " + this.lastname;
        }
        if (this.title != null) {
            result += "\n        title: " + this.title;
        }        return result;
    }
    
}
