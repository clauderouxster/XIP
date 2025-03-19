package eu.project.Cacao.Components.Utils;

import java.util.Collection;
import java.util.HashSet;

import org.apache.log4j.Logger;
import org.apache.solr.common.SolrDocument;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.NodeList;


/**
 * A Simple Search ressult container that just stores everything as strings
 *
 **/
public class DocsSearchResult
{	
	public static final String recordElementTag="Record", libraryID_Elm="libraryID", recordID_Elm="recordID", title_Elm="title", subj_Elm="subject", 
	lang_Elm="language", publisher_Elm="publisher", OAIset_Elm="OAI_set", type_Elm="type", scoreAttr="recordRelevance", 
	telRecordID_URL_Elm="telRecordID_URL", ISSN_Elm="ISSN", ISBN_Elm="ISBN", url_Elm="URL", contributor_Elm="contributor", 
	description_Elm="description", tableOfContents_Elm="tableOfContents", identifier_Elm="identifier", creator_Elm="creator";
	
    private String score, title, publisher, URL, ISSN, ISBN, telRecordID_URL, recordID, type, identifier, libraryID;
    private HashSet<String> subjects, oaiSets, descriptions, tableOfContents, languages, contributors, creators ;
    
	public DocsSearchResult(String score, String libraryID, String title, String publisher, String url, String issn, String isbn, String telRecordID_URL, 
			String identifier, String recordID, String type, HashSet<String> subjects, HashSet<String> oaiSets, 
			HashSet<String> descriptions, HashSet<String> tableOfContents, HashSet<String> contributors, HashSet<String> languages, HashSet<String> creators) {
		super();
		this.score = score;
		this.title = title;
		this.publisher = publisher;
		this.URL = url;
		this.ISSN = issn;
		this.ISBN = isbn;
		this.telRecordID_URL = telRecordID_URL;
		this.recordID = recordID;
		this.libraryID = libraryID;
		this.type = type;
		this.subjects = subjects;
		this.oaiSets = oaiSets;
		this.descriptions = descriptions;
		this.tableOfContents = tableOfContents;
		this.languages = languages;
		this.identifier = identifier;
		this.contributors = contributors;
		this.languages = languages;
		this.creators = creators;
	}


    public String getScore()
    {
        return score;
    }


    public String getPublisher()
    {
        return publisher;
    }


    public String getTitle()
    {
        return title;
    }

	
	public HashSet<String> getSubjects() {
		return subjects;
	}


	public String getURL() {
		return URL;
	}


	public String getRecordID() {
		return recordID;
	}
	
