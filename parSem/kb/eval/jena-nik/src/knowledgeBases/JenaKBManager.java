/*
 * KBManager.java
 *
 * Created on August 17, 2007, 2:29 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package knowledgeBases;

import java.io.IOException;
import java.net.MalformedURLException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
        
import java.util.logging.Level;
import org.mindswap.pellet.jena.PelletQueryExecution;
import org.mindswap.pellet.jena.PelletReasoner;
import org.mindswap.pellet.jena.PelletReasonerFactory;
import org.mindswap.pellet.jena.PelletInfGraph;

import com.hp.hpl.jena.ontology.OntDocumentManager;
import com.hp.hpl.jena.ontology.OntModel;
import com.hp.hpl.jena.ontology.OntModelSpec;

import com.hp.hpl.jena.query.DataSource;
import com.hp.hpl.jena.query.Dataset;
import com.hp.hpl.jena.query.Query;
import com.hp.hpl.jena.query.QueryExecution;
import com.hp.hpl.jena.query.QueryExecutionFactory;
import com.hp.hpl.jena.query.QueryFactory;

import com.hp.hpl.jena.query.ResultSet;
import com.hp.hpl.jena.query.ResultSetFormatter;

import com.hp.hpl.jena.rdf.model.InfModel;
import com.hp.hpl.jena.rdf.model.Model;
import com.hp.hpl.jena.rdf.model.ModelFactory;

import com.hp.hpl.jena.rdf.model.Property;
import com.hp.hpl.jena.rdf.model.RDFNode;
import com.hp.hpl.jena.rdf.model.ReifiedStatement;
import com.hp.hpl.jena.rdf.model.Resource;
import com.hp.hpl.jena.rdf.model.ResourceFactory;
import com.hp.hpl.jena.rdf.model.Statement;
import com.hp.hpl.jena.rdf.model.StmtIterator;

import com.hp.hpl.jena.reasoner.Reasoner;
import com.hp.hpl.jena.reasoner.ReasonerRegistry;
import com.hp.hpl.jena.reasoner.dig.DIGReasoner;
import com.hp.hpl.jena.reasoner.dig.DIGReasonerFactory;
import com.hp.hpl.jena.reasoner.rulesys.GenericRuleReasoner;
import com.hp.hpl.jena.reasoner.rulesys.Rule;

import com.hp.hpl.jena.shared.WrappedIOException;
import com.hp.hpl.jena.util.FileManager;
import com.hp.hpl.jena.util.PrintUtil;
import com.hp.hpl.jena.vocabulary.ReasonerVocabulary;
import org.apache.log4j.Logger;

import com.hp.hpl.jena.sdb.Store;
import com.hp.hpl.jena.sdb.StoreDesc;
import com.hp.hpl.jena.sdb.sql.SDBConnection;
//import com.hp.hpl.jena.sdb.store.*;
import com.hp.hpl.jena.sdb.SDBFactory;

import com.hp.hpl.jena.query.DatasetFactory;
import com.hp.hpl.jena.query.larq.IndexBuilderString;
import com.hp.hpl.jena.query.larq.IndexLARQ;

import com.hp.hpl.jena.rdf.model.ModelMaker;
import com.hp.hpl.jena.sdb.store.DatabaseType;
import com.hp.hpl.jena.sdb.store.DatasetStore;
import com.hp.hpl.jena.sdb.store.LayoutType;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.OutputStream;
import java.net.URI;
import java.net.URL;
import java.util.Enumeration;
import java.util.Properties;
import java.util.ResourceBundle;
import java.util.Vector;
import tests.ResultsViewer;

/**
 * The KBManager for Jena KB. This is the major implementation part of jena
 * framework. This manager extends the KBManager for specific configuration, 
 * namely, it uses Jena Framework as the foundation.
 * 
 * 
 * @author tlu
 * @author nl
 * @version 1.8
 */
public class JenaKBManager extends KBManager {

    //protected static Category log = Category.getInstance(JenaKBManager.class.getName());
    static Logger log = Logger.getLogger(JenaKBManager.class);
    OntModel schema;
    Model data, temp;
    //OntModel deductModel;
    Reasoner reasoner;
    InfModel kb;
    OntModelSpec s;
    SDBConnection conn;
    //Dataset data;
    //IDBConnection conn;
    
    Store store;
    IndexLARQ index;
    String reasonerType;
    DataSource ds;
    
