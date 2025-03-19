package eu.project.Cacao.Components;

import it.celi.names.corpus.vote.Hit;
import it.celi.names.corpus.vote.QueryVoteRunner;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.util.HashSet;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.Vector;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;

import org.apache.log4j.Logger;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.NodeList;

import com.sun.org.apache.xml.internal.serialize.OutputFormat;
import com.sun.org.apache.xml.internal.serialize.XMLSerializer;

import eu.project.Cacao.Components.Utils.NLPtoken;
import eu.project.Cacao.Managers.QueryManager;

public class NamedEntitiesDetector {
	// Tag & attributes names used in the xml configuration file
	public final static String deciderTagname = "decider", voterTagName = "voter", classNameAttr = "className", initParamAttr = "initParam", extToAnalyzeAttr = "extToAnalyze",
			thresholdAttr="threshold", scoreAttr="score";

	private static Logger logger = Logger.getLogger(NamedEntitiesDetector.class);

	private QueryVoteRunner vr;

	public NamedEntitiesDetector(Element initData) {
		this.vr = new QueryVoteRunner();
		initFromConfiguration(initData);
	}
	
	public NamedEntitiesDetector(String xmlConfigurationFile) {
		this.vr = new QueryVoteRunner();
		try {
        	logger.debug("Init Query Manager from "+xmlConfigurationFile);
        	
            DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
            DocumentBuilder builder = factory.newDocumentBuilder();
            Document myDocument = builder.parse(new File(xmlConfigurationFile));
            Element neDetector = (Element) myDocument.getElementsByTagName(QueryManager.neDetectorTag).item(0);
            initFromConfiguration(neDetector);
		} catch (Exception e) {
	        logger.error("Error initializing NE detector component from "+xmlConfigurationFile+".\n" +  e.getMessage());
	    }
	}

	public Vector<NLPtoken> detectNE(Vector<NLPtoken> queryAnalysis, String lang) {
		
		if(queryAnalysis==null || queryAnalysis.size()==0)
			return new Vector<NLPtoken>();
		
		StringBuffer bf = new StringBuffer(" ");
		for (NLPtoken t : queryAnalysis) {
			if(!t.isNE)
				bf = bf.append(t.surface + " ");
		}
		Hashtable<String, Float> neFound = this.analyzeString(bf.toString(), lang);
		for (String ne : neFound.keySet()) {
			Vector<NLPtoken> components = new Vector<NLPtoken>();
			boolean finished = false;
			int marker = -1, cnt = 0;

			for (Iterator<NLPtoken> it = queryAnalysis.iterator(); it.hasNext() && !finished; cnt++) {
				NLPtoken t = it.next();
				if (ne.startsWith(t.surface)) {
					components.add(t);
					it.remove();
					marker = cnt;
				} else if (ne.endsWith(t.surface)) {
					components.add(t);
					it.remove();
					finished = true;
				} else if (ne.contains(t.surface)) {
					components.add(t);
					it.remove();
				}

			}

			if (components.size() == 1) {
				queryAnalysis.add(marker, components.firstElement());
			} else {
				String surface = "", lemma = "", POS = "NOUN";
				for (NLPtoken c : components) {
					surface += c.surface + " ";
					lemma += c.getLemma_Category() + " ";
				}
				NLPtoken newT = new NLPtoken(surface.trim(), POS, lemma.trim(), "PERSON", lang);
				queryAnalysis.add(marker, newT);
			}

		}
		return queryAnalysis;
	}

	private Hashtable<String, Float> analyzeString(String text, String lang) {
		Hashtable<String, Float> results = new Hashtable<String, Float>();
		Hit[] hits = vr.extractNames(text, lang);
		logger.debug("In text: " + text + " found following NE..");
		for (int j = 0; j < hits.length; j++) {
			String match = text.substring(hits[j].getStart(), hits[j].getEnd());
			float score = (float) hits[j].getScore();
			logger.debug("\t- " + match + " " + score + " " + hits[j].getMainVote().toString());
			results.put(match, score);
		}
		return results;
	}

	private boolean initFromConfiguration(Element neDetectorElement) {
		try {
			logger.debug("Init NE detector Component from " + neDetectorElement);
			this.vr.init("xml");

			Element decider = (Element) neDetectorElement.getElementsByTagName(NamedEntitiesDetector.deciderTagname).item(0);
			String deciderClassName = decider.getAttribute(NamedEntitiesDetector.classNameAttr);
			String tmp=decider.getAttribute(NamedEntitiesDetector.thresholdAttr);
			if(tmp!=null && tmp.length()>0){
				double d=Double.parseDouble(tmp);
				this.vr.initDecider(deciderClassName, d);
			}
			else	
				this.vr.initDecider(deciderClassName);

			NodeList nl = neDetectorElement.getElementsByTagName(NamedEntitiesDetector.voterTagName);
			for (int i = 0; i < nl.getLength(); i++) {
					Element elm = (Element) nl.item(i);
					String className = elm.getAttribute(NamedEntitiesDetector.classNameAttr);
					String initParam = elm.getAttribute(NamedEntitiesDetector.initParamAttr);
				try {
					tmp = elm.getAttribute(NamedEntitiesDetector.scoreAttr);
					if(tmp!=null && tmp.length()>0){
						double d=Double.parseDouble(tmp);
						this.vr.addVoter(className, initParam, d);
					}
					else
						this.vr.addVoter(className, initParam);
				} catch (Exception e) {
					logger.error("Error initializing voter for NE detector component: " + className + " " + initParam + ".\n" + e.getMessage());
				}
			}

			return true;
		} catch (Exception e) {
			logger.error("Error initializing NE detector component.\n" + e.getMessage());

			return false;
		}
	}

	public static void createXMLfile(String fname) {
		try {
			String[] tmps={"itNames","frNames","enNames","deNames","plNames"};
			
			for(String s: tmps){
				HashSet<String> names = new HashSet<String>(1000);
				BufferedReader br0 = new BufferedReader(new FileReader("stuff/NeDetect/"+s));
				while (br0.ready()) {
					names.add(br0.readLine().trim());
				}
			}

			BufferedReader br = new BufferedReader(new FileReader(fname));
			// create the root element and append it to the XML document
			DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();
			DocumentBuilder db = dbf.newDocumentBuilder();
			Document doc = db.newDocument();
			Element rootElm = doc.createElement("fullNames");
			doc.appendChild(rootElm);

			while (br.ready()) {
				String str = new String(br.readLine());
				Element elm = doc.createElement("name");
				rootElm.appendChild(elm);
				Element elmC = doc.createElement("cognome");
				elmC.setAttribute("value", str.trim());
				elmC.setTextContent(str.trim());
				elm.appendChild(elmC);
			}

			FileWriter tmp = new FileWriter(fname + ".xml");
			XMLSerializer serializer = new XMLSerializer(tmp, new OutputFormat(doc, "UTF-8", true));
			serializer.serialize(doc);

		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	static public void main(String[] args) {
		createXMLfile("stuff/NeDetect/PERS.txt");
	}
}
