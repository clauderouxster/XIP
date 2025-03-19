/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package JTextEntailment;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.Arrays;
import javax.xml.stream.XMLInputFactory;
import javax.xml.stream.XMLStreamException;
import javax.xml.stream.XMLStreamReader;

/**
 *
 * @author sam
 */
public class XmlReader {

    private ArrayList<String> significantTagNames =
            new ArrayList<String>(Arrays.asList("pair", "t", "h"));
    final private String pairTagName = "pair";
    final private String textTagName = "t";
    final private String hypotheseTagName = "h";
    private XMLStreamReader xmlStream;

    public XmlReader(File inputfile) throws FileNotFoundException, XMLStreamException {
        XMLInputFactory inputFactory = XMLInputFactory.newInstance();

        InputStream input = new FileInputStream(inputfile);
        xmlStream = inputFactory.createXMLStreamReader(input);
    //xmlStream = inputFactory.createFilteredReader(xmlStream, filter);
    }

    public Pair readNextPair() {
        try {
//        	String text = "";
//        	String hypothese = "";
            this.skipToSignificantElement();
            if (!inStartPair()) {
                return null;
            }
            Pair pair = new Pair(); 
            pair.setId(xmlStream.getAttributeValue(null, "id"));
            pair.setEntailment(xmlStream.getAttributeValue(null, "entailment"));
            pair.setTask(xmlStream.getAttributeValue(null, "task"));
            
//            String verif = xmlStream.getElementText();
            xmlStream.next();
            // Read text
//            String test = xmlStream.getLocalName();
            this.skipToSignificantElement();
//             test = xmlStream.getLocalName();
            if (inStartText()) {
//            	 test = xmlStream.getLocalName();
            	pair.setText(xmlStream.getElementText());
            	long lengthText = (long) pair.getText().length() + 1;
            	pair.setHBeginOffset(lengthText);
            }
            xmlStream.next();
            this.skipToSignificantElement();
//            xmlStream.next();
            if (inStartHypothese()) {
            	pair.setHypothese(xmlStream.getElementText());
            }
            xmlStream.next();
            
            
            return pair;
        } catch (XMLStreamException ex) {
            return null;
        }
    }

    private void skipToSignificantElement() throws XMLStreamException {
        while (!(xmlStream.isStartElement() &&
                this.significantTagNames.contains(xmlStream.getLocalName())) &&
                xmlStream.hasNext()) {
            xmlStream.next();
        }
    }

    private void skipToNextPairElement() throws XMLStreamException {
        while (xmlStream.hasNext() && !inStartPair()) {
            skipToSignificantElement();
        }
    }

    public boolean hasNextPair() throws XMLStreamException {
    	this.skipToSignificantElement();
        if (xmlStream.hasNext()) {
            return true;
        }else{
        	return false;
        }
    }

    private boolean inStartPair() {
        return (xmlStream.isStartElement() && xmlStream.getLocalName().equals(pairTagName));
    }

    private boolean inEndPair() {
        return (xmlStream.isEndElement() && xmlStream.getLocalName().equals(pairTagName));
    }

    private boolean inStartText() {
        return (xmlStream.isStartElement() && xmlStream.getLocalName().equals(textTagName));
    }

    private boolean inEndText() {
        return (xmlStream.isEndElement() && xmlStream.getLocalName().equals(textTagName));
    }

    private boolean inStartHypothese() {
        return (xmlStream.isStartElement() && xmlStream.getLocalName().equals(hypotheseTagName));
    }

    private boolean inEndHypothese() {
        return (xmlStream.isEndElement() && xmlStream.getLocalName().equals(hypotheseTagName));
    }

}