	public Element toXMLElement(Document doc){
		Element result = doc.createElement(DocsSearchResult.recordElementTag);
		result.setAttribute(DocsSearchResult.scoreAttr, this.score);
		
		if(this.libraryID !=null){
			Element libraryID = doc.createElement(DocsSearchResult.libraryID_Elm);
			libraryID.setTextContent(this.libraryID);
			result.appendChild(libraryID);
		}
		
		if(this.recordID !=null){
			Element id = doc.createElement(DocsSearchResult.recordID_Elm);
			id.setTextContent(this.recordID);
			result.appendChild(id);
		}
		
		if(this.telRecordID_URL != null){
			Element telRecordID_URL = doc.createElement(DocsSearchResult.telRecordID_URL_Elm);
			telRecordID_URL.setTextContent(this.telRecordID_URL);
			result.appendChild(telRecordID_URL);
		}
		
		if(this.URL != null){
			Element url = doc.createElement(DocsSearchResult.url_Elm);
			url.setTextContent(this.URL);
			result.appendChild(url);
		}
		
		if(this.ISBN != null){
			Element isbn = doc.createElement(DocsSearchResult.ISBN_Elm);
			isbn.setTextContent(this.ISBN);
			result.appendChild(isbn);
		}
		
		if(this.ISSN != null){
			Element issn = doc.createElement(DocsSearchResult.ISSN_Elm);
			issn.setTextContent(this.ISSN);
			result.appendChild(issn);
		}
		
		if(this.identifier != null){
			Element identifier = doc.createElement(DocsSearchResult.identifier_Elm);
			identifier.setTextContent(this.identifier);
			result.appendChild(identifier);
		}
		
		if(this.oaiSets != null){
			for(String s: this.oaiSets){
				Element oaiSetsElm = doc.createElement(DocsSearchResult.OAIset_Elm);
				oaiSetsElm.setTextContent(s);
				result.appendChild(oaiSetsElm);
			}
		}
		
		if(this.title != null){
			Element title = doc.createElement(DocsSearchResult.title_Elm);
			title.setTextContent(this.title);
			result.appendChild(title);
		}

		
		if(this.subjects != null){
			for(String s: this.subjects){
				Element subj = doc.createElement(DocsSearchResult.subj_Elm);
				subj.setTextContent(s);
				result.appendChild(subj);
			}
		}
		
		if(this.tableOfContents != null){
			for(String s: this.tableOfContents){
				Element tocElm = doc.createElement(DocsSearchResult.tableOfContents_Elm);
				tocElm.setTextContent(s);
				result.appendChild(tocElm);
			}
		}
		
		if(this.descriptions != null){
			for(String s: this.descriptions){
				Element descriptionElm = doc.createElement(DocsSearchResult.description_Elm);
				descriptionElm.setTextContent(s);
				result.appendChild(descriptionElm);
			}
		}
		
		if(this.contributors != null){
			for(String s: this.contributors){
				Element contributorElm = doc.createElement(DocsSearchResult.contributor_Elm);
				contributorElm.setTextContent(s);
				result.appendChild(contributorElm);
			}
		}
		
		if(this.publisher != null){
			Element publisher = doc.createElement(DocsSearchResult.publisher_Elm);
			publisher.setTextContent(this.publisher);
			result.appendChild(publisher);
		}
		
		if(this.type != null){
			Element type = doc.createElement(DocsSearchResult.type_Elm);
			type.setTextContent(this.type);
			result.appendChild(type);
		}
		
		if(this.languages != null){
			for(String s: this.languages){
				Element langElm = doc.createElement(DocsSearchResult.lang_Elm);
				langElm.setTextContent(s);
				result.appendChild(langElm);
			}
		}
		
		if(this.creators != null){
			for(String s: this.creators){
				Element creatorElm = doc.createElement(DocsSearchResult.creator_Elm);
				creatorElm.setTextContent(s);
				result.appendChild(creatorElm);
			}
		}
		
		return result;
	}
	
