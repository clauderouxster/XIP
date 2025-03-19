package eu.project.Cacao.WS_Clients;

import java.io.ByteArrayInputStream;
import java.io.InputStream;
import java.rmi.RemoteException;
import java.util.HashSet;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;

import org.apache.axis2.AxisFault;
import org.apache.log4j.Logger;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.NodeList;

import eu.project.Cacao.Components.Utils.NLPtoken;
import eu.project.Cacao.WS_Clients.CACAO_ThesauriWS_stub.GetRelatedTerms;
import eu.project.Cacao.WS_Clients.CACAO_ThesauriWS_stub.GetRelatedTermsResponse;

public class CACAO_ThesauriWS_client {
    final public static String wntResponseRootTagname = "ThesauriExpansionResponse", wntTokenTagname = "expansionToken";
    
    private static final String antonym="!", hypernym="@", hyponym="~", similar="&", holonym="%m,%s,%p", meronym="#m,#s,#p", 
	verbGroup="$", derivationallyRelated="+", verbEntailment="*", verbCause=">", classificationMembers="-", classification=";"; 
	
	
    private CACAO_ThesauriWS_stub myStub;
    protected Logger logger = Logger.getLogger(CACAO_ThesauriWS_client.class.getName());

    public CACAO_ThesauriWS_client(String serviceEndpoint)  throws AxisFault {
        this.myStub = new CACAO_ThesauriWS_stub(serviceEndpoint);
    }

    public HashSet<NLPtoken> getRelatedTerms(String lemma, String category, String relationType, String lang) throws RemoteException {
        GetRelatedTerms input = new GetRelatedTerms();
        input.setLemma(lemma);
        input.setCategory(category);
        input.setLang(lang);
        input.setRelationType(relationType);

        GetRelatedTermsResponse resp = myStub.getRelatedTerms(input);
        this.myStub.cleanup();

        String msg = resp.get_return();
        return this.parseServiceResponseMsg(msg);
    }

  
    private HashSet<NLPtoken> parseServiceResponseMsg(
        String msg) {
    	HashSet<NLPtoken> results = new HashSet<NLPtoken>();
        
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
            NodeList nl = myDocument.getElementsByTagName(CACAO_ThesauriWS_client.wntTokenTagname);

            for (int i = 0; i < nl.getLength(); i++) {
                Element expansionElm = (Element) nl.item(i);
                NLPtoken tok = NLPtoken.createFromXMLElement(expansionElm);
                if (tok != null) {
                    results.add(tok);
                }
            }

            return results;
        } catch (java.lang.Exception e) {
            System.out.println("Error parsing the message : \n" + msg + "\n" +  e);
            return null;
        }
    }

    public static void main(String[] args) throws java.lang.Exception {
    	CACAO_ThesauriWS_client a = new CACAO_ThesauriWS_client(
                "http://hermes.celi.it:8080/axis2/services/CACAO_ThesauriWS");

        HashSet<NLPtoken> list= a.getRelatedTerms("Roman", "NADJ", null, "en");
        for(NLPtoken t:list)
	           System.out.println(t);
    }
}