    /**
     * Use Jena framework and KAON reasoner, bind KAON reasoner and persistent
     * DB
     * TODO: Doesn't work properly through DIG_1.1 interface-Does it worth implementing, still not good enough for scalability
     */
    public final static String JENA_OWL_KAON2 = "jena-owl-kaon2";
    /**
     * Use Jena framework and pellet reasoner, bind pellet reasoner and persistent
     * DB--Practical use only for classification and materialisation of data
     */
    public final static String JENA_PELLET_DB = "jena-pellet-pellet-db";
    /**
     * Use Jena framework and pellet reasoner, bind pellet reasoner and store the model in memory
     * Note: Not recommended
     */
    public final static String JENA_PELLET_MEM = "jena-pellet-pellet-mem";
    /**
     * Use Jena framework and the OWL-DL native reasoner in memory.
     */
    public final static String JENA_DL_MEM = "jena-dl-dl-mem";
    /**
     * Use Jena framework and the OWL-DL native reasoner backed up by a persistent model stored in a database.
     */
    public final static String JENA_DL_DB = "jena-dl-dl-db";

    
    /** Creates a new instance of KBManager
     * @param model
     * @param reasoner
     * @param database 
     */
    public JenaKBManager(String model, String reasoner, String database) throws Exception {
        
        /**
         * Default: Using OWL_MEM_RULE_INF over TBox and OWL_Micro to create inference model
         */
        OntDocumentManager mgr = new OntDocumentManager();
        
        
        s = new OntModelSpec(OntModelSpec.OWL_MEM_RULE_INF);
        s.setDocumentManager(mgr);
        
        // specific setting
        if (model.equals("jena-pellet") || reasoner.equals("pellet")) {
            log.info("Using Pellet to build the model");
            log.info("---------------------");
            log.info("");
            // create an empty ontology model using Pellet spec
            this.schema = ModelFactory.createOntologyModel(PelletReasonerFactory.THE_SPEC);
            if (database.equals("db")) {
                bindDB("jena-owl-pellet-test");
            } else {
                // Store model in memory
                this.data = ModelFactory.createDefaultModel();
            }
        }
        
        // Not implemented
        else if (model.equals("jena-owl") || reasoner.equals("kaon")) {
            log.info("Using KAON2 to build the model");
            log.info("---------------------");
            log.info("");
        }
        
        else {
            this.schema = ModelFactory.createOntologyModel(s);
            if (database.equals("db")){
                bindDB("jena_owl_db");
                log.info("Using Native Resoner "+reasoner+" with DB to build the model");
            } else {
                this.data = ModelFactory.createDefaultModel();
                log.info("Using Native Reasoner to build the model");
            }
            log.info("---------------------");
            log.info("");
        }
        
    }

    public void bindDB(String modelName) throws ClassNotFoundException {
        /*
        // create a RDB database connection
        Class.forName(M_DBDRIVER_CLASS);
        conn = new DBConnection(M_DB_URL, M_DB_USER, M_DB_PASSWD,M_DB);

        // create a model maker with the given connection parameters
        ModelMaker maker = ModelFactory.createModelRDBMaker(conn);
        this.data = maker.createModel(modelName);

         */

        //The SDB connection

        /**
         * Describing the database Layout
         *** LayTripleNodesHash - faster on querying
         *** LayTripleNodesIndex - faster on loading (not mature)
         * and database Type
         */
        StoreDesc storeDesc = new StoreDesc(LayoutType.LayoutTripleNodesHash,
                M_DB_TYPE);


        Class.forName(M_DBDRIVER_CLASS);

        this.conn = new SDBConnection(M_DB_URL, M_DB_USER, M_DB_PASSWD);

        this.store = SDBFactory.connectStore(conn, storeDesc);

        //Use a named model as the database-backed inference model to avoid changes in the Default model holding all the metadata.
        this.data = SDBFactory.connectNamedModel(store, "file:/InferenceModel");

        //Clean the model for storing the new inferences - required for testing - should we keep the inference model unless all the database is cleaned?
        data.removeAll();

        //Add the metadata
        data.add(SDBFactory.connectDefaultModel(store));

        log.info("database bound: [" + M_DB_URL + "] [" + M_DB_USER + "] [" + M_DB_PASSWD + "] [" + M_DB_TYPE.getName() + "].");

    }
    
    // Choose the reasoner to bind
    public void bindReasoner(String reasonerType) {
  
       if (reasonerType.contains("internal"))
            bindInternalReasoner();
        else if (reasonerType.contains("external"))
            bindExternalReasoner(reasonerType);
        else if (reasonerType.contains("rules"))
            bindRulesReasoner(reasonerType);
        else
            log.error("Problem binding the reasoner. \n");
    }
    
