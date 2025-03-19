package eu.project.Cacao.WS_Clients;

import java.io.ByteArrayInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStreamWriter;
import java.rmi.RemoteException;
import java.util.Random;
import java.util.Vector;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;

import org.apache.axis2.AxisFault;
import org.apache.log4j.Logger;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.NodeList;

import com.sun.org.apache.xml.internal.serialize.OutputFormat;
import com.sun.org.apache.xml.internal.serialize.XMLSerializer;

import eu.project.Cacao.Components.Utils.NLPtoken;
import eu.project.Cacao.WS_Clients.CACAO_nlpWS_stub.ParsePhrase;
import eu.project.Cacao.WS_Clients.CACAO_nlpWS_stub.ParsePhraseResponse;
import eu.project.Cacao.WS_Clients.CACAO_nlpWS_stub.ParseTerm;
import eu.project.Cacao.WS_Clients.CACAO_nlpWS_stub.ParseTermResponse;

public class CACAO_nlpWS_client {
	final public static String xipResponseRootTagname = "NLPanalysis";
	final public static String xipTokenTagname = "nlpToken";

	private Logger logger = Logger.getLogger(CACAO_nlpWS_client.class);

	private CACAO_nlpWS_stub cl;

	public CACAO_nlpWS_client(String endpoint) throws AxisFault {
		this.cl = new CACAO_nlpWS_stub(endpoint);
	}

	public CACAO_nlpWS_client() throws AxisFault {
		this.cl = new CACAO_nlpWS_stub();
	}

	public Vector<NLPtoken> parsePhrase(String text, String lang) {
		try {
			ParsePhrase input = new ParsePhrase();
			input.setText(text);
			input.setLang(lang);

			ParsePhraseResponse rsp = this.cl.parsePhrase(input);
			return CACAO_nlpWS_client.parseServiceResponseMsg(rsp.get_return());
		} catch (RemoteException e) {
			logger.error("Error analyzing phrase " + text + " in " + lang + " lang.\n" + e.getMessage());
			return null;
		}
	}

	public Vector<NLPtoken> parseTerm(String text, String lang) {
		try {
			ParseTerm input = new ParseTerm();
			input.setText(text);
			input.setLang(lang);

			ParseTermResponse rsp = this.cl.parseTerm(input);
			return parseServiceResponseMsg(rsp.get_return());
		} catch (RemoteException e) {
			logger.error("Error analyzing term " + text + " in " + lang + " lang.\n" + e.getMessage());
			return null;
		}
	}
	
	static private void serializeResponseDoc2File(Document doc) {
		try {
			Random r=new Random();
			OutputStreamWriter tmp = new OutputStreamWriter(new FileOutputStream ("prova_"+r.nextInt(100)+".xml"),"UTF-8");
			XMLSerializer serializer = new XMLSerializer(tmp, new OutputFormat(doc, "UTF-8", true));
			serializer.serialize(doc);
			tmp.close();
			return ;
		} catch (IOException e) {
			//logger.error("Error serializing XML response document to a string.\n" + e.getMessage());
			return ;
		}
	}
	
	static public Vector<NLPtoken> parseServiceResponseMsg(String msg) {
		Logger logger = Logger.getLogger(CACAO_nlpWS_client.class);
		Vector<NLPtoken> results = new Vector<NLPtoken>();
		
		logger.debug(msg);
		
		try {
			// getting the document builder Factory
			DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();

			// if the document has to be validated against the schema source

			// getting the document builder from the Factory
			DocumentBuilder builder = factory.newDocumentBuilder();

			// parses the xml
			InputStream is = new ByteArrayInputStream(msg.getBytes("UTF-8"));
			Document myDocument = builder.parse(is, "UTF-8");
			//serializeResponseDoc2File(myDocument);
			NodeList nl = myDocument.getElementsByTagName(CACAO_nlpWS_client.xipTokenTagname);

			for (int i = 0; i < nl.getLength(); i++) {
				Element token = (Element) nl.item(i);
				NLPtoken tok = NLPtoken.createFromXMLElement(token);
				results.add(tok);
			}

			return results;
		} catch (Exception e) {
			logger.error("Error parsing the message : \n" + msg + "\n" + e);

			return null;
		}
	}


	public static void main(String args[]) throws RemoteException {
		   //System.err.close();
		
		   CACAO_nlpWS_client cl=new CACAO_nlpWS_client("http://hermes.celi.it:8080/axis2/services/CACAO_nlpWS");
		   
		   
		   Vector<NLPtoken> list=cl.parsePhrase("è", "it");
		   for(NLPtoken t:list)
		           System.out.println(t);
		   
		   list=cl.parseTerm("Qualität", "de");
		   for(NLPtoken t:list)
	           System.out.println(t);
	           
		   list=cl.parsePhrase("zaspokajania", "pl");
		   for(NLPtoken t:list)
	           System.out.println(t);
	}
	
	
	 
}
