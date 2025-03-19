/*
 * DataBaseConnection.java
 *
 * Created on February 15, 2007, 3:47 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package common.database;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;
import java.sql.Statement;

/**
 * This class handles database basic treatment
 * @author xtannier
 */
public class DataBaseConnection {
    
    Connection connection;
    

    
    /**
     * Creates a new instance of DataBaseConnection
     */
    public DataBaseConnection(String pilote, String hostname, String database, 
            String login, String password) {
        
        Statement stmt;
//        String pilote = "com.mysql.jdbc.Driver";
        String url = "jdbc:mysql://" + hostname + "/" + database;
        String commande = "";
//        
        try {
            Class.forName(pilote);
            this.connection = DriverManager.getConnection(url, login, password);        
        } catch (SQLException e) {
            e.printStackTrace();
            System.err.println("SQLException: " + e.getMessage());
            System.err.println("SQLState: " + e.getSQLState());
            System.err.println("VendorError: " + e.getErrorCode());
            System.exit(-1);
        } catch (ClassNotFoundException e) {
            e.printStackTrace();
            System.exit(-1);
        }       
    }
    
    public void finalize(){
        try {
            this.connection.close();
        } catch (SQLException e) {
            e.printStackTrace();
            System.err.println("SQLException: " + e.getMessage());
            System.err.println("SQLState: " + e.getSQLState());
            System.err.println("VendorError: " + e.getErrorCode());
            System.exit(-1);
        }
    }
    
    public int executeUpdate(String command){
        try{             
            Statement statement = this.connection.createStatement();
            int result = statement.executeUpdate(command);
            statement.close();
            return result;
        } catch (SQLException e) {
            e.printStackTrace();
            System.err.println("SQLException: " + e.getMessage());
            System.err.println("SQLState: " + e.getSQLState());
            System.err.println("VendorError: " + e.getErrorCode());
            System.exit(-1);
            return -1;
        }
}
    
}