    /*
     * Bind external reasoner 
     */
    private void bindInternalReasoner() {
        reasoner = ReasonerRegistry.getOWLMicroReasoner();
        reasoner = reasoner.bindSchema(schema);
    }

    /*
     * Bind external reasoner 
     */
    private void bindExternalReasoner(String location) {
        if (location.contains("pellet")) {
            // create Pellet reasoner using the facility of Pellet
            // "Jena-built-in-model" reasoner
            reasoner = PelletReasonerFactory.theInstance().create();
            reasoner = reasoner.bindSchema(schema);
        } else {
            //use DIG interface to bind external reasoner
            bindDIGCompatibleReasoner(location);
        }
    }

    /**
     * This method provides the ability to bind external reasoner, which is DIG
     * compatible.
     * 
     * @param url
     */
    private void bindDIGCompatibleReasoner(String url) {
        // set up a configuration resource to connect to the reasoner
        Resource conf = data.createResource();
        conf.addProperty(ReasonerVocabulary.EXT_REASONER_URL, data.createResource(url));
        // create the reasoner factory and the reasoner
        DIGReasonerFactory drf = (DIGReasonerFactory) ReasonerRegistry.theRegistry().getFactory(
                DIGReasonerFactory.URI);
        DIGReasoner r = (DIGReasoner) drf.createWithOWLAxioms(conf);
        this.reasoner = r;
        s.setReasoner(r);
    }

    /*
     * Bind the Jena rules reasoner
     */ 
    private void bindRulesReasoner(String url) {
        List rules = Rule.rulesFromURL(url);
        reasoner = new GenericRuleReasoner(rules);
    }

    /*
     * Build KB
     * - load knowledge
     * - load data
     * - load reasoner
     * - materialise inferred triples
     * - 
     */ 
    public boolean buildKB(String ontologyURL, String schemaURL, List<String> dataLocations, String reasonerURL) {
        
        reasonerType = reasonerURL;
        
        // bind schema
        if (schemaURL == null || schemaURL.equals("")) {
            log.error("schemaURL: " + schemaURL);
            return false;
        }
        loadBackgroundKnowledge(schemaURL, ontologyURL);
        
        // load data
        if (dataLocations == null || dataLocations.isEmpty()) {
            log.warn("dataLocation: " + dataLocations + " is empty.");
            log.warn("Duplicated load the schema ontology as data.");
            // get the data locations
            dataLocations = new ArrayList<String>();
            dataLocations.add(schemaURL);
        }
        String dataLocation;
        for (Iterator it = dataLocations.iterator(); it.hasNext();) {
            dataLocation = it.next().toString();
            loadData(dataLocation);
        }
        
        if (reasonerURL.equals("")){
            log.warn("No reasoner defined-direct SPARQL query on database");
        }
        else{
            //bind reasoner
            bindReasoner(reasonerURL);

            kb = ModelFactory.createInfModel(reasoner, data);
             
            // Materialise KB before query time
            materialiseKB(kb);
            //renderKB();
        }

        return true;
    }
    
   /*
    // Add a model
    private void addToMainModel(Model model, Model tModel) {
        log.info("adding " + tModel.size() + " triples");
            model.begin();
	log.info("began transaction");
            model.add(tModel);
	log.info("ready to commit");
            model.commit();
	log.info("removing raw triples from temporary model");
            tModel.removeAll();
	log.info("done adding -- ");
            tModel.close();
    }*/

    //TO DO:Incomplete
    private IndexLARQ luceneIndexing (Model m){
        
        // -- Read and index all literal strings.
        IndexBuilderString larqBuilder = new IndexBuilderString() ;
        // -- Create an index based on existing statements
        larqBuilder.indexStatements(m.listStatements()) ;
        // -- Finish indexing
        larqBuilder.closeForWriting();
        // -- Create the access index  
        IndexLARQ indexLocal = larqBuilder.getIndex() ;
        
        /*NodeIterator nIter = index.searchModelByIndex("+document") ;
        for ( ; nIter.hasNext() ; )
        {
        // if it's an index storing literals ...
            Literal lit = (Literal)nIter.nextNode();
            System.out.println(lit);
        }*/
        return indexLocal;
        
    }
    
    // Print all statements
    public void printStatements(Model m, Resource s, Property p, Resource o) {
        for (StmtIterator i = m.listStatements(s, p, o); i.hasNext();) {
            Statement stmt = i.nextStatement();
            log.info(" - " + PrintUtil.print(stmt));
        }
    }

