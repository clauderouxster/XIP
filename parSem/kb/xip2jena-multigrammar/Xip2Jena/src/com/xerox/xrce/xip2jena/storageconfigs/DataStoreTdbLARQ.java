/*
 * To be debugged - Index is empty (no .cfs file)
 */

package com.xerox.xrce.xip2jena.storageconfigs;

import com.hp.hpl.jena.query.larq.IndexBuilderString;
import com.hp.hpl.jena.query.larq.IndexLARQ;
import com.hp.hpl.jena.query.larq.LARQ;

/**
 *
 */
public class DataStoreTdbLARQ extends DataStoreTdb {

    public DataStoreTdbLARQ (String aDirectory) throws ClassNotFoundException {
        super (aDirectory);
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
