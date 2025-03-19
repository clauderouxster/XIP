/*
 * KBManager.java
 *
 * Created on August 17, 2007, 2:29 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package knowledgeBases;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import org.mindswap.pellet.jena.PelletQueryExecution;
import org.mindswap.pellet.jena.PelletReasoner;
import org.mindswap.pellet.jena.PelletReasonerFactory;

import com.hp.hpl.jena.db.DBConnection;
import com.hp.hpl.jena.db.IDBConnection;
import com.hp.hpl.jena.ontology.OntDocumentManager;
import com.hp.hpl.jena.ontology.OntModel;
import com.hp.hpl.jena.ontology.OntModelSpec;
import com.hp.hpl.jena.query.Query;
import com.hp.hpl.jena.query.QueryExecution;
import com.hp.hpl.jena.query.QueryExecutionFactory;
import com.hp.hpl.jena.query.QueryFactory;
import com.hp.hpl.jena.query.ResultSet;
import com.hp.hpl.jena.query.ResultSetFormatter;
import com.hp.hpl.jena.rdf.model.InfModel;
import com.hp.hpl.jena.rdf.model.Model;
import com.hp.hpl.jena.rdf.model.ModelFactory;
import com.hp.hpl.jena.rdf.model.ModelMaker;
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

import tests.ResultsViewer;

/**
 * The KBManager for Jena KB. This is the major implementation part of jena
 * framework. This manager extends the KBManager for specific configuration, 
 * namely, it uses Jena Framework as the foundation.
 * 
 * 
 * @author tlu
 * @version 1.0
 */
public class JenaKBManager extends KBManager {

    //protected static Category log = Category.getInstance(JenaKBManager.class.getName());
    static Logger log = Logger.getLogger(JenaKBManager.class);
    OntModel schema;
    Model data;
    Reasoner reasoner;
    InfModel kb;
    OntModelSpec s;
    IDBConnection conn;
    public final static String JENA_OWL_KAON2 = "jena-owl-kaon2";
    /**
     * Use Jena framework and pellet model, bind pellet reasoner and persistent
     * DB
     */
    public final static String JENA_PELLET_PELLET_DB = "jena-pellet-pellet-db";
    /**
     * Use Jena framework and its OWL model, bind Pellet reasoner and persistent
     * DB.
     */
    public final static String JENA_OWL_PELLET_DB = "jena-owl-pellet-db";
    /**
     * Use Jena framework and pellet model, bind pellet reasoner and persistent
     * DB
     */
    public final static String JENA_PELLET_PELLET_MEM = "jena-pellet-pellet-mem";

    /** Creates a new instance of KBManager
     * @param model
     * @param reasoner
     * @param database 
     */
    public JenaKBManager(String model, String reasoner, String database) {
        // default settings
        OntDocumentManager mgr = new OntDocumentManager();
        s = new OntModelSpec(OntModelSpec.OWL_MEM_RULE_INF);
        // use the jena built in OWL inference rules engine
        s.setDocumentManager(mgr);
        // specific setting
        if (model.equals("") || reasoner.equals("") || database.equals("")) {
            this.schema = ModelFactory.createOntologyModel(s);
            this.data = ModelFactory.createDefaultModel();
        } else if (model.equals("jena-pellet") || reasoner.equals("pellet")) {
            log.info("Using Pellet to build the model");
            log.info("---------------------");
            log.info("");
            // create an empty ontology model using Pellet spec
            this.schema = ModelFactory.createOntologyModel(PelletReasonerFactory.THE_SPEC);
            if (database.equals("db")) {
                bindDB("jena-owl-pellet-test");
            } else {
                // TODO: do nothing to use the default: memory version
            }
        } else if (model.equals("jena-owl") || reasoner.equals("kaon")) {
            log.info("Using KAON2 to build the model");
            log.info("---------------------");
            log.info("");
        }
    }

    public void bindDB(String modelName) {
        try {
            // load the the driver class
            Class.forName(M_DBDRIVER_CLASS);
            // create a database connection
            conn = new DBConnection(M_DB_URL, M_DB_USER, M_DB_PASSWD, M_DB);
            // create a model maker with the given connection parameters
            ModelMaker maker = ModelFactory.createModelRDBMaker(conn);
            // create a default model
            // Model defModel = maker.createDefaultModel();
            // ...
            // Open existing default model
            // Model defModel = maker.openModel();
            // or create a named model
            this.data = maker.createModel(modelName);
            // ...
            // or open a previously created named model
            // Model prvModel = maker.openModel("AnExistingModel");
            log.info("database bound: [" + M_DB_URL + "] [" + M_DB_USER + "] [" + M_DB_PASSWD + "] [" + M_DB + "].");
        } catch (Exception e) {
            log.error("Exception catched by binding the DB. \n" + e);
        }
    }