    // Render the KB -- check with printStatements
    private void renderKB() {
        int count = 0;
        for (StmtIterator i = data.listStatements(); i.hasNext();) {
            Statement stmt = i.nextStatement();
            count++;
            log.info(" - " + PrintUtil.print(stmt));
        }
        
        System.out.println("The size of the inferred model is: "+count);
    }
    
    
    // Perform initial processing and caching. Forward rules should all fire here.
    private void prepareKB(InfModel kb) {
        System.out.print( "Preparing..." );
        kb.prepare();
        System.out.println( "done" );
    }
    
    // Perform classification of a Pellet model
    private void classifyKB(InfModel kb) {
        System.out.print( "Classifying..." );
        ((PelletInfGraph) kb.getGraph()).getKB().classify();
        System.out.println( "done" );
    }
    
    private void realizeKB(InfModel kb){
        System.out.println( "Realising..." );
        ((PelletInfGraph) kb.getGraph()).getKB().realize();
        System.out.println( "done" );
    }
    
    private void materialiseKB(InfModel kb){
        prepareKB(kb);
        if (this.kb.getReasoner() instanceof PelletReasoner){
            classifyKB(kb);
            realizeKB(kb);
        }
        
    }
    
    public void loadData(String location) {
        Model newData = FileManager.get().loadModel(location);
        data.add(newData);
        log.info("OWL instances successfully loaded.");
    }

    public void loadBackgroundKnowledge(String location, String xmlBase) {
        try {
            setXmlBase(xmlBase);
            log.info("Creating OntologyModel for " + location + " ...");
            // Manage the imports
            manageImports();
            schema.read(location);
            //addMappings();
        } catch (WrappedIOException e) {
            log.info("Something wrong by loading the ontology: " + location);
            log.info(e);
        }
        log.info("Successfully load the ontology schema into knowledge base.");
    }
    
    /*
     * Redirects to local directory for imports if the URL of the imported ontology is not found (the imports redirections should be included in the ont-policy.rdf file). 
    */
    private void manageImports() {
        File filePath = new File(System.getProperty("user.dir"), "conf");
        Model policyModel = ModelFactory.createDefaultModel();
        File policyFile = new File (filePath, "ont-policy.rdf");
        URL policyURL;
        try {
           policyURL = policyFile.toURI().toURL();
           policyModel.read(policyURL.toString());
        } catch (MalformedURLException ex) {
           log.error("System can't generate URL from it: "+ex);
        }
        schema.getDocumentManager().configure(policyModel);
    }
    
    /*private void addMappings(){
        
        Properties properties = new Properties() ;
        try {
            File filePath = new File(System.getProperty("user.dir"), "conf");
            File mappingsFile = new File (filePath, "mappings.properties");
            try {
                properties.load(new FileInputStream(mappingsFile.getAbsolutePath()));
            } catch (MalformedURLException ex) {
                log.error("System can't generate URL from it: "+ex);
            }
        } catch (IOException e) {
            log.error("System can't find mappings file: "+e);
        }
        Model mappedModel = ModelFactory.createDefaultModel();
        mappedModel.add(FileManager.get().loadModel(properties.getProperty("ontologyMappings")));
        mappedModel.add(FileManager.get().loadModel(properties.getProperty("targetOntology")));
        schema.add(mappedModel);
    }*/

    public boolean addStatementWithoutChecking(String statement) {
        String[] content = statement.split(" ");
        if (content.length != 3) {
            log.error("statement must have the form: \"subject predicate object\"");
            log.info("no statement added");
            return false;
        }
        addStatement(content[0], content[1], content[2]);
        return true;
    }

    public boolean addRules(String rule) {
        return false;
    }

    public boolean deleteKnowledge(String statement) {
        return false;
    }

    public boolean deleteRules(String rule) {
        return false;
    }

    public List<String> getRules() {
        return null;
    }

    public void renderKnowledge() {
        renderKB();
    }

    public boolean isTrue(String queryStatement) {
        return false;
    }


    public List querySingleItem(String sparqlRestrictions) {
        ResultSet rs = selectSPARQL(sparqlRestrictions);
        List resultList;
        if (rs == null) {
            log.info("No result found.");
            resultList = new ArrayList();
        } else {
            resultList = ResultSetFormatter.toList(rs);
        }
        return resultList;
    }
    