	static public DocsSearchResult createFromXMLElement(Element e){
		String recordID=null, title=null, libraryID=null, publisher=null, URL=null, score="-", ISSN=null, ISBN=null, 
		telrecordID_URL=null, type=null, identifier=null, creator=null;
		
		if(e.hasAttribute(DocsSearchResult.scoreAttr))
		score=e.getAttribute(DocsSearchResult.scoreAttr);
		
		NodeList nl= e.getElementsByTagName(DocsSearchResult.recordID_Elm);
		if(nl != null && nl.getLength()>0){
			recordID = nl.item(0).getTextContent();
		}
		
		nl= e.getElementsByTagName(DocsSearchResult.libraryID_Elm);
		if(nl != null && nl.getLength()>0){
			libraryID = nl.item(0).getTextContent();
		}
		
		nl= e.getElementsByTagName(DocsSearchResult.url_Elm);
		if(nl != null && nl.getLength()>0){
			URL = nl.item(0).getTextContent();
		}
		
		nl= e.getElementsByTagName(DocsSearchResult.ISSN_Elm);
		if(nl != null && nl.getLength()>0){
			ISSN = nl.item(0).getTextContent();
		}
		
		nl= e.getElementsByTagName(DocsSearchResult.ISBN_Elm);
		if(nl != null && nl.getLength()>0){
			ISBN = nl.item(0).getTextContent();
		}
		
		nl= e.getElementsByTagName(DocsSearchResult.telRecordID_URL_Elm);
		if(nl != null && nl.getLength()>0){
			telrecordID_URL = nl.item(0).getTextContent();
		}
		
		nl= e.getElementsByTagName(DocsSearchResult.type_Elm);
		if(nl != null && nl.getLength()>0){
			type = nl.item(0).getTextContent();
		}
		
		nl= e.getElementsByTagName(DocsSearchResult.identifier_Elm);
		if(nl != null && nl.getLength()>0){
			identifier = nl.item(0).getTextContent();
		}
		
		nl= e.getElementsByTagName(DocsSearchResult.title_Elm);
		if(nl != null && nl.getLength()>0){
			title = nl.item(0).getTextContent();
		}
		
		nl= e.getElementsByTagName(DocsSearchResult.publisher_Elm);
		if(nl != null && nl.getLength()>0){
			publisher = nl.item(0).getTextContent();
		}
		
		nl= e.getElementsByTagName(DocsSearchResult.subj_Elm);
		HashSet<String> subjects=new HashSet<String>();
		for(int i=0;nl!= null && i<nl.getLength(); i++){
			subjects.add( nl.item(i).getTextContent() );
		}
		
		nl= e.getElementsByTagName(DocsSearchResult.OAIset_Elm);
		HashSet<String> oaiSets=new HashSet<String>();
		for(int i=0;nl!= null && i<nl.getLength(); i++){
			oaiSets.add( nl.item(i).getTextContent() );
		}
		
		nl= e.getElementsByTagName(DocsSearchResult.tableOfContents_Elm);
		HashSet<String> tableOfContents=new HashSet<String>();
		for(int i=0;nl!= null && i<nl.getLength(); i++){
			tableOfContents.add( nl.item(i).getTextContent() );
		}
		
		nl= e.getElementsByTagName(DocsSearchResult.description_Elm);
		HashSet<String> descriptions=new HashSet<String>();
		for(int i=0;nl!= null && i<nl.getLength(); i++){
			descriptions.add( nl.item(i).getTextContent() );
		}
		
		nl= e.getElementsByTagName(DocsSearchResult.lang_Elm);
		HashSet<String> languages=new HashSet<String>();
		for(int i=0;nl!= null && i<nl.getLength(); i++){
			languages.add( nl.item(i).getTextContent() );
		}
		
		nl= e.getElementsByTagName(DocsSearchResult.contributor_Elm);
		HashSet<String> contributors=new HashSet<String>();
		for(int i=0;nl!= null && i<nl.getLength(); i++){
			contributors.add( nl.item(i).getTextContent() );
		}
		
		nl= e.getElementsByTagName(DocsSearchResult.creator_Elm);
		HashSet<String> creators=new HashSet<String>();
		for(int i=0;nl!= null && i<nl.getLength(); i++){
			creators.add( nl.item(i).getTextContent() );
		}
		
    	return new DocsSearchResult( score,  libraryID, title,  publisher,  URL,  ISSN,  ISBN,  telrecordID_URL, 
				 identifier,  recordID,  type,  subjects,  oaiSets, descriptions,  tableOfContents, contributors, languages, creators);
    }
	
