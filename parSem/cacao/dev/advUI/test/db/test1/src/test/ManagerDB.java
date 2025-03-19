package test;

import java.io.File;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Properties;
import javax.persistence.*;

import oracle.toplink.essentials.config.TopLinkProperties;
import oracle.toplink.essentials.ejb.cmp3.EntityManagerFactoryProvider;
import org.apache.log4j.Logger;

/**
 *
 * @author ctrojahn
 */
public class ManagerDB {

    private EntityManagerFactory emf;
    //private EntityManager em;
    private HashMap<String, String> propertiesMap;
    private static final Logger logger = Logger.getLogger(ManagerDB.class.getName());

    /**
     *
     * @param properties
     * @param dropAndCreateTables
     */
    public ManagerDB(Properties properties, boolean dropAndCreateTables) {

        //System.setProperty("h2.optimizeInJoin", "true");
        long time = System.currentTimeMillis();
        propertiesMap = new HashMap<String, String>();
        String default_jdbc_url = "";
        String default_jdbc_driver = "";
        String default_target_database = "";
        String default_jdbc_user = "";
        String default_jdbc_password = "";
        String jdbc_url, jdbc_driver, target_database, jdbc_user, jdbc_password, logging_level;
        boolean h2 = true;
        boolean embedded = true;

       // String dbPath = "/cacao/h2/h2DB";
        String dbPath = "db";
        //String dbPath = "/opt/ParSem/users/cassiat/parSem/cacao/dev/advUI/db";
        File file = new File(dbPath);
        if (!file.isDirectory()) {
            file.mkdir();
        }
        if (h2) {
            String params = "";
//            params += ";LOCK_MODE=3";
//            params += ";CACHE_SIZE=131072";
//            params += ";MULTI_THREADED=1";
//            params += ";LOCK_MODE=3";
//            params += ";LOCK_TIMEOUT=2000";
            if (embedded) { // ok with ;MVCC=TRUE
                default_jdbc_url = "jdbc:h2:file:" + dbPath + "/" + Constant.dbName + params;
            } else {
                default_jdbc_url = "jdbc:h2:tcp://localhost/" + Constant.dbName + params;
            }
            logger.info(default_jdbc_url);
            default_jdbc_driver = "org.h2.Driver";
            default_target_database = "oracle.toplink.essentials.platform.database.H2Platform";
            default_jdbc_user = "sa";
            default_jdbc_password = "";
        } else {
            // mysql
            default_jdbc_url = "jdbc:mysql://salses:3306/" + Constant.dbName;
            default_jdbc_driver = "com.mysql.jdbc.Driver";
            default_target_database = "Auto";
            default_jdbc_user = "root";
            default_jdbc_password = "root";
        }
        // get properies values defined in classificationExplorer.properties or use default value
        jdbc_url = properties.getProperty("toplink.jdbc.url", default_jdbc_url);
        jdbc_driver = properties.getProperty("toplink.jdbc.driver", default_jdbc_driver);
        target_database = properties.getProperty("toplink.target-database", default_target_database);
        jdbc_user = properties.getProperty("toplink.jdbc.user", default_jdbc_user);
        jdbc_password = properties.getProperty("toplink.jdbc.password", default_jdbc_password);
        //logging_level = properties.getProperty("toplink.logging.level", "FINE");
        logging_level = properties.getProperty("toplink.logging.level", "SEVERE");
        //init TopLink
        propertiesMap.put(TopLinkProperties.JDBC_URL, jdbc_url);
        propertiesMap.put(TopLinkProperties.JDBC_DRIVER, jdbc_driver);
        propertiesMap.put(TopLinkProperties.TARGET_DATABASE, target_database);
        propertiesMap.put(TopLinkProperties.JDBC_USER, jdbc_user);
        propertiesMap.put(TopLinkProperties.JDBC_PASSWORD, jdbc_password);
        propertiesMap.put(TopLinkProperties.LOGGING_LEVEL, logging_level);
        logger.info("using the following jdbc.url: " + propertiesMap.get(TopLinkProperties.JDBC_URL));
        logger.info("using the following jdbc.driver: " + propertiesMap.get(TopLinkProperties.JDBC_DRIVER));
        logger.info("using the following target-database: " + propertiesMap.get(TopLinkProperties.TARGET_DATABASE));
        logger.info("using the following logging.level: " + propertiesMap.get(TopLinkProperties.LOGGING_LEVEL));
        if (dropAndCreateTables) {
            logger.info("DB tables will be dropped and created");
            //http://www.oracle.com/technology/products/ias/toplink/JPA/essentials/toplink-jpa-extensions.html
            // specify where dropDDL.jdbc and createDDl.jdbc whill be created
            propertiesMap.put(EntityManagerFactoryProvider.APP_LOCATION, dbPath);
            // ask to generate dropDDL.jdbc and createDDl.jdbc
            propertiesMap.put(EntityManagerFactoryProvider.DDL_GENERATION, EntityManagerFactoryProvider.DROP_AND_CREATE);
            // generate dropDDL.jdbc and createDDl.jdbc files and execute them on the database
            propertiesMap.put(EntityManagerFactoryProvider.DDL_GENERATION_MODE, EntityManagerFactoryProvider.DDL_BOTH_GENERATION);
        } else {
            logger.info("DB tables will be created if they do not exist");
            // specify where createDDl.jdbc whill be created
            propertiesMap.put(EntityManagerFactoryProvider.APP_LOCATION, dbPath);
            // generate createDDl.jdbc for non-existent tables; leave existing tables unchanged
            propertiesMap.put(EntityManagerFactoryProvider.DDL_GENERATION, EntityManagerFactoryProvider.CREATE_ONLY);
            // generate createDDl.jdbc files and execute it on the database if DB tables don't exist
            propertiesMap.put(EntityManagerFactoryProvider.DDL_GENERATION_MODE, EntityManagerFactoryProvider.DDL_BOTH_GENERATION);
        }
        try {
            String persitenceUnitName = "default";
            emf = Persistence.createEntityManagerFactory(persitenceUnitName, propertiesMap);
            // just to create (and drop) the table if necessary
            EntityManager em;
            em = emf.createEntityManager();
            em.close();
        } catch (Exception e) {
            System.out.println(e);
        }
        time = System.currentTimeMillis() - time;
        System.out.println("Creation of the DB: " + time);
    }

