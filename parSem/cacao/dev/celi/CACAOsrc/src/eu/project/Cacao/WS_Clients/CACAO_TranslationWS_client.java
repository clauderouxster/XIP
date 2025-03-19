package eu.project.Cacao.WS_Clients;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.StringWriter;
import java.rmi.RemoteException;
import java.util.Hashtable;
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
import eu.project.Cacao.WS_Clients.CACAO_TranslationWS_stub.SupportsIOMappingsResponse;
import eu.project.Cacao.WS_Clients.CACAO_TranslationWS_stub.SupportsMorphoAnalysisResponse;
import eu.project.Cacao.WS_Clients.CACAO_TranslationWS_stub.SupportsMultiWordsResponse;
import eu.project.Cacao.WS_Clients.CACAO_TranslationWS_stub.TranslatePhrase;
import eu.project.Cacao.WS_Clients.CACAO_TranslationWS_stub.TranslatePhraseResponse;
import eu.project.Cacao.WS_Clients.CACAO_TranslationWS_stub.TranslateTerm;
import eu.project.Cacao.WS_Clients.CACAO_TranslationWS_stub.TranslateTermResponse;
import eu.project.Cacao.WS_Clients.CACAO_TranslationWS_stub.TranslateTerms;
import eu.project.Cacao.WS_Clients.CACAO_TranslationWS_stub.TranslateTermsResponse;


public class CACAO_TranslationWS_client {
	
	public static final String rootElmTag="TSresponse", translationElementTag="translation", srcTermTag="src", 
	targetTermTag="target", confidenceAttr="weight", POSAttr="category", translationReqDocElm="TranslationRequest",
	langFromAttr="langFrom",	langToAttr="langTo";
	
    private CACAO_TranslationWS_stub myStub;
    protected Logger logger = Logger.getLogger(CACAO_TranslationWS_client.class.getName());

    public CACAO_TranslationWS_client(String serviceEndpoint)  throws AxisFault {
        this.myStub = new CACAO_TranslationWS_stub(serviceEndpoint);
    }

    public boolean supportsMultiWords() throws RemoteException {
        SupportsMultiWordsResponse response = myStub.supportsMultiWords();

        return response.get_return();
    }

    public boolean supportsMorphoAnalysis() throws RemoteException {
        SupportsMorphoAnalysisResponse response = myStub.supportsMorphoAnalysis();

        return response.get_return();
    }

    public boolean supportsIOMappings() throws RemoteException {
        SupportsIOMappingsResponse response = myStub.supportsIOMappings();

        return response.get_return();
    }

    public Hashtable<NLPtoken, NLPtoken[]> translatePhrase(String text, String langFrom, String langTo)
        throws RemoteException {
        TranslatePhrase input = new TranslatePhrase();
        input.setText(text);
        input.setLangFROM(langFrom);
        input.setLangTO(langTo);

        TranslatePhraseResponse resp = myStub.translatePhrase(input);
        this.myStub.cleanup();

        String msg = resp.get_return();
        return this.parseServiceResponseMsg(msg, true, langFrom);
    }
    
    private String createTranslateTermsRequestDoc(Vector<NLPtoken> tokens, String langFrom, String langTo){
    	DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();
		try {
			// get an instance of builder
			DocumentBuilder db = dbf.newDocumentBuilder();
			
			// create an instance of DOM
			Document responseDocument = db.newDocument();
			// create the root element  and append it to the XML document
			Element responseDocumentRootElm = responseDocument.createElement(CACAO_TranslationWS_client.translationReqDocElm);
			responseDocumentRootElm.setAttribute(CACAO_TranslationWS_client.langFromAttr, langFrom);
			responseDocumentRootElm.setAttribute(CACAO_TranslationWS_client.langToAttr, langTo);
			responseDocument.appendChild(responseDocumentRootElm);
			
			for(NLPtoken tok: tokens){
				Element termToBeTranslated = tok.toXMLElement(responseDocument, CACAO_TranslationWS_client.srcTermTag);
				responseDocumentRootElm.appendChild(termToBeTranslated);
			}
			
			return this.serializeResponseDoc2String(responseDocument);
		} catch (Exception e) {
			logger.error("Error creating Response Document\n"+e);
			return null;
		}
    }
    
    public Hashtable<NLPtoken, NLPtoken[]> translateTerms(Vector<NLPtoken> termsToTranslate, String langFrom, String langTo) throws RemoteException {
            String msg=this.createTranslateTermsRequestDoc(termsToTranslate, langFrom, langTo);
    	
    		TranslateTerms input = new TranslateTerms();
            input.setTermsTranslationRequest(msg);

            TranslateTermsResponse resp = myStub.translateTerms(input);
            this.myStub.cleanup();

            msg = resp.get_return();
            return this.parseServiceResponseMsg(msg, false, langFrom);
     }
    
