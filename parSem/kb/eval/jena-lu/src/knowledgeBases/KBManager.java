package knowledgeBases;

import interfaces.InputAPI;
import interfaces.KBManagement;
import interfaces.QueryAPI;

/**
 * A KBmanager implement the inputAPI, KBMangement and QueryAPI, which provide a
 * full support to a specific framework setting. Each specific framework setting
 * is one-one matched to each kind of KBManager. All kinds of KBManager should
 * extends this abstract class. This structure is aimed to give more flexibility
 * and control of choosing different combination of handling knowledge base
 * setting, in order to test different tools and combine the best ones for our
 * particular use case.
 * 
 * @author tlu
 * @version 1.0
 */
public abstract class KBManager implements InputAPI, KBManagement, QueryAPI {

    // database URL
    public final static String M_DB_URL = "jdbc:mysql://etretat:3306/test_gj";
    // User name
    public final static String M_DB_USER = "root";
    // Password
    public final static String M_DB_PASSWD = "root";
    // Database engine name
    public final static String M_DB = "MySQL";
    // JDBC driver
    public final static String M_DBDRIVER_CLASS = "com.mysql.jdbc.Driver";
    protected String xmlBase;

    public String getXmlBase() {
        return xmlBase;
    }

    public void setXmlBase(String xmlBase) {
        this.xmlBase = xmlBase;
    }
}