    private void bindInternalReasoner() {
        // bind default reasoner of Jena
        reasoner = ReasonerRegistry.getRDFSReasoner();
        // reasoner = ReasonerRegistry.getOWLReasoner();
        reasoner = reasoner.bindSchema(schema);
    }

    private void bindExternalReasoner(String url) {
        if (url.equalsIgnoreCase("pellet")) {
            // create Pellet reasoner using the facility of Pellet
            // "Jena-built-in-model" reasoner
            reasoner = PelletReasonerFactory.theInstance().create();
            reasoner = reasoner.bindSchema(schema);
        } else {
            bindDIGCompatibleReasoner(url);
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

    private void bindRulesReasoner(String url) {
        List rules = Rule.rulesFromURL(url);
        reasoner = new GenericRuleReasoner(rules);
    //reasoner.setOWLTranslation(true);  // not needed in RDFS case
    //reasoner.setTransitiveClosureCaching(true);
    }

    public boolean buildKB(String ontologyURL, String schemaURL, List<String> dataLocations, String reasonerURL) {
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
        // bind reasoner
        bindReasoner(reasonerURL);
        kb = ModelFactory.createInfModel(reasoner, data);
        return true;
    }

    public void printStatements(Model m, Resource s, Property p, Resource o) {
        for (StmtIterator i = m.listStatements(s, p, o); i.hasNext();) {
            Statement stmt = i.nextStatement();
            log.info(" - " + PrintUtil.print(stmt));
        }
    }

    private void renderKB() {
        for (StmtIterator i = kb.listStatements(); i.hasNext();) {
            Statement stmt = i.nextStatement();
            log.info(" - " + PrintUtil.print(stmt));
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
            // model.read(new
            // FileInputStream(kbCaller.getOntologyFileName()),"file:"+kbCaller.getOntologyFileName());
            // reading the ontology from a URL:
            schema.read(location);
        /*
         * } catch (FileNotFoundException e) { throw new
         * OntologyException("Creation failed! File
         * '"+kbCaller.getOntologyFileName()+"' not found.");
         */
        } catch (WrappedIOException e) {
            log.info("Something wrong by loading the ontology: " + location);
            log.info(e);
        }
        log.info("Successfully load the ontology schema into knowledge base.");
    }

    public boolean addStatementWithoutChecking(String statement) {
        String[] content = statement.split(" ");
        if (content.length != 3) {
            log.error("statement must be have the form: \"subject predicate object\"");
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

    // public static void printQueryResults(String header, QueryExecution qe,
    // Query query) {
    // log.info(header);
    // try {
    // ResultSet results = qe.execSelect();
    // ResultSetFormatter.out(System.out, results, query);
    // } catch (Exception e) {
    // log.error("Exceptoin caught by execution of query: \n" + e);
    // }
    // }
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
    
    
    
    private ResultSet selectSPARQL(String whereRestrictionSPARQL,int nbItem)
    {
        String select = "";
        if(nbItem ==1){
            select = "?x1";
        }else if(nbItem ==2){
            select = "?x1 ?x2";
        }else if(nbItem ==3){
            select = "?x1 ?x2 ?x3";
        }
        //System.out.println("verif getXmlBase() : "+getXmlBase());
        String queryBeginn = "PREFIX rdf:  <http://www.w3.org/1999/02/22-rdf-syntax-ns#>\r\n"
                + "PREFIX ns: <" + getXmlBase() + "#>\r\n" + "\r\n" + "SELECT "+select+"\r\n"
                + "WHERE {\r\n";
        String queryEnd = "}";

        String queryStr = queryBeginn + whereRestrictionSPARQL + queryEnd;
        Query query = QueryFactory.create(queryStr);

        ResultSet results;
        try {

            if (this.kb.getReasoner() instanceof PelletReasoner) {
                // query Pellet reasoner/model
                PelletQueryExecution qe = new PelletQueryExecution(query, kb);
                results = qe.execSelect();
            }
            else {
                // query default reasoner.
                QueryExecution qe = QueryExecutionFactory.create(query, kb);
                System.out.println(qe.toString());
                results = qe.execSelect();
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

    public void bindReasoner(String location) {
        // bindInternalReasoner();
        // bindRulesReasoner(reasonerURL);
        bindExternalReasoner(location);
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
                conn.cleanDB();
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
        viewer.show();
        viewer.display(rs);
    }
}
