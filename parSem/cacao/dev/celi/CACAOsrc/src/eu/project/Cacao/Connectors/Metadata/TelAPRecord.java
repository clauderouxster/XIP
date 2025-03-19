package eu.project.Cacao.Connectors.Metadata;

import java.io.StringReader;
import java.util.Vector;
import java.util.Hashtable;
import org.xml.sax.Attributes;
import org.xml.sax.InputSource;
import org.xml.sax.SAXException;
import org.xml.sax.XMLReader;
import org.xml.sax.helpers.DefaultHandler;
import org.xml.sax.helpers.XMLReaderFactory;

/**
 * 
 * Class for parsing a single TELAP record
 * 
 * @author Igor Barsanti - <a href="http://www.gonetwork.it"
 *         target="_blank">Gonetwork Srl</a>
 * 
 */
public class TelAPRecord extends DefaultHandler {

	private String elementValue;
	private String TelAPData;
    private String recordID;
	private Vector<String> title = new Vector<String>();
	private Vector<String> creator = new Vector<String>();
	private Vector<String> subject = new Vector<String>();
	private Vector<String> description = new Vector<String>();
	private Vector<String> tableOfContents = new Vector<String>();
	private Vector<String> publisher = new Vector<String>();
	private Vector<String> contributor = new Vector<String>();
	private Vector<String> date = new Vector<String>();
	private Vector<String> type = new Vector<String>();
	private Vector<String> format = new Vector<String>();
	private Vector<String> identifier = new Vector<String>();
	private Vector<String> source = new Vector<String>();
	private Vector<String> language = new Vector<String>();
	private Vector<String> relation = new Vector<String>();
	private Vector<String> coverage = new Vector<String>();
	private Vector<String> rights = new Vector<String>();
	private Vector<String> edition = new Vector<String>();
	private Vector<String> audience = new Vector<String>();
	private Vector<String> location = new Vector<String>();
	private Vector<String> telRecordId = new Vector<String>();
	
	private Vector<Hashtable<String, String>> titleAtts = new Vector<Hashtable<String, String>>();
	private Vector<Hashtable<String, String>> subjectAtts = new Vector<Hashtable<String, String>>();
	private Vector<Hashtable<String, String>> creatorAtts = new Vector<Hashtable<String, String>>();
	private Vector<Hashtable<String, String>> identifierAtts = new Vector<Hashtable<String, String>>();
	private Vector<Hashtable<String, String>> languageAtts = new Vector<Hashtable<String, String>>();
	private Vector<Hashtable<String, String>> tableOfContentsAtts = new Vector<Hashtable<String, String>>();
	private Vector<Hashtable<String, String>> descriptionAtts = new Vector<Hashtable<String, String>>();
	private Vector<Hashtable<String, String>> typeAtts = new Vector<Hashtable<String, String>>();
	private Vector<Hashtable<String, String>> dateAtts = new Vector<Hashtable<String, String>>();
	private Vector<Hashtable<String, String>> contributorAtts = new Vector<Hashtable<String, String>>();
	private Vector<Hashtable<String, String>> formatAtts = new Vector<Hashtable<String, String>>();
	private Vector<Hashtable<String, String>> sourceAtts = new Vector<Hashtable<String, String>>();
	private Vector<Hashtable<String, String>> relationAtts = new Vector<Hashtable<String, String>>();
	private Vector<Hashtable<String, String>> coverageAtts = new Vector<Hashtable<String, String>>();
	private Vector<Hashtable<String, String>> locationAtts = new Vector<Hashtable<String, String>>();
	private Vector<Hashtable<String, String>> telRecordIdAtts = new Vector<Hashtable<String, String>>();
	
