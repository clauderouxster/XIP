/*
 * Copyright Xerox 2009
 */

package xip2jena_risk;

import com.xerox.xrce.xip2jena.storageconfigs.*;
import com.xerox.xrce.xip2jena.DataStore;

public class StorageConfigRisk {

    // make uninstantiable
    private StorageConfigRisk(){}

    public static DataStore connect() throws ClassNotFoundException {
        return new DataStoreTdbLARQ("tdbstore-risk");
    }
}