    public void cleanDB() {
        logger.info("cleanDB()");
        EntityManager em = emf.createEntityManager();
        em.getTransaction().begin();
        String tableName = Constant.docTableName;
        try {
            em.createNativeQuery("delete from " + tableName).executeUpdate();
        } catch (Exception ex) {
            logger.warn(ex.getCause());
        }
        em.getTransaction().commit();
        em.close();
    }

    public void persistFacet(Facet facet) {
        EntityManager em = emf.createEntityManager();
        em.getTransaction().begin();
        em.persist(facet);
        em.getTransaction().commit();
        em.close();
    }

    public void persistFacetValue(FacetValue facetValue) {
        EntityManager em = emf.createEntityManager();
        em.getTransaction().begin();
        em.persist(facetValue);
        em.getTransaction().commit();
        em.close();
    }

    public void persistDoc(Document doc) {
        EntityManager em = emf.createEntityManager();
        em.getTransaction().begin();
        em.persist(doc);
        em.getTransaction().commit();
        em.close();
    }

    /**
     * NamedQuery: class Document
     * @return
     */
    public int getNumberDocuments(EntityManager em) {
        //long time = System.currentTimeMillis();
        logger.debug("getNumberDocuments");
        //createEntityManager();
        //EntityManager em = emf.createEntityManager();
        Query query = em.createNamedQuery("getNumberDocuments");
        String r = query.getSingleResult().toString();
        //closeEntityManager();
        //em.close();
        //time = System.currentTimeMillis() - time;
        //System.out.println("getNumberDocuments: " + time);
        return Integer.parseInt(r);

    }

    /**
     * NamedQuery; class Document
     * @param id
     * @param em
     * @return
     */
    public Document getDocument(long id, EntityManager em) {
        //long time = System.currentTimeMillis();
        logger.info("getDocument " + id);
        //createEntityManager();
        //EntityManager em = emf.createEntityManager();
        Query query = em.createNamedQuery("getDocument");
        query.setParameter("id", id);
        Document doc = (Document) query.getSingleResult();
        //closeEntityManager();
        //em.close();
        //time = System.currentTimeMillis() - time;
        //System.out.println("getDocuments: " + time);
        return doc;
    }

    public String getFacetValueDOC(long documentID, String v, EntityManager em) {
        //long time = System.currentTimeMillis();
        //createEntityManager();
        //EntityManager em = emf.createEntityManager();
        Query query = em.createNamedQuery("getFacetValueDOC");
        query.setParameter("v", v);
        query.setParameter("id", documentID);
        String value = (String) query.getSingleResult();
        //closeEntityManager();
        //em.close();
        //time = System.currentTimeMillis() - time;
        //System.out.println("getFacetValueDOC: " + time);
        return value;
    }

    /*
     * NamedQuery: class Facet
     */
    public Facet getFacet(String name, EntityManager em) {

        try {
            //  long time = System.currentTimeMillis();
            //createEntityManager();
            //EntityManager em = emf.createEntityManager();
            Query query = em.createNamedQuery("getFacet");
            query.setParameter("v", name);
            Facet f = null;
            if (query.getResultList().size() > 0) {
                f = (Facet) query.getSingleResult();
            }
            //closeEntityManager();
            //em.close();
            //time = System.currentTimeMillis() - time;
            //System.out.println("getFacet: " + time);
            return f;
        } catch (Exception e) {
            System.out.println("ERRO getFacet: " + e);
        }
        return null;
    }


