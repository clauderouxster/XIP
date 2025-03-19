package com.xerox.xrce.xip2jena;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

/**
 *
 * @author akaplan
 */
public class DependencyTranslation {
    private Map<String,String> individualDependencies;
   
    public DependencyTranslation(String filename) throws FileNotFoundException, IOException {
        individualDependencies = new HashMap<String,String>();
        BufferedReader input = new BufferedReader(new FileReader(filename));
        String line = null;
        while ((line = input.readLine()) != null) {
            individualDependencies.put(line,line.substring(0, 1).toUpperCase() + line.substring(1).toLowerCase());
        }
    }
    
    public String depName2IndivClass(String depname) {
        return individualDependencies.get(depname);
    }
}