	/**
	 * Class constructor
	 * 
	 * @param data
	 *            TelAP record string
	 * 
	 * @throws TELAPXmlException
	 *             in case of XML error in the record
	 */
	public TelAPRecord(String record_ID, String data) throws TELAPXmlException {
		TelAPData = data;
		recordID = record_ID;

		try {
			XMLReader parser = XMLReaderFactory.createXMLReader();
			parser.setContentHandler(this);

			parser.parse(new InputSource(new StringReader(TelAPData)));
		} catch (SAXException e) {
			throw (new TELAPXmlException("XML Error"));
		} catch (Exception e) {
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.xml.sax.helpers.DefaultHandler#startElement(java.lang.String,
	 *      java.lang.String, java.lang.String, org.xml.sax.Attributes)
	 */
	public void startElement(String namespaceURI, String localName,
			String qualifiedName, Attributes atts) throws SAXException {

		elementValue = "";

		Hashtable<String, String> hash = new Hashtable<String, String>();

		if (atts != null) {
			for (int i = 0; i < atts.getLength(); i++) {
				String aName = atts.getLocalName(i);
				if ("".equals(aName))
					aName = atts.getQName(i);
				hash.put(aName, atts.getValue(i));
			}
		}

		if (qualifiedName.equalsIgnoreCase("dc:title")) {
			titleAtts.add(hash);
		} else if (qualifiedName.equalsIgnoreCase("dc:creator")) {
			creatorAtts.add(hash);
		} else if (qualifiedName.equalsIgnoreCase("dc:subject")) {
			subjectAtts.add(hash);
		} else if (qualifiedName.equalsIgnoreCase("dc:description")) {
			descriptionAtts.add(hash);
		} else if (qualifiedName.equalsIgnoreCase("dc:contributor")) {
			contributorAtts.add(hash);
		} else if (qualifiedName.equalsIgnoreCase("dc:date")) {
			dateAtts.add(hash);
		} else if (qualifiedName.equalsIgnoreCase("dc:type")) {
			typeAtts.add(hash);
		} else if (qualifiedName.equalsIgnoreCase("dc:format")) {
			formatAtts.add(hash);
		} else if (qualifiedName.equalsIgnoreCase("dc:identifier")) {
			identifierAtts.add(hash);
		} else if (qualifiedName.equalsIgnoreCase("dcterms:tableOfContents")) {
			tableOfContentsAtts.add(hash);
		} else if (qualifiedName.equalsIgnoreCase("dc:source")) {
			sourceAtts.add(hash);
		} else if (qualifiedName.equalsIgnoreCase("dc:language")) {
			languageAtts.add(hash);
		} else if (qualifiedName.equalsIgnoreCase("dc:relation")) {
			relationAtts.add(hash);
		} else if (qualifiedName.equalsIgnoreCase("dc:coverage")) {
			coverageAtts.add(hash);
		} else if (qualifiedName.equalsIgnoreCase("tel:location")) {
			locationAtts.add(hash);
		} else if (qualifiedName.equalsIgnoreCase("tel:recordId")) {
			telRecordIdAtts.add(hash);
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.xml.sax.helpers.DefaultHandler#endElement(java.lang.String,
	 *      java.lang.String, java.lang.String)
	 */
	public void endElement(String namespaceURI, String localName,
			String qualifiedName) throws SAXException {

		if (qualifiedName.equalsIgnoreCase("dc:title")) {
			title.add(elementValue);
		} else if (qualifiedName.equalsIgnoreCase("dc:creator")) {
			creator.add(elementValue);
		} else if (qualifiedName.equalsIgnoreCase("dc:subject")) {
			subject.add(elementValue);
		} else if (qualifiedName.equalsIgnoreCase("dc:description")) {
			description.add(elementValue);
		} else if (qualifiedName.equalsIgnoreCase("dc:publisher")) {
			publisher.add(elementValue);
		} else if (qualifiedName.equalsIgnoreCase("dc:contributor")) {
			contributor.add(elementValue);
		} else if (qualifiedName.equalsIgnoreCase("dc:date")) {
			date.add(elementValue);
		} else if (qualifiedName.equalsIgnoreCase("dc:type")) {
			type.add(elementValue);
		} else if (qualifiedName.equalsIgnoreCase("dc:format")) {
			format.add(elementValue);
		} else if (qualifiedName.equalsIgnoreCase("dc:identifier")) {
			identifier.add(elementValue);
		} else if (qualifiedName.equalsIgnoreCase("dcterms:tableOfContents")) {
			tableOfContents.add(elementValue);
		} else if (qualifiedName.equalsIgnoreCase("dc:source")) {
			source.add(elementValue);
		} else if (qualifiedName.equalsIgnoreCase("dc:language")) {
			language.add(elementValue);
		} else if (qualifiedName.equalsIgnoreCase("dc:relation")) {
			relation.add(elementValue);
		} else if (qualifiedName.equalsIgnoreCase("dc:coverage")) {
			coverage.add(elementValue);
		} else if (qualifiedName.equalsIgnoreCase("dc:rights")) {
			rights.add(elementValue);
		} else if (qualifiedName.equalsIgnoreCase("tel:edition")) {
			edition.add(elementValue);
		} else if (qualifiedName.equalsIgnoreCase("tel:audience")) {
			audience.add(elementValue);
		} else if (qualifiedName.equalsIgnoreCase("tel:location")) {
			location.add(elementValue);
		} else if (qualifiedName.equalsIgnoreCase("tel:recordId")) {
			telRecordId.add(elementValue);
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.xml.sax.helpers.DefaultHandler#characters(char[], int, int)
	 */
	public void characters(char ch[], int start, int length) {
		for (int i = start; i < start + length; i++) {
			elementValue += ch[i];
		}
	}

	/**
	 * Return a Vector with all Title fields of TELAP record
	 * 
	 * @return titles
	 */
	public Vector<String> getTitle() {
		return title;
	}

	/**
	 * Return a Vector with all Creator fields of TELAP record
	 * 
	 * @return creators
	 */
	public Vector<String> getCreator() {
		return creator;
	}

	/**
	 * Return a Vector with all Subject fields of TELAP record
	 * 
	 * @return subjects
	 */
	public Vector<String> getSubject() {
		return subject;
	}

	/**
	 * Return a Vector with all description fields of TELAP record
	 * 
	 * @return descriptions
	 */
	public Vector<String> getDescription() {
		return description;
	}

	/**
	 * Return a Vector with all Publisher fields of TELAP record
	 * 
	 * @return publishers
	 */
	public Vector<String> getPublisher() {
		return publisher;
	}

	/**
	 * Return a Vector with all Contributor fields of TELAP record
	 * 
	 * @return contributors
	 */
	public Vector<String> getContributor() {
		return contributor;
	}

	/**
	 * Return a Vector with all Date fields of TELAP record
	 * 
	 * @return Dates
	 */
	public Vector<String> getDate() {
		return date;
	}

	/**
	 * Return a Vector with all Type fields of TELAP record
	 * 
	 * @return types
	 */
	public Vector<String> getType() {
		return type;
	}

	/**
	 * Return a Vector with all Format fields of TELAP record
	 * 
	 * @return formats
	 */
	public Vector<String> getFormat() {
		return format;
	}

	/**
	 * Return a Vector with all Identifiers fields of TELAP record
	 * 
	 * @return identifiers
	 */
	public Vector<String> getIdentifier() {
		return identifier;
	}

	/**
	 * Return a Vector with all Source fields of TELAP record
	 * 
	 * @return Sources
	 */
	public Vector<String> getSource() {
		return source;
	}

	/**
	 * Return a Vector with all Language fields of TELAP record
	 * 
	 * @return Languages
	 */
	public Vector<String> getLanguage() {
		return language;
	}

	/**
	 * Return a Vector with all Relation fields of TELAP record
	 * 
	 * @return Relations
	 */
	public Vector<String> getRelation() {
		return relation;
	}

	/**
	 * Return a Vector with all Coverage fields of TELAP record
	 * 
	 * @return Coverages
	 */
	public Vector<String> getCoverage() {
		return coverage;
	}

	/**
	 * Return a Vector with all Rights fields of TELAP record
	 * 
	 * @return Rights
	 */
	public Vector<String> getRights() {
		return rights;
	}

	/**
	 * Return a Vector with all Edition fields of TELAP record
	 * 
	 * @return Edition
	 */
	public Vector<String> getEdition() {
		return edition;
	}

	/**
	 * Return a Vector with all Audience fields of TELAP record
	 * 
	 * @return Audience
	 */
	public Vector<String> getAudience() {
		return audience;
	}

	/**
	 * Return a Vector with all Location fields of TELAP record
	 * 
	 * @return Location
	 */
	public Vector<String> getLocation() {
		return location;
	}

	/**
	 * Return a Vector with all recordId fields of TELAP record
	 * 
	 * @return recordeId
	 */
	public Vector<String> getRecordId() {
		return telRecordId;
	}

	/**
	 * 
	 * Return a String with TELAP data in XML format
	 * 
	 * @return TELAP Data
	 */
	public String getTELAPString() {
		return TelAPData;
	}

	/**
	 * Return an Hashtable with Subject
	 * 
	 * @return the subjectAtts
	 */
	public Vector<Hashtable<String, String>> getSubjectAtts() {
		return subjectAtts;
	}

	/**
	 * @return the recordID
	 */
	public String getRecordID() {
		return recordID;
	}

	public Vector<Hashtable<String, String>> getTitleAtts() {
		return titleAtts;
	}

	public void setTitleAtts(Vector<Hashtable<String, String>> titleAtts) {
		this.titleAtts = titleAtts;
	}

	public Vector<String> getTableOfContents() {
		return tableOfContents;
	}

	public Vector<String> getTelRecordId() {
		return telRecordId;
	}

	public Vector<Hashtable<String, String>> getCreatorAtts() {
		return creatorAtts;
	}

	public Vector<Hashtable<String, String>> getIdentifierAtts() {
		return identifierAtts;
	}

	public Vector<Hashtable<String, String>> getLanguageAtts() {
		return languageAtts;
	}

	public Vector<Hashtable<String, String>> getTableOfContentsAtts() {
		return tableOfContentsAtts;
	}

	public Vector<Hashtable<String, String>> getDescriptionAtts() {
		return descriptionAtts;
	}

	public Vector<Hashtable<String, String>> getTypeAtts() {
		return typeAtts;
	}

	public Vector<Hashtable<String, String>> getDateAtts() {
		return dateAtts;
	}

	public Vector<Hashtable<String, String>> getContributorAtts() {
		return contributorAtts;
	}

	public Vector<Hashtable<String, String>> getFormatAtts() {
		return formatAtts;
	}

	public Vector<Hashtable<String, String>> getSourceAtts() {
		return sourceAtts;
	}

	public Vector<Hashtable<String, String>> getRelationAtts() {
		return relationAtts;
	}

	public Vector<Hashtable<String, String>> getCoverageAtts() {
		return coverageAtts;
	}

	public Vector<Hashtable<String, String>> getLocationAtts() {
		return locationAtts;
	}

	public Vector<Hashtable<String, String>> getTelRecordIdAtts() {
		return telRecordIdAtts;
	}

}
