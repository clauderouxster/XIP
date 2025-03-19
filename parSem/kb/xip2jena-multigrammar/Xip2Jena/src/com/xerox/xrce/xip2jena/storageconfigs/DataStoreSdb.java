package com.xerox.xrce.xip2jena.storageconfigs;

import com.xerox.xrce.xip2jena.*;
import com.hp.hpl.jena.rdf.model.Model;
import com.hp.hpl.jena.sdb.SDBFactory;
import com.hp.hpl.jena.sdb.Store;
import com.hp.hpl.jena.sdb.StoreDesc;
import com.hp.hpl.jena.sdb.sql.JDBC;
import java.io.OutputStream;
import java.io.PrintStream;
import java.sql.SQLException;
import com.hp.hpl.jena.sdb.sql.SDBConnection;
import com.hp.hpl.jena.sdb.sql.SDBExceptionSQL;
import com.hp.hpl.jena.sdb.store.LayoutType;
import org.apache.log4j.Logger;

public class DataStoreSdb extends DbImpl {

    private LayoutType dbLayout = LayoutType.LayoutTripleNodesHash;
    
    private Store store;

    private static Logger log = Logger.getLogger(DataStoreSdb.class);

    public DataStoreSdb(DbConfig config) throws ClassNotFoundException {

        StoreDesc storeDesc = new StoreDesc(dbLayout, config.getDbType());
        JDBC.loadDriver(config.getDbClass());
        SDBConnection conn = new SDBConnection(config.getDbUrl(), config.getDbUser(), config.getDbPw());
        if (log.isDebugEnabled()) {
            conn.setLogSQLQueries(true);
        }
        store = SDBFactory.connectStore(conn, storeDesc);

        init();
    }

    @Override
    public void close() {
        store.getConnection().close();
        store.close();
    }

    @Override
    public Model getDefaultModel() { return SDBFactory.connectDefaultModel(store); }

    @Override
    public void dump(OutputStream out, String format) {
        PrintStream psout = new PrintStream(out);
        getDefaultModel().write(psout, format);
    }

    @Override
    public void clean() throws SQLException {
        store.getTableFormatter().truncate();
        init();
    }

    private void init() {
        // Strangely, the SDB API doesn't provide a way to check if the tables
        // have been set up.  Setting them up again if they exist already 
        // has the effect of deleting all the previously-stored information, which
        // is not what we want.
        try {
            store.getSize();
        } catch (SDBExceptionSQL e) {
            log.info("Creating db");
            store.getTableFormatter().create();
        }
        Model defaultModel = SDBFactory.connectDefaultModel(store);
        defaultModel.setNsPrefix("indiv", Xip2Jena.indivBase);
        defaultModel.setNsPrefix("parsem", ParsemGenericOntology.NS);
    }

}
