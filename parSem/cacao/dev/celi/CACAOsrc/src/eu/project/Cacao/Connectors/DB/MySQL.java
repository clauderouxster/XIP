/**
 *
 */
package eu.project.Cacao.Connectors.DB;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.DataOutputStream;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.Vector;

import eu.project.Cacao.Connectors.Metadata.DublinCoreRecord;
import eu.project.Cacao.Connectors.Metadata.TelAPRecord;
import eu.project.Cacao.Harvester.OAIHarvestingAgent;
import eu.project.Cacao.Interfaces.CacaoDB;

import org.apache.log4j.Level;
import org.apache.log4j.Logger;

/**
 * 
 * Class for MySQL DB backend
 * 
 * @author Igor Barsanti - <a href="http://www.gonetwork.it"
 *         target="_blank">Gonetwork Srl</a>
 * 
 */
public class MySQL implements CacaoDB {
	private static MySQL instance = null;

	private Logger Log = Logger.getLogger(OAIHarvestingAgent.class);

	private String username;
	private String password;
	private String server;
	private String db;
	private String valuesSeparator = "<";
	private String hashSeparator = ">";
	private Connection conn;

	/**
	 * Class constructor<br>
	 * The user must set DB connection parameters with<br>
	 * setUsername(), setPassword(), setServer(), set DBname() methods, <br>
	 * then calling connect().
	 * 
	 */
	public MySQL() {
	}

	/**
	 * Class constructor<br>
	 * Initialize DB connection parameters and start a connection <br>
	 * (user don't need to call connect() method)
	 * 
	 * @param username
	 * @param password
	 * @param server
	 * @param database
	 */
	public MySQL(String username, String password, String server, String database) {
		super();
		this.username = username;
		this.password = password;
		this.server = server;
		this.db = database;
		connect();
	}

