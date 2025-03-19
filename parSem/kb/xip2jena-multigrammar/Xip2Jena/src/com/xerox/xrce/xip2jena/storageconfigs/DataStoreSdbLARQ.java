/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.xerox.xrce.xip2jena.storageconfigs;

import com.xerox.xrce.xip2jena.*;
import com.hp.hpl.jena.query.larq.IndexBuilderString;
import com.hp.hpl.jena.query.larq.IndexLARQ;
import com.hp.hpl.jena.query.larq.LARQ;

/**
 * 
 */
public class DataStoreSdbLARQ extends DataStoreSdb {

    public DataStoreSdbLARQ (DbConfig config) throws ClassNotFoundException {
        super (config);
        init();
    }
    
    private void init()
    {
        // -- Read and index all literal strings.
        IndexBuilderString larqBuilder = new IndexBuilderString() ;
        // -- Create an index based on existing statements
        larqBuilder.indexStatements(getDefaultModel().listStatements());
        larqBuilder.closeWriter();
        // ---- Create the access index
        IndexLARQ index = larqBuilder.getIndex() ;
        LARQ.setDefaultIndex(index);
    }

}
