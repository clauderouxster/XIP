/**
 *
 */
package eu.project.Cacao.Connectors.Clef;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.io.StringReader;
import java.util.Vector;

import org.xml.sax.Attributes;
import org.xml.sax.InputSource;
import org.xml.sax.SAXException;
import org.xml.sax.XMLReader;
import org.xml.sax.helpers.DefaultHandler;
import org.xml.sax.helpers.XMLReaderFactory;

import eu.project.Cacao.Connectors.Metadata.DublinCoreRecord;


/**
 *
 * Class for parsing CLEF data files
 *
 * @author Igor Barsanti - <a href="http://www.gonetwork.it" target="_blank">Gonetwork Srl</a>
 *
 */
public class ClefData extends DefaultHandler {
    private String elementValue;
    private boolean DCRecord = false;
    private String DCRecordData;
    private String recordID;
    private Vector<DublinCoreRecord> records = new Vector<DublinCoreRecord>();
    private Vector<String> badRecords = new Vector<String>();
    private int errors = 0;

    /**
     *
     * Class constructor, parse a single file
     *
     * @param path XML File path
     * @param file        XML file to parse
     *
     * @throws IOException        in case of file IO error
     */
    public ClefData(String file) throws IOException {
        try {
            parseFile(file);
        } catch (IOException e) {
            throw (e);
        }
    }

    /**
     * Parse specified file and extract DublinCoreData
     *
     * @param file XML file to parse
     */
    private void parseFile(String file) throws IOException {
        try {
            XMLReader parser = XMLReaderFactory.createXMLReader();

            File xmlFile = new File(file);
            BufferedReader xmlFileReader = new BufferedReader(new FileReader(
                        xmlFile));
            String xmlData = "";

            while (xmlFileReader.ready()) {
                char[] cbuf = new char[1048576];

                xmlFileReader.read(cbuf);

                String buffer = new String(cbuf);

                buffer = buffer.replace("<<", "");
                buffer = buffer.replace(">>", "");
                buffer = buffer.replace("&lt;", "");
                buffer = buffer.replace("&gt;", "");

                xmlData += buffer;
            }

            parser.setContentHandler(this);

            parser.parse(new InputSource(new StringReader(xmlData)));
        } catch (SAXException e) {
        } catch (IOException e) {
            throw new IOException("File error");
        }
    }

    /*
     * (non-Javadoc)
     *
     * @see eu.project.Cacao.Connectors.OAIPMH.GenericRequest#startElement(java.lang.String,
     *      java.lang.String, java.lang.String, org.xml.sax.Attributes)
     */
    public void startElement(String namespaceURI, String localName,
        String qualifiedName, Attributes atts) throws SAXException {
        elementValue = "";

        if (qualifiedName.equalsIgnoreCase("oai_dc:dc")) {
            DCRecord = true;
            DCRecordData = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" +
                "<CLEF xmlns:xlink=\"http://www.w3.org/1999/xlink\" " +
                "xmlns:mods=\"http://www.loc.gov/mods\" " +
                "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " +
                "xmlns:tel=\"http://krait.kb.nl/coop/tel/handbook/telterms.html\" " +
                "xmlns:dcmitype=\"http://purl.org/dc/dcmitype/\" " +
                "xmlns:oai_dc=\"http://www.openarchives.org/OAI/2.0/oai_dc/\" " +
                "xmlns:dcterms=\"http://purl.org/dc/terms/\" " +
                "xmlns:dc=\"http://purl.org/dc/elements/1.1/\">\n";
        }

        if (DCRecord) {
            DCRecordData += ("<" + qualifiedName);

            if (atts != null) {
                for (int i = 0; i < atts.getLength(); i++) {
                    String aName = atts.getLocalName(i);

                    if ("".equals(aName)) {
                        aName = atts.getQName(i);
                    }

                    DCRecordData += (" " + aName + "=\"" +
                    atts.getValue(i).replace("&", "&amp;") + "\"");
                }
            }

            DCRecordData += ">";
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
        if (DCRecord) {
            DCRecordData += (elementValue.replaceAll("&", "&amp;") + "</" +
            qualifiedName + ">\n");
        } else if (qualifiedName.equalsIgnoreCase("id")) {
            recordID = this.elementValue;
        }

        if (qualifiedName.equalsIgnoreCase("oai_dc:dc")) {
            DCRecordData += "</CLEF>\n";
            DCRecord = false;

            try {
                records.add(new DublinCoreRecord(recordID, DCRecordData));
            } catch (Exception e) {
                errors++;
                badRecords.add(DCRecordData);
            }
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
     * Return a Vector of DublinCoreRecord objects with all records retrieved
     *
     * @return the records
     */
    public Vector<DublinCoreRecord> getRecords() {
        return records;
    }

    /**
     * Return a Vector with all records with XML error
     *
     * @return Records with error
     */
    public Vector<String> getBadRecords() {
        return badRecords;
    }

    /**
     * Return count of errors
     *
     * @return errors
     */
    public int getErrors() {
        return errors;
    }

    public String getRecordID() {
        return recordID;
    }
}