    public Hashtable<NLPtoken, NLPtoken[]> translateTerm(String text, String POS, String langFrom,
        String langTo) throws RemoteException {
        TranslateTerm input = new TranslateTerm();
        input.setTerm(text);
        input.setLangFROM(langFrom);
        input.setLangTO(langTo);
        input.setPOS(POS);

        TranslateTermResponse resp = myStub.translateTerm(input);
        this.myStub.cleanup();

        String msg = resp.get_return();
        return this.parseServiceResponseMsg(msg, false, langFrom);
    }

    private Hashtable<NLPtoken, NLPtoken[]> parseServiceResponseMsg(String msg, boolean isPhraseTranslation, String langFrom) {
    	
        Hashtable<NLPtoken, NLPtoken[]> results = new Hashtable<NLPtoken, NLPtoken[]>();
        
        logger.debug(msg);
        
        try {
            // getting the document builder Factory
            DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();

            // if the document has to be validated against the schema source

            // getting the document builder from the Factory
            DocumentBuilder builder = factory.newDocumentBuilder();

            // parses the xml
            InputStream is = new ByteArrayInputStream(msg.getBytes("UTF-8"));
            Document myDocument = builder.parse(is,"UTF-8");
            NodeList nl = myDocument.getElementsByTagName(CACAO_TranslationWS_client.translationElementTag);

            for (int i = 0; i < nl.getLength(); i++) {
                Element translation = (Element) nl.item(i);
                Element src = (Element) translation.getElementsByTagName(CACAO_TranslationWS_client.srcTermTag).item(0);
                
                NLPtoken srcTok;
            	if(isPhraseTranslation){
                    String term = new String(src.getTextContent().getBytes("UTF-8"),"UTF-8");
                    srcTok = new NLPtoken(term,null,null,null,langFrom);
            	}
            	else
            		srcTok=NLPtoken.createFromXMLElement(src);
                NodeList nlTranslations = translation.getElementsByTagName(CACAO_TranslationWS_client.targetTermTag);
                NLPtoken[] trResults = new NLPtoken[nlTranslations.getLength()];

                for (int k = 0; k < nlTranslations.getLength(); k++) {
                    Element translElm = (Element) nlTranslations.item(k);
                    trResults[k] = NLPtoken.createFromXMLElement(translElm);
                }

                if ((srcTok != null) && (trResults != null) && (trResults.length > 0)) {

                    results.put(srcTok, trResults);
                }
            }

            return results;
        } catch (java.lang.Exception e) {
            logger.error("Error parsing the message : \n" + msg + "\n" +e);
            return null;
        }
    }
    
    /**
	 * Serializes the response xml document (internal field) to a String
	 * @return the serialized document as a String
	 */
	private String serializeResponseDoc2String(Document doc){
		try {
			StringWriter tmp=new StringWriter();
			XMLSerializer serializer = new XMLSerializer(tmp, new OutputFormat(doc,"UTF-8",true));
			serializer.serialize(doc);
			return tmp.toString();
		} catch (IOException e) {
			return null;
		}
	}
	
	static private Vector<NLPtoken> getTermsFromPhrase(String text, String lang, String endpoint) throws AxisFault{
		CACAO_nlpWS_client cl=new CACAO_nlpWS_client(endpoint);
		return cl.parsePhrase(text, lang);
	}
	
    public static void main(String[] args) throws java.lang.Exception {
        CACAO_TranslationWS_client a = new CACAO_TranslationWS_client(
                "http://hermes.celi.it:8080/axis2/services/CACAO_TranslationWS");

        Hashtable<NLPtoken, NLPtoken[]> result = a.translatePhrase("Roman law", "en", "pl");
        //using dictionaries we have the pairings srcToken -> translation candidates List 
        //otherwise we obtain srcText -> translatedText
        for (NLPtoken src : result.keySet()) {
            System.out.println(">> " + src);

            for (NLPtoken target : result.get(src)) {
                System.out.println("\t\t" + target);
            }
        }
        
        //if useDictionaries value is absent, the default value is true...
        result = a.translateTerm("Roman", null, "en", "de");
        for (NLPtoken src : result.keySet()) {
            System.out.println(">> " + src);

            for (NLPtoken target : result.get(src)) {
                System.out.println("\t\t" + target);
            }
        }
        
        Vector<NLPtoken> terms = CACAO_TranslationWS_client.getTermsFromPhrase("diritto agricoltura", "it","http://hermes.celi.it:8080/axis2/services/CACAO_nlpWS");
        result = a.translateTerms(terms, "it", "en");
        for (NLPtoken src : result.keySet()) {
            System.out.println(">> " + src);

            for (NLPtoken target : result.get(src)) {
                System.out.println("\t\t" + target);
            }
        }
    }
}
