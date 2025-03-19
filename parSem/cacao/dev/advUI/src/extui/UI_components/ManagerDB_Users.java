/*
 * Xerox Research Center Europe
 * Copyright (C) 2009 XRCE, All Rights Reserved
 */
package extui.UI_components;

import java.io.File;
import java.io.Serializable;
import java.util.HashMap;
import java.util.Properties;
import javax.persistence.EntityManager;
import javax.persistence.EntityManagerFactory;
import javax.persistence.Persistence;
import javax.persistence.Query;
import oracle.toplink.essentials.config.TopLinkProperties;
import oracle.toplink.essentials.ejb.cmp3.EntityManagerFactoryProvider;
import org.apache.log4j.Logger;

/**
 *
 * @author ctrojahn
 */
public class ManagerDB_Users implements Serializable {

    private static final Logger logger = Logger.getLogger(ManagerDB_Users.class.getName());
    private static final String persitenceUnitName = "default";
    EntityManagerFactory emf;

    public ManagerDB_Users() {
        logger.trace("new ManagerDB Users");
    }

    public UserProfile addBookmark(UserProfile user) {
        EntityManager em = emf.createEntityManager();
        try {
            em.getTransaction().begin();
            UserProfile userNew = em.find(UserProfile.class, user.getID());
            userNew.setBookmarks(user.getBookmarks());
            em.getTransaction().commit();
            return userNew;
        } finally {
            em.close();
            logger.error("ERRO!");
        }

    }

    public UserProfile setNewSettings(UserProfile user) {
        EntityManager em = emf.createEntityManager();
        try {
            em.getTransaction().begin();
            logger.info("USER:  " + user.getID());
            UserProfile userNew = em.find(UserProfile.class, user.getID());
            userNew = user;
            em.getTransaction().commit();
            return userNew;
        } catch (Exception e) {
            em.close();
            logger.error("ERRO! " + e);
        }
        return null;
    }

    private String getRootTmp() {
        String rootTmp;
        rootTmp = System.getProperty("java.io.tmpdir") + File.separator + "advUI";
        return rootTmp;
    }