    public ResultSet queryXItem(String sparqlRestrictions,int nbItem)
    {
        ResultSet rs = selectSPARQL(sparqlRestrictions,nbItem);
        if (rs == null) {
            log.info("No result found.");
        }
        return rs;

    }
    
    public void queryContextItem(String sparqlRestrictions)
    {
        selectSPARQLContext(sparqlRestrictions);        
    }
    
    
    private ResultSet selectSPARQL(String whereRestrictionSPARQL,int nbItem)
    {
        String select = "";
        if(nbItem ==1){
            select = "DISTINCT ?x1";
        }else if(nbItem ==2){
            select = "DISTINCT ?x1 ?x2";
        }else if(nbItem ==3){
            select = "DISTINCT ?x1 ?x2 ?x3";
        }else if(nbItem ==4){
            select = "DISTINCT ?x1 ?x2 ?x3 ?x4";
        }else{
            select="*";
        }
        
        System.out.println("verif getXmlBase() : "+getXmlBase());
       
        String queryBeginn = "PREFIX rdf:  <http://www.w3.org/1999/02/22-rdf-syntax-ns#>\r\n"
                + "PREFIX ns: <" + getXmlBase() + "#>\r\n" 
                + "\r\n"
                + "PREFIX ns1: <http://www.xrce.xerox.com/parsem/ParsemGenericOntology.owl#>\r\n"
                + "\r\n"
                + "SELECT "+select+"\r\n"
                + "WHERE {\r\n";
        String queryEnd = "}";
     
        String queryStr = queryBeginn + whereRestrictionSPARQL + queryEnd;
        
        Query query = QueryFactory.create(queryStr);
        System.out.println("The reasoner is " +reasonerType);
        
        ResultSet results;
        try {
           
            if (reasonerType.equals("")){
                // query the database.
                log.info("I am in simple mode");
                QueryExecution qe = QueryExecutionFactory.create(query, ds);
                System.out.println(qe.toString());
                results = qe.execSelect();  
            }
            else {
                if (this.kb.getReasoner() instanceof PelletReasoner) {
                // query Pellet reasoner/model        
                    PelletQueryExecution qeR = new PelletQueryExecution(query, kb);
                    results = qeR.execSelect();          
                }
                else{
                //query using native reasoner
                    QueryExecution qeR = QueryExecutionFactory.create(query, kb);
                    results = qeR.execSelect();
                }
            }
        return results;
        }
        catch (Exception e) {
            log.error("query result exception by executing SPARQL query." + e);
            return null;
        }

    }

    
    

    private ResultSet selectSPARQL(String whereRestrictionSPARQL) {
        String queryBeginn = "PREFIX rdf:  <http://www.w3.org/1999/02/22-rdf-syntax-ns#>\r\n" + "PREFIX ns: <" + getXmlBase() + "#>\r\n" + "\r\n" + "SELECT ?x\r\n" + "WHERE {\r\n";
        String queryEnd = "}";
        String queryStr = queryBeginn + whereRestrictionSPARQL + queryEnd;
        Query query = QueryFactory.create(queryStr);
        ResultSet results;
        try {
            if (this.kb.getReasoner() instanceof PelletReasoner) {
                // query Pellet reasoner/model
                PelletQueryExecution qe = new PelletQueryExecution(query, kb);
                results = qe.execSelect();
            } else {
                // query defaul reasoner.
                QueryExecution qe = QueryExecutionFactory.create(query, kb);
                results = qe.execSelect();
            }
            // print the result out
            ResultSetFormatter.out(System.out, results, query);
            return results;
        } catch (Exception e) {
            log.error("query result exception by executing SPARQL query." + e);
            return null;
        }
    }
    
    
    private void selectSPARQLContext (String sparqlRestriction){
        String select = "DISTINCT ?x1";
                
        System.out.println("verif getXmlBase() : "+getXmlBase());
       
        String queryBeginn = "PREFIX rdf:  <http://www.w3.org/1999/02/22-rdf-syntax-ns#>\r\n"
                + "PREFIX ns: <" + getXmlBase() + "#>\r\n" 
                + "\r\n"
                + "PREFIX ns1: <http://www.xrce.xerox.com/parsem/ParsemGenericOntology.owl#>\r\n"
                + "\r\n"
                + "SELECT "+select+"\r\n"
                + "WHERE {\r\n";
        String queryEnd = "}";
     
        String queryStr = queryBeginn + sparqlRestriction + queryEnd;
        
        Query query = QueryFactory.create(queryStr);
        System.out.println("The reasoner is " +reasonerType);
        
        ResultSet results;
        try{
                Dataset dataset = DatasetStore.create(store) ;
                QueryExecution qe = QueryExecutionFactory.create(query, dataset);
                results = qe.execSelect();
                //data.removeAll();
                //System.out.println(results.toString());
                for (Iterator i = storeGraphNames(results).iterator(); i.hasNext();){  
                    String name = i.next().toString();
                    if (!name.equals("file:/InferenceModel")){
                        System.out.println(name);
                        data.begin();
                        data.add(SDBFactory.connectNamedModel(store,name));
                        data.commit();
                    }
                }
                ds = DatasetFactory.create(data);
                
                
        }
        catch (Exception e) {
            log.error("query result exception by executing SPARQL query." + e);
            //return null;
        }
        
        
    }