	public static DocsSearchResult createSearchResultFromSolrDocument(SolrDocument doc){
		Logger logger = Logger.getLogger(DocsSearchResult.class.getName());
		
		String recordID=null, title=null, libraryID=null, publisher=null, URL=null, score="-", ISSN=null, ISBN=null, 
			telrecordID_URL=null, type=null, identifier=null;
		
		Object tmp = doc.getFieldValue(SolrWrapper.recordID_field);
		if(tmp !=null)
			recordID = tmp.toString();
		tmp= doc.getFieldValue(SolrWrapper.title_field);
		if(tmp !=null)
			title=tmp.toString();
		tmp=doc.getFieldValue(SolrWrapper.publisher_field);
		if(tmp !=null)
			publisher=tmp.toString();
		tmp=doc.getFieldValue(SolrWrapper.libraryID_field);
		if(tmp !=null)
			libraryID=tmp.toString();
		tmp=doc.getFieldValue(SolrWrapper.inferredURL_field);
		if(tmp !=null)
			URL=tmp.toString();
		tmp=doc.getFieldValue(SolrWrapper.identifierUrl_field);
		if(tmp !=null)
			URL=tmp.toString();
		tmp=doc.getFieldValue(SolrWrapper.ISBN_field);
		if(tmp !=null)
			ISBN=tmp.toString();
		tmp=doc.getFieldValue(SolrWrapper.ISSN_field);
		if(tmp !=null)
			ISSN=tmp.toString();
		tmp=doc.getFieldValue(SolrWrapper.telRecordID_URL_field);
		if(tmp !=null)
			telrecordID_URL=tmp.toString();
		tmp=doc.getFieldValue(SolrWrapper.identifier_field);
		if(tmp !=null)
			identifier=tmp.toString();
		tmp=doc.getFieldValue(SolrWrapper.creator_field);
		tmp=doc.getFieldValue(SolrWrapper.type_field);
		if(tmp !=null)
			type=tmp.toString();
		tmp=doc.getFieldValue(SolrWrapper.score_field);
		if(tmp !=null)
			score=tmp.toString();
		
		Collection<Object> tmps =  doc.getFieldValues(SolrWrapper.tableOfContents_field);
		HashSet<String> tableOfContents=new HashSet<String>();
		if(tmps != null){
			for(Object obj: tmps){
				tableOfContents.add(obj.toString());
			}
		}
		tmps =  doc.getFieldValues(SolrWrapper.subject_field);
		HashSet<String> subjects=new HashSet<String>();
		if(tmps!=null){
			for(Object obj: tmps){
				subjects.add(obj.toString());
			}
		}
		tmps =  doc.getFieldValues(SolrWrapper.description_field);
		HashSet<String> descriptions=new HashSet<String>();
		if(tmps!=null){
			for(Object obj: tmps){
				descriptions.add(obj.toString());
			}
		}
		tmps =  doc.getFieldValues(SolrWrapper.lang_field);
		HashSet<String> languages=new HashSet<String>();
		if(tmps!=null){
			for(Object obj: tmps){
				languages.add(obj.toString());
			}
		}
		tmps =  doc.getFieldValues(SolrWrapper.oaiSet_field);
		HashSet<String> oaiSets=new HashSet<String>();
		if(tmps!=null){
			for(Object obj: tmps){
				oaiSets.add(obj.toString());
			}
		}
		tmps =  doc.getFieldValues(SolrWrapper.contributor_field);
		HashSet<String> contributors=new HashSet<String>();
		if(tmps!=null){
			for(Object obj: tmps){
				contributors.add(obj.toString());
			}
		}
		
		tmps =  doc.getFieldValues(SolrWrapper.creator_field);
		HashSet<String> creators=new HashSet<String>();
		if(tmps!=null){
			for(Object obj: tmps){
				creators.add(obj.toString());
			}
		}
		
		DocsSearchResult sr = new DocsSearchResult( score,  libraryID, title,  publisher,  URL,  ISSN,  ISBN,  telrecordID_URL, 
				 identifier,  recordID, type,  subjects,  oaiSets, descriptions,  tableOfContents, contributors, languages, creators);
		
		logger.debug("-"+sr.score+" "+sr.libraryID_Elm+" "+sr.title);
		return sr;
	}


	public String getISSN() {
		return ISSN;
	}


	public String getISBN() {
		return ISBN;
	}


	public String getTelRecordID_URL() {
		return telRecordID_URL;
	}


	public String getType() {
		return type;
	}


	public HashSet<String> getOaiSets() {
		return oaiSets;
	}


	public HashSet<String> getDescriptions() {
		return descriptions;
	}


	public HashSet<String> getTableOfContents() {
		return tableOfContents;
	}


	public HashSet<String> getLanguages() {
		return languages;
	}


	public String getIdentifier() {
		return identifier;
	}


	public HashSet<String> getContributors() {
		return contributors;
	}


	public HashSet<String> getCreators() {
		return creators;
	}


	public String getLibraryID() {
		return libraryID;
	}
}