    /**
     *
     * @param properties
     * @param dropAndCreateTables
     */
    public void newDB(Properties properties, boolean dropAndCreateTables) {
        long time = System.currentTimeMillis();
        HashMap<String, String> propertiesMap;
        propertiesMap = new HashMap<String, String>();
        String default_jdbc_url = "";
        String default_jdbc_driver = "";
        String default_target_database = "";
        String default_jdbc_user = "";
        String default_jdbc_password = "";
        String jdbc_url, jdbc_driver, target_database, jdbc_user, jdbc_password, logging_level;
        String dbPath = "";
        dbPath = getRootTmp(); // + File.separator + "advUI"; ???
        File tempDir = new File(dbPath);

        //TODO:  Just pour le test = when user are registered, do not remove
        if (tempDir.exists()) {
            tempDir.delete();
        }
        tempDir.mkdirs();
        logger.info("creating one common db dir (dbpath) - USERS: " + dbPath);
        boolean embedded = true;
        if (embedded) {
            // ok with ;MVCC=TRUE
            //String parameters = "";
            //parameters += ";LOCK_TIMEOUT=2000";
            //parameters += ";CACHE_SIZE=131072";
            //parameters += ";LOCK_MODE=3";
            default_jdbc_url = "jdbc:h2:file:" + dbPath + "/" + Constant.dbName;
        } else {
            //String parameters = "";
            //parameters += ";LOCK_TIMEOUT=2000";
            //parameters += ";CACHE_SIZE=131072";
            //parameters += ";MULTI_THREADED=1"
            default_jdbc_url = "jdbc:h2:tcp://localhost" + "/" + Constant.dbName;
        }
        logger.info(default_jdbc_url);
        default_jdbc_driver = "org.h2.Driver";
        default_target_database = "oracle.toplink.essentials.platform.database.H2Platform";
        default_jdbc_user = "sa";
        default_jdbc_password = "";
        // get properies values defined in classificationExplorer.properties or use default value
        jdbc_url = properties.getProperty("toplink.jdbc.url", default_jdbc_url);
        jdbc_driver = properties.getProperty("toplink.jdbc.driver", default_jdbc_driver);
        target_database = properties.getProperty("toplink.target-database", default_target_database);
        jdbc_user = properties.getProperty("toplink.jdbc.user", default_jdbc_user);
        jdbc_password = properties.getProperty("toplink.jdbc.password", default_jdbc_password);
        //logging_level = properties.getProperty("toplink.logging.level", "FINE");
        //logging_level = properties.getProperty("toplink.logging.level", "SEVERE");
        //init TopLink
        propertiesMap.put(TopLinkProperties.JDBC_URL, jdbc_url);
        propertiesMap.put(TopLinkProperties.JDBC_DRIVER, jdbc_driver);
        propertiesMap.put(TopLinkProperties.TARGET_DATABASE, target_database);
        propertiesMap.put(TopLinkProperties.JDBC_USER, jdbc_user);
        propertiesMap.put(TopLinkProperties.JDBC_PASSWORD, jdbc_password);
        //propertiesMap.put(TopLinkProperties.LOGGING_LEVEL, logging_level);
        logger.info("using the following jdbc.url: " + propertiesMap.get(TopLinkProperties.JDBC_URL));
        logger.info("using the following jdbc.driver: " + propertiesMap.get(TopLinkProperties.JDBC_DRIVER));
        //logger.info("using the following target-database: " + propertiesMap.get(TopLinkProperties.TARGET_DATABASE));
        //logger.info("using the following logging.level: " + propertiesMap.get(TopLinkProperties.LOGGING_LEVEL));
        // manage session
        //propertiesMap.put(TopLinkProperties.SESSION_NAME, "advUI" + sessionId); pas compris a quoi ca sert
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
            logger.info("DB tables will be created (if they do not exist)");
            // specify where createDDl.jdbc whill be created
            propertiesMap.put(EntityManagerFactoryProvider.APP_LOCATION, dbPath);
            // generate createDDl.jdbc for non-existent tables; leave existing tables unchanged
            propertiesMap.put(EntityManagerFactoryProvider.DDL_GENERATION, EntityManagerFactoryProvider.CREATE_ONLY);
            // generate createDDl.jdbc files and execute it on the database if DB tables don't exist
            propertiesMap.put(EntityManagerFactoryProvider.DDL_GENERATION_MODE, EntityManagerFactoryProvider.DDL_BOTH_GENERATION);
        }
        try {
            emf = Persistence.createEntityManagerFactory(persitenceUnitName, propertiesMap);
            logger.info("created emf manager USERS = " + emf);
            // just to create (and drop) the table if necessary
            logger.info("Create and Drop DB using emf = " + emf);
            EntityManager em = emf.createEntityManager();
            em.close();
        } catch (Exception e) {
            logger.info("Create and Drop DB USERS Exception: " + e);
        }
        time = System.currentTimeMillis() - time;
        logger.info("Creation of the DB USERS in: " + time + " ms");
    }

    public synchronized void persistUserProfile(UserProfile user) {
        EntityManager em = emf.createEntityManager();
        em.getTransaction().begin();
        try {
            em.persist(user);
            em.getTransaction().commit();
            em.close();
        } catch (Exception e) {
            logger.error("ERROR PERSIST USER PROFILE: " + e);
        }
    }

    public synchronized UserProfile getUserProfile(String username, String password) {
        EntityManager em = emf.createEntityManager();
        Query query = null;
        try {
            query = em.createNamedQuery("getUserProfile");
            query.setParameter("login", username);
            query.setParameter("password", password);
            UserProfile user = (UserProfile) query.getSingleResult();
            em.close();
            return user;
        } catch (Exception e) {
            logger.error("ERROR getUserProfile: " + e);
            return null;
        }
    }
}
