package eu.project.Cacao.Apps;

import eu.project.Cacao.Connectors.DB.Library;
import eu.project.Cacao.Connectors.DB.LibraryCollection;
import eu.project.Cacao.Connectors.DB.LibraryRecord;
import eu.project.Cacao.Connectors.Metadata.DublinCoreRecord;
import eu.project.Cacao.Connectors.OAIPMH.OAIPMHAccess;


public class OAITest {
    public static void main(String[] args) {
        String url = "http://tesi.cab.unipd.it/perl/oai2"; //"http://cacao.inf.unibz.it/joai/provider"; //

        OAIPMHAccess oai = new OAIPMHAccess(url);
        
        String[] libraries = oai.retrieveLibrariesID();
        for(String libraryID: libraries){
        	Library library = oai.retrieveLibrary(libraryID);
        	System.out.println(library.getName());

        	String[] colls = oai.retrieveCollectionsID(library);
        	for(String coll: colls){
        		LibraryCollection collection = oai.retrieveCollection(coll);
        		System.out.println("Collection code: " + collection.getCode() +" Name: " + collection.getDescription());
        		String[] records = oai.retrieveRecordsID(collection);
        		System.out.println(">> size of collection: "+records.length);
        		for(String rID: records){
        			LibraryRecord record = oai.retrieveRecord(rID, collection);
        			DublinCoreRecord dcr = record.getDublinCoreData();
        			System.out.println(">> "+rID+" - "+dcr.getTitle().elementAt(0) );
        		}
        	}
        }
    }
}