	/**
	 * method for get a singleton instance
	 * 
	 * @return the singleton instance
	 */
	public static MySQL getInstance() {
		if (instance == null) {
			instance = new MySQL();
		}

		return instance;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * eu.project.Cacao.Interfaces.DB#addCollection(eu.project.Cacao.Connectors
	 * .DB.Collection)
	 */
	public boolean addCollection(LibraryCollection collection) {
		try {
			if (conn.isClosed())
				this.connect();

			if (collection.getLastUpdate() == null) {
				collection.setLastUpdate(now());
			}

			PreparedStatement stmt = conn.prepareStatement("INSERT INTO Collections(library_id, description, code, lastupdate) VALUES(?,?,?,?);");
			stmt.setInt(1, collection.getLibrary().getIDasInteger());
			stmt.setString(2, collection.getDescription());
			stmt.setString(3, collection.getCode());
			if (collection.getLastUpdate() == null || collection.getLastUpdate().length() == 0)
				stmt.setString(4, null);
			else
				stmt.setString(4, collection.getLastUpdate());
			stmt.execute();

			ResultSet resID = stmt.getGeneratedKeys();
			resID.next();
			collection.setID(resID.getString(1));

			stmt.close();

			return true;
		} catch (SQLException e) {
			Log.error(e.getMessage());

			return false;
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * eu.project.Cacao.Interfaces.DB#addLibrary(eu.project.Cacao.Connectors
	 * .DB.Library)
	 */
	public boolean addLibrary(Library library) {
		try {
			if (conn.isClosed())
				this.connect();
			
			String languagesDBString = "";
			String[] languages = library.getLanguages();

			for (int i = 0; i < languages.length; i++) {
				languagesDBString += (languages[i] + this.valuesSeparator);
			}

			PreparedStatement stmt = conn.prepareStatement("INSERT INTO Libraries(name, languages, url, protocol) VALUES(?,?,?,?);");
			stmt.setString(1, library.getName());
			stmt.setString(2, languagesDBString);
			stmt.setString(3, library.getURL());
			stmt.setString(4, library.getProtocol());
			stmt.executeUpdate();

			ResultSet resID = stmt.getGeneratedKeys();
			resID.next();
			library.setID(resID.getString(1));

			stmt.close();

			return true;
		} catch (SQLException e) {
			Log.error(e.getMessage());
			return false;
		}
	}

	public int addErrorRecordInfo(String library_id, String collection_id, String record_id) {
		
		try {
			if (conn.isClosed())
				this.connect();

			PreparedStatement stmt = conn
			.prepareStatement("INSERT INTO Records(library_id, collection_id, record_id, lastupdate, deleted, skipped) VALUES(?,?,?,?,?,?);");
			stmt.setInt(1, Integer.parseInt(library_id));
			stmt.setInt(2, Integer.parseInt(collection_id));
			stmt.setString(3, record_id);
			stmt.setString(4, getNowString());
			stmt.setString(5, "0");
			stmt.setInt(6, 1);
			stmt.executeUpdate();		
			
			return 0;
			
		} catch (Exception e) {
			Log.error(e.getMessage());
			return -1;
		}
	}
	
	
	
	public boolean checkRecordCollection (LibraryRecord record, LibraryCollection collection) {
		try {
			if (conn.isClosed())
				this.connect();
			

			LibraryRecord rec = getRecordByLibraryID(record.getLibrary(), record.getDatabaseID());

			String[] multiCollection = rec.getMultiCollection();

			for (int c = 0; c < multiCollection.length; c++) {
				if (multiCollection[c].equalsIgnoreCase(collection.getID()))
					return true;
			}
			
			return false;
			
		} catch (Exception e) {
			Log.error(e.getMessage());
			return false;
		}
	}
	
	public int updateRecordCollection (LibraryRecord record) {
		try {
			if (conn.isClosed())
				this.connect();
			
			if (record.getLastUpdate() == null) {
				record.setLastUpdate(now());
			}

			Log.error("Add collection " + record.getCollection().getCode() + " to record " + record.getLibraryID());
			
			LibraryRecord rec = getRecordByLibraryID(record.getLibrary(), record.getDatabaseID());

			String newMultiCollection = this.addToMultiCollectionString(rec.getMultiCollection(), record.getCollection().getID());

			PreparedStatement stmtUp = conn.prepareStatement("UPDATE Records SET multiCollection=? WHERE record_id=?;");
			stmtUp.setString(1, newMultiCollection);
			stmtUp.setString(2, record.getDatabaseID());
			stmtUp.execute();
			stmtUp.close();

			return 0;
			
		} catch (Exception e) {
			Log.error(e.getMessage());
			return -1;
		}
	}
	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * eu.project.Cacao.Interfaces.DB#addRecord(eu.project.Cacao.Connectors.
	 * DB.Record)
	 */
	public int addRecord(LibraryRecord record) {
		try {
			if (conn.isClosed())
				this.connect();

			DublinCoreRecord dublincore = record.getDublinCoreData();
			TelAPRecord telap = record.getTelAPData();

			if (record.getLastUpdate() == null) {
				record.setLastUpdate(now());
			}

			LibraryRecord rec = getRecordByLibraryID(record.getLibrary(), record.getDatabaseID());

			if (rec == null) {
				PreparedStatement stmt = conn
						.prepareStatement("INSERT INTO Records(library_id, collection_id, record_id, dublincoredata, telapdata, lastupdate, deleted, multiCollection) VALUES(?,?,?,?,?,?,?,?);");
				stmt.setInt(1, record.getLibrary().getIDasInteger());
				stmt.setInt(2, record.getCollection().getIDasInteger());
				stmt.setString(3, record.getDatabaseID());
				stmt.setString(4, record.getDublinCoreData() == null ? "" : record.getDublinCoreData().getDublinCoreString());
				stmt.setString(3, record.getDatabaseID());
				stmt.setString(5, record.getTelAPData() == null ? "" : record.getTelAPData().getTELAPString());
				stmt.setString(6, record.getLastUpdate() == null ? getNowString() : record.getLastUpdate());
				stmt.setString(7, (record.getDeleted() ? "1" : "0"));
				stmt.setString(8, record.getCollection().getID());
				stmt.executeUpdate();

				ResultSet rsID = stmt.getGeneratedKeys();
				rsID.next();

				int record_id = rsID.getInt(1);

				stmt.close();

				if (dublincore != null) {
					PreparedStatement stmtDC = conn
							.prepareStatement("INSERT INTO MetaData (record_id, title, creator, subject, description, publisher, contributor, date, type, format, identifier, source, language, relation, coverage, rights, subjectAttributes) VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?);");
					stmtDC.setInt(1, record_id);
					stmtDC.setString(2, new String(getDBSeparatedString(dublincore.getTitle()).getBytes(),"UTF-8"));
					stmtDC.setString(3, getDBSeparatedString(dublincore.getCreator()));
					stmtDC.setString(4, getDBSeparatedString(dublincore.getSubject()));
					stmtDC.setString(5, getDBSeparatedString(dublincore.getDescription()));
					stmtDC.setString(6, getDBSeparatedString(dublincore.getPublisher()));
					stmtDC.setString(7, getDBSeparatedString(dublincore.getContributor()));
					stmtDC.setString(8, getDBSeparatedString(dublincore.getDate()));
					stmtDC.setString(9, getDBSeparatedString(dublincore.getType()));
					stmtDC.setString(10, getDBSeparatedString(dublincore.getFormat()));
					stmtDC.setString(11, getDBSeparatedString(dublincore.getIdentifier()));
					stmtDC.setString(12, getDBSeparatedString(dublincore.getSource()));
					stmtDC.setString(13, getDBSeparatedString(dublincore.getLanguage()));
					stmtDC.setString(14, getDBSeparatedString(dublincore.getRelation()));
					stmtDC.setString(15, getDBSeparatedString(dublincore.getCoverage()));
					stmtDC.setString(16, getDBSeparatedString(dublincore.getRights()));
					stmtDC.setString(17, getDBSeparatedHashString(dublincore.getSubjectAtts()));
					stmtDC.execute();
					stmtDC.close();
				} else if (telap != null) {
					PreparedStatement stmtDC = conn
							.prepareStatement("INSERT INTO MetaData (record_id, title, creator, subject, description, publisher, contributor, date, type, format, identifier, source, language, relation, coverage, rights, subjectAttributes, audience, location, recordId) VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?);");
					stmtDC.setInt(1, record_id);
					stmtDC.setString(2, getDBSeparatedString(telap.getTitle()));
					stmtDC.setString(3, getDBSeparatedString(telap.getCreator()));
					stmtDC.setString(4, getDBSeparatedString(telap.getSubject()));
					stmtDC.setString(5, getDBSeparatedString(telap.getDescription()));
					stmtDC.setString(6, getDBSeparatedString(telap.getPublisher()));
					stmtDC.setString(7, getDBSeparatedString(telap.getContributor()));
					stmtDC.setString(8, getDBSeparatedString(telap.getDate()));
					stmtDC.setString(9, getDBSeparatedString(telap.getType()));
					stmtDC.setString(10, getDBSeparatedString(telap.getFormat()));
					stmtDC.setString(11, getDBSeparatedString(telap.getIdentifier()));
					stmtDC.setString(12, getDBSeparatedString(telap.getSource()));
					stmtDC.setString(13, getDBSeparatedString(telap.getLanguage()));
					stmtDC.setString(14, getDBSeparatedString(telap.getRelation()));
					stmtDC.setString(15, getDBSeparatedString(telap.getCoverage()));
					stmtDC.setString(16, getDBSeparatedString(telap.getRights()));
					stmtDC.setString(17, getDBSeparatedHashString(telap.getSubjectAtts()));
					stmtDC.setString(18, getDBSeparatedString(telap.getAudience()));
					stmtDC.setString(19, getDBSeparatedString(telap.getLocation()));
					stmtDC.setString(20, getDBSeparatedString(telap.getRecordId()));
					stmtDC.execute();
					stmtDC.close();
				}
				return 1;
			} else {
				String newMultiCollection = this.addToMultiCollectionString(rec.getMultiCollection(), record.getCollection().getID());

				PreparedStatement stmtUp = conn.prepareStatement("UPDATE Records SET multiCollection=? WHERE record_id=?;");
				stmtUp.setString(1, newMultiCollection);
				stmtUp.setString(2, record.getDatabaseID());
				stmtUp.execute();
				stmtUp.close();
				return 0;
			}
		} catch (Exception e) {
			Log.error("MySQL->addRecord(): " + e.getMessage());
			return -1;
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see eu.project.Cacao.Interfaces.DB#connect()
	 */
	public boolean connect() {
		String url = "";

		try {
			url = "jdbc:mysql://" + server + "/" + db;
			Class.forName("com.mysql.jdbc.Driver").newInstance();
			conn = DriverManager.getConnection(url, username, password);

			return true;
		} catch (Exception e) {
			Log.error(e.getMessage());
			return false;
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see eu.project.Cacao.Interfaces.DB#close()
	 */
	public boolean close() {
		try {
			conn.close();

			return true;
		} catch (Exception e) {
			Log.error(e.getMessage());
			return false;
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see eu.project.Cacao.Interfaces.DB#delCollection(java.lang.String)
	 */
	public boolean deleteCollection(String ID) {
		// TODO Auto-generated method stub
		return false;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see eu.project.Cacao.Interfaces.DB#delRecord(java.lang.String)
	 */
	public boolean deleteRecord(String ID) {
		// TODO Auto-generated method stub
		return false;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see eu.project.Cacao.Interfaces.DB#deleteLibrary(java.lang.String)
	 */
	public boolean deleteLibrary(String ID) {
		// TODO Auto-generated method stub
		return false;
	}

	public LibraryCollection getCollection(Library library, String code) {
		try {
			Statement stmt = conn.createStatement();
			ResultSet res = stmt.executeQuery("SELECT ID, library_id, description, code, lastupdate FROM Collections WHERE library_id=" + library.getID() + " AND code = '" + code + "';");

			if (res.first()) {
				LibraryCollection collection = new LibraryCollection(res.getString(1), res.getString(2), res.getString(3), res.getString(4), res.getString(5), this);

				stmt.close();

				return collection;
			}

			stmt.close();

			return null;
		} catch (SQLException e) {
			Log.error(e.getMessage());

			return null;
		}	
	}
	
	/*
	 * (non-Javadoc)
	 * 
	 * @see eu.project.Cacao.Interfaces.DB#getCollection(java.lang.String)
	 */
	public LibraryCollection getCollection(String ID) {
		try {
			Statement stmt = conn.createStatement();
			ResultSet res = stmt.executeQuery("SELECT ID, library_id, description, code, lastupdate FROM Collections WHERE ID=" + ID + ";");

			if (res.first()) {
				LibraryCollection collection = new LibraryCollection(res.getString(1), res.getString(2), res.getString(3), res.getString(4), res.getString(5), this);

				stmt.close();

				return collection;
			}

			stmt.close();

			return null;
		} catch (SQLException e) {
			Log.error(e.getMessage());

			return null;
		}	
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see eu.project.Cacao.Interfaces.DB#getLibrary(java.lang.String)
	 */
	public Library getLibrary(String ID) {
		try {
			Statement stmt = conn.createStatement();
			ResultSet res = stmt.executeQuery("SELECT ID, name, languages, url, protocol FROM Libraries WHERE ID=" + ID + ";");

			if (res.first()) {
				String[] languages = res.getString(3).split(this.valuesSeparator);

				Library library = new Library(res.getString(1), res.getString(2), languages, res.getString(4), res.getString(5));

				stmt.close();

				return library;
			}

			stmt.close();

			return null;
		} catch (SQLException e) {
			Log.error(e.getMessage());

			return null;
		}
	}

	public Library getLibraryByURL(String url) {
		try {
			Statement stmt = conn.createStatement();
			ResultSet res = stmt.executeQuery("SELECT ID, name, languages, url, protocol FROM Libraries WHERE url='" + url + "';");

			if (res.first()) {
				String[] languages = res.getString(3).split(this.valuesSeparator);

				Library library = new Library(res.getString(1), res.getString(2), languages, res.getString(4), res.getString(5));

				stmt.close();

				return library;
			}

			stmt.close();

			return null;
		} catch (SQLException e) {
			Log.error(e.getMessage());

			return null;
		}
	}
	
	public LibraryRecord getRecordByLibraryID(String ID) {
		return getRecordByLibraryID(null, ID);
	}
	
	/*
	 * (non-Javadoc)
	 * 
	 * @see eu.project.Cacao.Interfaces.DB#getRecord(java.lang.String)
	 */
	public LibraryRecord getRecordByLibraryID(Library library, String ID) {
		try {

			LibraryRecord record = null;

			Statement stmt = conn.createStatement();
			ResultSet res;
			
			if (library == null)
				res = stmt.executeQuery("SELECT ID, library_id, collection_id, record_id, dublincoredata, telapdata, lastupdate, deleted, multiCollection FROM Records WHERE record_id='" + ID + "';");
			else
				res = stmt.executeQuery("SELECT ID, library_id, collection_id, record_id, dublincoredata, telapdata, lastupdate, deleted, multiCollection FROM Records WHERE library_id = " + library.getID() + " AND record_id='" + ID + "';");
			
			if (res.first()) {

				if (res.getString(5).trim() != "")
					record = new LibraryRecord(res.getString(4), res.getString(2), res.getString(3), new DublinCoreRecord(res.getString(4), res.getString(5)), null, res
							.getString(7), res.getString(8).equals("1"), res.getString(9), this);
				else if (res.getString(6).trim() != "") {
					record = new LibraryRecord(res.getString(4), res.getString(2), res.getString(3), null, new TelAPRecord(res.getString(4), res.getString(6)), res.getString(7),
							res.getString(8).equals("1"), res.getString(9), this);
				}
				stmt.close();

				return record;
			}

			stmt.close();

			return null;
		} catch (Exception e) {
			Log.error(e.toString());

			return null;
		}
	}
	
	/*
	 * (non-Javadoc)
	 * 
	 * @see eu.project.Cacao.Interfaces.DB#getRecord(java.lang.String)
	 */
	public LibraryRecord getRecordByDatabaseID(long ID) {
		try {

			LibraryRecord record = null;

			Statement stmt = conn.createStatement();
			ResultSet res = stmt
					.executeQuery("SELECT ID, library_id, collection_id, record_id, dublincoredata, telapdata, lastupdate, deleted, multiCollection FROM Records WHERE ID = "
							+ ID + " ;");

			if (res.first()) {

				if (res.getString(5).trim() != "")
					record = new LibraryRecord(res.getString(4), res.getString(2), res.getString(3), new DublinCoreRecord(res.getString(4), res.getString(5)), null, res
							.getString(7), res.getString(8).equals("1"), res.getString(9), this);
				else if (res.getString(6).trim() != "") {
					record = new LibraryRecord(res.getString(4), res.getString(2), res.getString(3), null, new TelAPRecord(res.getString(4), res.getString(6)), res.getString(7),
							res.getString(8).equals("1"), res.getString(9), this);
				}
				stmt.close();

				return record;
			}

			stmt.close();

			return null;
		} catch (Exception e) {
			Log.error(e.getMessage());

			return null;
		}
	}
	
	/*
	 * (non-Javadoc)
	 * 
	 * @see eu.project.Cacao.Interfaces.DB#getAllRecords(Library library,
	 * LibraryCollection collection)
	 */
	public String[] getAllRecords(Library library, LibraryCollection collection) {
		Vector<String> results = new Vector<String>();

		try {
			Statement stmt = conn.createStatement();
			ResultSet res = null;

			if (library != null && collection != null) {
				res = stmt.executeQuery("SELECT ID FROM Records WHERE skipped = 0 AND library_id=" + library.getID() + " AND collection_id=" + collection.getID() + ";");
			} else {
				res = stmt.executeQuery("SELECT ID FROM Records WHERE skipped = 0 ;");
			}

			while (res.next()) {
				results.add(res.getString(1));
			}

			String[] resultsStr = results.toArray(new String[0]);

			stmt.close();

			return (resultsStr);
		} catch (SQLException e) {
			Log.error(e.getMessage());

			return null;
		}
	}
	
	public String[] getAllRecords(Library library) {
		Vector<String> results = new Vector<String>();

		try {
			Statement stmt = conn.createStatement();
			ResultSet res = null;

			if (library != null) {
				res = stmt.executeQuery("SELECT ID FROM Records WHERE skipped = 0 AND library_id=" + library.getID() + ";");
			} else {
				res = stmt.executeQuery("SELECT ID FROM Records WHERE skipped = 0;");
			}

			while (res.next()) {
				results.add(res.getString(1));
			}

			String[] resultsStr = results.toArray(new String[0]);

			stmt.close();

			return (resultsStr);
		} catch (SQLException e) {
			Log.error(e.getMessage());

			return null;
		}
	}
	

	/*
	 * (non-Javadoc)
	 * 
	 * @see eu.project.Cacao.Interfaces.DB#getAllCollections()
	 */
	public String[] getAllCollections() {
		return getAllCollections(null);
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see eu.project.Cacao.Interfaces.DB#getAllCollections(Library library)
	 */
	public String[] getAllCollections(Library library) {
		Vector<String> results = new Vector<String>();

		try {
			Statement stmt = conn.createStatement();
			ResultSet res = null;

			if (library != null) {
				res = stmt.executeQuery("SELECT ID FROM Collections WHERE library_id=" + library.getID() + ";");
			} else {
				res = stmt.executeQuery("SELECT ID FROM Collections;");
			}

			while (res.next()) {
				results.add(res.getString(1));
			}

			String[] resultsStr = results.toArray(new String[0]);

			stmt.close();

			return (resultsStr);
		} catch (SQLException e) {
			Log.error(e.getMessage());
			return null;
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see eu.project.Cacao.Interfaces.DB#getAllLibraries()
	 */
	public String[] getAllLibraries() {
		Vector<String> results = new Vector<String>();

		try {
			Statement stmt = conn.createStatement();
			ResultSet res = stmt.executeQuery("SELECT ID FROM Libraries;");

			while (res.next()) {
				results.add(res.getString(1));
			}

			String[] resultsStr = results.toArray(new String[0]);

			stmt.close();

			return (resultsStr);
		} catch (SQLException e) {
			Log.error(e.getMessage());
			return null;
		}
	}

	/**
	 * Return password used to connect to MySQL server
	 * 
	 * @return the password
	 */
	public String getPassword() {
		return password;
	}

	/**
	 * Set password used to connect to MySQL server
	 * 
	 * @param password
	 *            the password to set
	 */
	public void setPassword(String password) {
		this.password = password;
	}

	/**
	 * Return the MySQL server hostname/ip address
	 * 
	 * @return the server
	 */
	public String getServer() {
		return server;
	}

	/**
	 * Set the MySQL server hostname/ip address
	 * 
	 * @param server
	 *            the server to set
	 */
	public void setServer(String server) {
		this.server = server;
	}

	/**
	 * Return the name of database
	 * 
	 * @return the database name
	 */
	public String getDBname() {
		return db;
	}

	/**
	 * Set the name of database
	 * 
	 * @param dbname
	 *            the database name to set
	 */
	public void setDBname(String dbname) {
		this.db = dbname;
	}

	/**
	 * Return the username used to connect to MySQL server
	 * 
	 * @return the username
	 */
	public String getUsername() {
		return username;
	}

	/**
	 * Set the username used to connect to MySQL server
	 * 
	 * @param username
	 *            the username to set
	 */
	public void setUsername(String username) {
		this.username = username;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see eu.project.Cacao.Interfaces.DB#getDBSeparatedString(Vector<String>
	 * v)
	 */
	public String getDBSeparatedString(Vector<String> v) {
		String res = "";

		Iterator<String> it = v.iterator();

		while (it.hasNext()) {
			res += ((String) it.next() + this.valuesSeparator);
		}

		return res;
	}

	public String getDBSeparatedHashString(Vector<Hashtable<String, String>> v) {
		String res = "";

		Iterator<Hashtable<String, String>> it = v.iterator();

		while (it.hasNext()) {
			Hashtable<String, String> h = it.next();
			Enumeration<String> keys = h.keys();

			while (keys.hasMoreElements()) {
				String key = (String) keys.nextElement();
				String value = h.get(key);
				res += (key + this.hashSeparator + value + "|");
			}

			res += this.valuesSeparator;
		}

		return res;
	}

	/**
	 * Return a String with present timestamp formatted for MySQL query
	 * 
	 * @return now timestamp
	 */
	private String now() {
		Date now = new Date();
		SimpleDateFormat nowdf = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");

		return nowdf.format(now);
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see eu.project.Cacao.Interfaces.DB#getValuesSeparator()
	 */
	public String getValuesSeparator() {
		return valuesSeparator;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see eu.project.Cacao.Interfaces.DB#getHashSeparator()
	 */
	public String getHashSeparator() {
		return hashSeparator;
	}

	public Connection getConnection() {
		return conn;
	}

	private String getNowString() {
		Date date = new Date();
		SimpleDateFormat formatter = new SimpleDateFormat("yyyy-MM-dd hh-mm-ss");
		return formatter.format(date);
	}

	private String addToMultiCollectionString(String[] multiCollection, String collection) {
		String res = "";

		for (int i = 0; i < multiCollection.length; i++) {
			if (!multiCollection[i].trim().equals(collection.trim()))
				res += multiCollection[i] + "<";
		}

		res += collection;

		return res;
	}
}
