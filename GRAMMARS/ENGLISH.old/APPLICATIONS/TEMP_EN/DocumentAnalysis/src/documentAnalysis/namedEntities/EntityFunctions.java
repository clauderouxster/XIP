/*
 * EntityFunctions.java
 *
 * Created on June 1, 2007, 3:40 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package documentAnalysis.namedEntities;

import com.xerox.jxip.XipDependency;
import com.xerox.jxip.XipNode;
import com.xerox.jxip.XipUnit;
import common.Options;
import java.util.Vector;

/**
 * This class contains all methods to handle named entities
 * @author xtannier
 */
public class EntityFunctions {
    
    /**
     * Extracts any information concerning entities from a XipUnit
     *
     * @param unit the XipUnit 
     * @param options the parser options
     */
    public static void parseEntities(XipUnit unit, Options options) {
        EntitySet entitySet = options.getEntitySet();
        
        // ORGANISATIONs
        Vector<XipDependency> dependencies = unit.getDependenciesByName("ORGANISATION");
        XipNode node;
        for (XipDependency dependency : dependencies) {
            node = dependency.getArg(1);
            entitySet.add(new Organisation(unit, node));
        }
        
        // PERSONs
        dependencies = unit.getDependenciesByName("PERSON");
        for (XipDependency dependency : dependencies) {
             node = dependency.getArg(1);
             entitySet.add(new Person(unit, node));
        }        
        
        // LOCATIONs
        dependencies = unit.getDependenciesByName("LOCATION");
        for (XipDependency dependency : dependencies) {
             node = dependency.getArg(1);
             entitySet.add(new Location(unit, node));
        }        
        
    }
}
