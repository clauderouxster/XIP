package eu.project.Cacao.Components;

/**
 * The class retrieves category membership of terms stored in a DB by the WordToCategory_Manager
 * @author Alessio
 *
 */
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.util.Collection;
import java.util.HashSet;

import org.apache.log4j.Logger;

import eu.project.Cacao.Components.Utils.NLPtoken;
import eu.project.Cacao.Connectors.DB.MySQL;

public class WordToCategory_Component {
	private MySQL cacaodb=null;
	
	protected Logger logger = Logger.getLogger(WordToCategory_Component.class.getName());
	
	static public void main(String[] args) {
        try { // demo.celi.it:8095
        	System.err.close();
        	
        	WordToCategory_Component a = new WordToCategory_Component("cacaoUser", "cacao","hermes.celi.it");
            //String result = a.processQuery("Roman law", "en", true, false, true);
            //System.out.println(">> " + result);
            
           HashSet<String> res=a.getCategoriesByWord("British Library", "Dewey", "peace", null, "en");
           for(String s:res)
        	   System.out.println(s);
           
        } catch (Exception e) {
            e.printStackTrace();
        }
    } 
	
	// the idea is to have a component per library/language (so in Bolzano we
	// will have 3 w2c_component) to allow multilingual libraries
	public WordToCategory_Component( String user, String pwd, String dbServer) {
		this.cacaodb = new MySQL(user, pwd, dbServer, "CACAO");
		cacaodb.connect();
	}
	
	public HashSet<String> getCategoriesByWords(String libraryID, String classificationID, 
			Collection<NLPtoken> terms, String lang){
		HashSet<String> commonCategories=new HashSet<String>();
		
		boolean isFirst=true;
		for(NLPtoken tok: terms){
			HashSet<String> result = getCategoriesByWord(libraryID, classificationID, tok.lemma, tok.category, lang);
			if(isFirst){
				commonCategories = result;
				isFirst = false;
			}
			else{
				if(result.size()>0)
					commonCategories.retainAll(result);
			}
		}
		
//		HashSet<String> children=new HashSet<String>();
//		for(String cat: commonCategories)
//			children.addAll(classificationExplorer.getChildren("Dewey", cat));
//		commonCategories.addAll(children);
		
		return commonCategories;
		
	}
	
	public HashSet<NLPtoken> getWordByWord(String libraryID, String classificationID, String term, String POS, String langFrom, String langTo){
		HashSet<NLPtoken> results=new HashSet<NLPtoken>();
		HashSet<String> categories=getCategoriesByWord(libraryID, classificationID, term, POS, langFrom);
		for(String cat: categories){
			results.addAll( this.getWordsByCategory(libraryID, classificationID, cat, langTo) );
		}
		return results;
	}


	public HashSet<String> getCategoriesByWord(String libraryID, String classificationID, String term, String POS, String lang) {
		HashSet<String> results=new HashSet<String>();
		try {
			
			PreparedStatement stmt = null;
			if(POS==null)
				stmt = this.cacaodb.getConnection().prepareStatement("SELECT c.categoryID from Word2Cat_Categories as c, Word2Cat_Terms as t where c.language=? and c.classificationID=? AND c.collectionID=? AND c.ID=t.categoryID and t.term=?;");
			else
				stmt = this.cacaodb.getConnection().prepareStatement("SELECT c.categoryID from Word2Cat_Categories as c, Word2Cat_Terms as t where c.language=? and c.classificationID=? AND c.collectionID=? AND c.ID=t.categoryID and t.term=? and t.POS=?;");
			
			stmt.setString(1, lang);
			stmt.setString(2, classificationID);
			stmt.setString(3, libraryID);
			stmt.setString(4, term);
			if(POS!=null)
				stmt.setString(5, POS);
			
			stmt.execute();

			ResultSet rs = stmt.getResultSet();
			
			while(rs.next()){
				String s=rs.getString(1);
				results.add(s);
			}
			

			stmt.close();
		} catch (Exception e) {
			logger.error("Can't select category entry with " + classificationID +" "+term+" "+lang+"\n" + e.getMessage());
		}
		return results;
	}

	public HashSet<NLPtoken> getWordsByCategory(String libraryID, String classificationID, String categoryID, String lang) {
		HashSet<NLPtoken> results=new HashSet<NLPtoken>();
		try {

			PreparedStatement stmt = this.cacaodb.getConnection().prepareStatement("SELECT distinct t.term, t.POS, t.relevance from CACAO.Word2Cat_Categories as c, CACAO.Word2Cat_Terms as t where c.language=? and c.classificationID=? AND c.collectionID=? AND c.categoryID=? AND t.categoryID=c.ID order by t.relevance DESC;");

			stmt.setString(1, lang);
			stmt.setString(2, classificationID);
			stmt.setString(3, libraryID);
			stmt.setString(4, categoryID);
			stmt.execute();

			ResultSet rs = stmt.getResultSet();
			
			while(rs.next()){
				String term=rs.getString(1);
				String pos=rs.getString(2);
				float relevance=rs.getFloat(3);
				NLPtoken tok=new NLPtoken(term, pos, term, null, lang);
				results.add(tok);
			}
			

			stmt.close();
		} catch (Exception e) {
			logger.error("Can't select category entry with " + classificationID +" "+categoryID+" "+lang+"\n" + e.getMessage());
		}
		return results;
	}
	
	
}
