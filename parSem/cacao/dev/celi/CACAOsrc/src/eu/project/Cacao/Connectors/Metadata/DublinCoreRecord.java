package eu.project.Cacao.Connectors.Metadata;

import java.io.StringReader;
import java.io.UnsupportedEncodingException;
import java.util.Hashtable;
import java.util.Vector;

import org.xml.sax.Attributes;
import org.xml.sax.InputSource;
import org.xml.sax.SAXException;
import org.xml.sax.XMLReader;
import org.xml.sax.helpers.DefaultHandler;
import org.xml.sax.helpers.XMLReaderFactory;


/**
 *
 * Class for parsing a single DublinCore record
 *
 * @author Igor Barsanti - <a href="http://www.gonetwork.it" target="_blank">Gonetwork Srl</a>
 *
 */
public class DublinCoreRecord extends DefaultHandler {
    private String elementValue;
    private String DublinCoreData;
    private String recordID;
    private Vector<String> title = new Vector<String>();
    private Vector<String> creator = new Vector<String>();
    private Vector<String> subject = new Vector<String>();
    private Vector<Hashtable<String, String>> subjectAtts = new Vector<Hashtable<String, String>>();
    private Vector<String> description = new Vector<String>();
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

    /**
     * Class constructor
     *
     * @param data        DublinCore record string
     *
     * @throws DCXmlException        in case of XML error in the record
     */
    public DublinCoreRecord(String record_ID, String data)	throws DCXmlException {
		DublinCoreData = data;
		recordID = record_ID;

		try {
			XMLReader parser = XMLReaderFactory.createXMLReader();
			parser.setContentHandler(this);

			parser.parse(new InputSource(new StringReader(DublinCoreData)));
		} catch (SAXException e) {
			throw (new DCXmlException("XML Error"));
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

                if ("".equals(aName)) {
                    aName = atts.getQName(i);
                }

                hash.put(aName, atts.getValue(i));
            }
        }

        if (qualifiedName.equalsIgnoreCase("dc:subject")) {
            subjectAtts.add(hash);
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
        }
    }

    /*
     * (non-Javadoc)
     *
     * @see org.xml.sax.helpers.DefaultHandler#characters(char[], int, int)
     */
    public void characters(char[] ch, int start, int length) {
        for (int i = start; i < (start + length); i++) {
            elementValue += ch[i];
        }
    }

    /**
     * Return a Vector with all Title fields of DublinCore record
     *
     * @return titles
     */
    public Vector<String> getTitle() {
        return title;
    }

    /**
     * Return a Vector with all Creator fields of DublinCore record
     *
     * @return creators
     */
    public Vector<String> getCreator() {
        return creator;
    }

    /**
     * Return a Vector with all Subject fields of DublinCore record
     *
     * @return subjects
     */
    public Vector<String> getSubject() {
        return subject;
    }

    /**
     * Return a Vector with all description fields of DublinCore record
     *
     * @return descriptions
     */
    public Vector<String> getDescription() {
        return description;
    }

    /**
     * Return a Vector with all Publisher fields of DublinCore record
     *
     * @return publishers
     */
    public Vector<String> getPublisher() {
        return publisher;
    }

    /**
     * Return a Vector with all Contributor fields of DublinCore record
     *
     * @return contributors
     */
    public Vector<String> getContributor() {
        return contributor;
    }

    /**
     * Return a Vector with all Date fields of DublinCore record
     *
     * @return Dates
     */
    public Vector<String> getDate() {
        return date;
    }

    /**
     * Return a Vector with all Type fields of DublinCore record
     *
     * @return types
     */
    public Vector<String> getType() {
        return type;
    }

    /**
     * Return a Vector with all Format fields of DublinCore record
     *
     * @return formats
     */
    public Vector<String> getFormat() {
        return format;
    }

    /**
     * Return a Vector with all Identifiers fields of DublinCore record
     *
     * @return identifiers
     */
    public Vector<String> getIdentifier() {
        return identifier;
    }

    /**
     * Return a Vector with all Source fields of DublinCore record
     *
     * @return Sources
     */
    public Vector<String> getSource() {
        return source;
    }

    /**
     * Return a Vector with all Language fields of DublinCore record
     *
     * @return Languages
     */
    public Vector<String> getLanguage() {
        return language;
    }

    /**
     * Return a Vector with all Relation fields of DublinCore record
     *
     * @return Relations
     */
    public Vector<String> getRelation() {
        return relation;
    }

    /**
     * Return a Vector with all Coverage fields of DublinCore record
     *
     * @return Coverages
     */
    public Vector<String> getCoverage() {
        return coverage;
    }

    /**
     * Return a Vector with all Rights fields of DublinCore record
     *
     * @return Rights
     */
    public Vector<String> getRights() {
        return rights;
    }

    /**
     *
     * Return a String with DublinCore data in XML format
     *
     * @return DublinCore Data
     */
    public String getDublinCoreString() {
        try {
			return new String(DublinCoreData.getBytes("UTF-8"),"UTF-8");
		} catch (UnsupportedEncodingException e) {
			return DublinCoreData;
		}
    }

    /**
     * Return an Hashtable with Subject
     * @return the subjectAtts
     */
    public Vector<Hashtable<String, String>> getSubjectAtts() {
        return subjectAtts;
    }

    public String getRecordID() {
        return recordID;
    }
}