    /*
     * NamedQuery: class Facet
     */
    public List getFacets(EntityManager em) {
        //EntityManager em = emf.createEntityManager();
        try {
            //long time = System.currentTimeMillis();
            //createEntityManager();
            Query query = em.createNamedQuery("getFacets");
            List l = query.getResultList();
            //closeEntityManager();
            //em.close();
            //time = System.currentTimeMillis() - time;
            //System.out.println("getFacets: " + time);
            return l;
        } catch (Exception e) {
            System.out.println("ERRO getFacets: " + e);
        }
        return null;
    }

    /*
     * NamedQuery: class Facet
     */
    public List getValues(String facetName, EntityManager em) {
        //EntityManager em = emf.createEntityManager();
        try {
            //long time = System.currentTimeMillis();
            //createEntityManager();
            Query query = em.createNamedQuery("getValues");
            query.setParameter("v", facetName);
            List l = query.getResultList();
            //closeEntityManager();
            //em.close();
            // time = System.currentTimeMillis() - time;
            // System.out.println("getValues: " + time);
            return l;
        } catch (Exception e) {
            System.out.println("ERRO getValues: " + e);
        }
        return null;
    }

    /*
     * NamedQuery: class FacetValue
     */
    public FacetValue getFacetValue(String f, String v, EntityManager em) {
        //EntityManager em = emf.createEntityManager();
        try {
            // long time = System.currentTimeMillis();
            //createEntityManager();
            Query query = em.createNamedQuery("getFacetValue");
            query.setParameter("f", f);
            query.setParameter("v", v);
            FacetValue fv = (FacetValue) query.getSingleResult();
            // closeEntityManager();
            //em.close();
            // time = System.currentTimeMillis() - time;
            // System.out.println("getFacetValue: " + time);
            return fv;
        } catch (Exception e) {
            System.out.println("ERRO getFacetValue: " + e);
        }
        return null;
    }

    /*
     *
     */
    public List<FacetValue> getListFacetValues(String f, HashSet hash, EntityManager em) {
        //EntityManager em = emf.createEntityManager();
        //long time = System.currentTimeMillis();
        try {
            //createEntityManager();
            Iterator itr = hash.iterator();
            int p = 0;
            String params = "";
            while (itr.hasNext()) {
                params = params + ":v" + p;
                itr.next();
                if (itr.hasNext()) {
                    params = params + ",";
                }
                p++;
            }
            String queryStr = "SELECT fv FROM FacetValue fv JOIN fv.facet f where fv.value IN (" + params + ") AND f.nameSolr = :f";
            Query query = em.createQuery(queryStr);
            itr = hash.iterator();
            p = 0;
            while (itr.hasNext()) {
                query.setParameter("v" + p, itr.next());
                p++;
            }
            query.setParameter("f", f);
            @SuppressWarnings("unchecked")
            List<FacetValue> fvs = query.getResultList();
            //closeEntityManager();
            //em.close();
            //  time = System.currentTimeMillis() - time;
            //  System.out.println("getListFacetValues: " + time);
            return fvs;
        } catch (Exception e) {
            System.out.println("ERRO getListFacetValue:" + e);
        }
        return null;
    }

    /**
     *
     * @param first
     * @param max
     * @return
     * NamedQuery: class Document
     */
    public List getListDocs(int first, int max, EntityManager em) {
        //EntityManager em = emf.createEntityManager();
        //long time = System.currentTimeMillis();
        //createEntityManager();
        Query query = em.createNamedQuery("getListDocs");
        query.setFirstResult(first);
        query.setMaxResults(max);
        List list = query.getResultList();
        //closeEntityManager();
        //em.close();
        //time = System.currentTimeMillis() - time;
        //System.out.println("getListDocs: " + time);
        return list;
    }

    public EntityManagerFactory getEntityManagerFactory() {
        return emf;
    }
//    private  void createTransactionalEntityManager() {
//        logger.info("createTransactionalEntityManager()");
//        em = emf.createEntityManager();
//        em.getTransaction().begin();
//    }
//
//    private  void closeTransactionalEntityManager() {
//        logger.info("closeTransactionalEntityManager()");
//        em.getTransaction().commit();
//        em.close();
//    }
//
//    private  void createEntityManager() {
//        logger.info("createEntityManager()");
//        em = emf.createEntityManager();
//    }
//
//    private  void closeEntityManager() {
//        logger.info("closeEntityManager()");
//        em.close();
//    }
}