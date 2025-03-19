package com.xerox.xrce.xip2jena.storageconfigs;

import com.xerox.xrce.xip2jena.*;
import com.hp.hpl.jena.db.DBConnection;
import com.hp.hpl.jena.db.IDBConnection;
import com.hp.hpl.jena.rdf.model.Model;
import com.hp.hpl.jena.rdf.model.ModelFactory;
import com.hp.hpl.jena.rdf.model.ModelMaker;
import com.hp.hpl.jena.util.iterator.ExtendedIterator;
import java.io.OutputStream;
import java.io.PrintStream;
import java.sql.SQLException;

public class DataStoreRdb extends DbImpl {

    private IDBConnection dbconn;
    private ModelMaker modelMaker;

    private static String dbType = "Derby";

    public DataStoreRdb(DbConfig config) throws ClassNotFoundException {
        
        // ensure the JDBC driver class is loaded
        Class.forName(config.getDbClass());

        dbconn = new DBConnection(config.getDbUrl(), config.getDbUser(),
                config.getDbPw(), dbType);

        init();
    }

    @Override
    public void close() { modelMaker.close(); }

    @Override
    public Model getDefaultModel() { return modelMaker.createDefaultModel(); }

    @Override
    public void dump(OutputStream out, String format) {
        PrintStream psout = new PrintStream(out);

        psout.println("Contents of default model:");
        getDefaultModel().write(psout, "N3");

        ModelMaker maker = ModelFactory.createModelRDBMaker(dbconn);
        ExtendedIterator it = maker.listModels();
        while (it.hasNext()) {
            String modelName = (String) it.next();
            Model model = maker.openModel(modelName, true);
            psout.println("\nContents of graph " + modelName);
            model.write(psout, format);
        }
    }

    @Override
    public void clean() throws SQLException {
        dbconn.cleanDB();
        init();
    }

    private void init() {
        modelMaker = ModelFactory.createModelRDBMaker(dbconn);
        Model defaultModel = modelMaker.createDefaultModel();
        defaultModel.setNsPrefix("indiv", Xip2Jena.indivBase);
        defaultModel.setNsPrefix("parsem", ParsemGenericOntology.NS);
    }

}