    /**
     * @return the schema
     */
    public OntModel getSchema() {
        return schema;
    }

    /**
     * @return the data
     */
    public Model getData() {
        return data;
    }

    /**
     * @return the reasoner
     */
    public Reasoner getReasoner() {
        return reasoner;
    }

    /**
     * @return the kb
     */
    public InfModel getKb() {
        return kb;
    }
    
        
    public Statement addStatement(String subjectId, String predicateId, String objectId) {
        if (subjectId == null || predicateId == null || objectId == null) {
            log.error("invalid arguments to addStatement: " + "subjectId: " + subjectId + " predicateId: " + predicateId + "objectId:" + objectId);
            return null;
        }
        // get the resource
        Resource subjectR;
        if (subjectId.contains("#")) {
            subjectR = kb.getResource(subjectId);
        } else {
            subjectR = kb.getResource(xmlBase + "#" + subjectId);
        }
        if (subjectR == null) {
            log.error("given subject: [" + subjectId + "] not found in model.");
            return null;
        }
        // get the property
        Property predicateP;
        if (predicateId.contains("#")) {
            predicateP = kb.getProperty(predicateId);
        } else {
            predicateP = kb.getProperty(xmlBase + "#" + predicateId);
        }
        if (predicateP == null) {
            log.error("given predicate: [" + predicateId + "] not found in model.");
            return null;
        }
        // get the object
        RDFNode objectR;
        if (predicateId.contains("#")) {
            objectR = kb.getResource(objectId);
        } else {
            objectR = kb.getResource(xmlBase + "#" + objectId);
        }
        if (objectR == null) {
            log.error("given object: [" + objectId + "] not found in model.");
            return null;
        }
        Statement stat = ResourceFactory.createStatement(subjectR, predicateP, objectR);
        kb.add(stat);
        return stat;
    }

    /**
     * 
     * @param subjectId
     * @param predicateId
     * @param objectId
     * @return
     */
    public ReifiedStatement addReifiedStatement(String subjectId, String predicateId, String objectId) {
        Statement stat = addStatement(subjectId, predicateId, objectId);
        if (stat == null) {
            log.error("can't add reified statement because no original statement can be generated.");
            return null;
        }
        List<String> args = new ArrayList<String>();
        args.add(subjectId);
        args.add(predicateId);
        args.add(objectId);
        String rid = hashID(args);
        ReifiedStatement rfs = kb.createReifiedStatement(rid, stat);
        return rfs;
    }

    private String hashID(List args) {
        // defaul lazy hashing, haha
        return args.toString();
    }

    /**
     * If we use database, we need to clean the database after we run the test.
     */
    public void clean() {
        if (this.conn != null) {
            try {
                // TODO: delete the model from the db
               // conn.cleanDB();
                store.getTableFormatter().truncate();
                // Close the database connection
                conn.close();
                log.info("data base cleaned.");
            } catch (Exception e) {
                log.error("Problem by closing DB connection: \n" + e);
            }
        } else {
        }
    }

    public boolean addStatementWithChecking(String statement) {
        return false;
    }
    
    public static void displayResultSet(ResultSet rs,String title){
        ResultsViewer viewer = new ResultsViewer(title);        
        viewer.setVisible(true);
        viewer.display(rs);       
    }
    
    
    
    public static Vector <String> storeGraphNames(ResultSet rs){
        ResultsViewer viewer = new ResultsViewer("");
        Vector <Vector> results = new Vector <Vector> (viewer.getData(rs));
        Vector <String> graphNames = new Vector <String>();
        for (Iterator it = results.iterator (); it.hasNext (); ) {
            Vector o = (Vector)it.next ();
            for (Iterator i = o.iterator (); i.hasNext (); ) {
                graphNames.add((String)i.next());
            }
        }
        return graphNames;
    }
}
