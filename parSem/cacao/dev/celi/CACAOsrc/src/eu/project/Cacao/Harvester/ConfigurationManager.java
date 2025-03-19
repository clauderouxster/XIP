package eu.project.Cacao.Harvester;

import org.xml.sax.Attributes;
import org.xml.sax.SAXException;
import org.xml.sax.XMLReader;
import org.xml.sax.helpers.DefaultHandler;
import org.xml.sax.helpers.XMLReaderFactory;

/**
 * @author Igor Barsanti - <a href="http://www.gonetwork.it" target="_blank">Gonetwork Srl</a>
 *
 */
public class ConfigurationManager extends DefaultHandler {
	
	private static ConfigurationManager instance = null;
	
	private String configurationFile;
	private String elementValue;
	private String module;
	private String[][] params = new String[100][7];
	private int paramIndex = 0;
	private String[][] agents = new String[100][10];
	private int agentIndex = 0;
	private String agentIdentifier;
	private String agentType;
	private String agentTargetUrl;
	private String agentActive;
	private String agentStart;
	private String agentLimit;
	private String agentLimitType;
	private String agentMetadataPrefix;
	private String agentFrom;
	private String agentTo;
	private int parseLevel = -1;
	private String[] parsePath = new String[20];
	
	/**
	 * 
	 */
	protected ConfigurationManager() {}
	
	 public static ConfigurationManager getInstance() {
		 if(instance == null) {
			 instance = new ConfigurationManager();
	     }
	     return instance;
	 }
	
	/**
	 * @param configurationFile
	 */
	public ConfigurationManager(String configurationFile) {
		super();
		this.configurationFile = configurationFile;
	}

	public String getParam(String module, String parameter) {
		for (int idx = 0; idx < paramIndex; idx++) {
			if (params[idx][0].equalsIgnoreCase(module) &&
				params[idx][1].equalsIgnoreCase(parameter))
					return params[idx][2];				
		}
		
		return null;
	}

	public String[] getAgents() {
		String[] agentList = new String[agentIndex];
		
		for (int idx = 0; idx < agentIndex; idx++) {
			agentList[idx] = agents[idx][0];											
		}
		
		return agentList;
	}
	
	public String getAgentType(String identifier) {
		for (int idx = 0; idx < agentIndex; idx++) {
			if (agents[idx][0].equalsIgnoreCase(identifier))				
					return agents[idx][1];				
		}
		
		return null;
	}
	
	public String getAgentTargetUrl(String identifier) {
		for (int idx = 0; idx < agentIndex; idx++) {
			if (agents[idx][0].equalsIgnoreCase(identifier))				
					return agents[idx][2];				
		}
		
		return null;
	}

	public String getAgentActive(String identifier) {
		for (int idx = 0; idx < agentIndex; idx++) {
			if (agents[idx][0].equalsIgnoreCase(identifier))				
					return agents[idx][3];				
		}
		
		return null;
	}
	
	public String getAgentLimit(String identifier) {
		for (int idx = 0; idx < agentIndex; idx++) {
			if (agents[idx][0].equalsIgnoreCase(identifier))				
					return agents[idx][4];				
		}
		
		return null;
	}
	
	public String getAgentLimitType(String identifier) {
		for (int idx = 0; idx < agentIndex; idx++) {
			if (agents[idx][0].equalsIgnoreCase(identifier))				
					return agents[idx][7];				
		}
		
		return null;
	}

	public String getAgentMetadataPrefix(String identifier) {
		for (int idx = 0; idx < agentIndex; idx++) {
			if (agents[idx][0].equalsIgnoreCase(identifier))				
					return agents[idx][5];				
		}
		
		return null;
	}

	public String getAgentStart(String identifier) {
		for (int idx = 0; idx < agentIndex; idx++) {
			if (agents[idx][0].equalsIgnoreCase(identifier))				
					return agents[idx][6];				
		}
		
		return null;
	}

	public String getAgentFrom(String identifier) {
		for (int idx = 0; idx < agentIndex; idx++) {
			if (agents[idx][0].equalsIgnoreCase(identifier))				
					return agents[idx][8];				
		}
		
		return null;
	}

	public String getAgentTo(String identifier) {
		for (int idx = 0; idx < agentIndex; idx++) {
			if (agents[idx][0].equalsIgnoreCase(identifier))				
					return agents[idx][9];				
		}
		
		return null;
	}
	
	public boolean parse() {
		try {
			XMLReader parser = XMLReaderFactory.createXMLReader();
			parser.setContentHandler(this);
			parser.parse(this.getConfigurationFile());
		} catch (Exception e) {
			System.out.println (e.toString());
			return false;
		}
		return true;
	}

	/**
	 * @return the configurationFile
	 */
	public String getConfigurationFile() {
		return configurationFile;
	}

	/**
	 * @param configurationFile the configurationFile to set
	 */
	public void setConfigurationFile(String configurationFile) {
		this.configurationFile = configurationFile;
	}
	
	/*
	 * (non-Javadoc)
	 * 
	 * @see org.xml.sax.helpers.DefaultHandler#startElement(java.lang.String,
	 *      java.lang.String, java.lang.String, org.xml.sax.Attributes)
	 */
	public void startElement(String namespaceURI, String localName,
			String qualifiedName, Attributes atts) throws SAXException {

		parseLevel++;
		parsePath[parseLevel] = qualifiedName;
		
		elementValue = "";
		
		String path = dumpParsePath();
				
		if (path.equalsIgnoreCase("Harvester:WebMonitor")) {
			module = "WebMonitor";			
		}
		else if (path.equalsIgnoreCase("Harvester:Agent")) {
			module = "Agent";	
	        
			if(qualifiedName.equals("Limit")){
				if (atts != null) {
		            for (int i = 0; i < atts.getLength(); i++) {
		                if(atts.getLocalName(i).equals("type"))
		                	agentLimitType=atts.getValue(i);
	
		            }
		        }
			}
		}
		else if (path.equalsIgnoreCase("Harvester:DBEngine")) {
			module = "DBEngine";			
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

		
		String path = dumpParsePath();		
	
		if (parseLevel == 2) {
			if (module.equalsIgnoreCase("Agent")) {

				if (qualifiedName.equalsIgnoreCase("Identifier"))
					agentIdentifier = elementValue;
				
				if (qualifiedName.equalsIgnoreCase("Type"))
					agentType = elementValue;
				
				if (qualifiedName.equalsIgnoreCase("TargetUrl"))
					agentTargetUrl = elementValue;
				
				if (qualifiedName.equalsIgnoreCase("Active"))
					agentActive = elementValue;

				if (qualifiedName.equalsIgnoreCase("Start"))
					agentStart = elementValue;
				
				if (qualifiedName.equalsIgnoreCase("Limit"))
					agentLimit = elementValue;

				if (qualifiedName.equalsIgnoreCase("LimitType"))
					agentLimitType = elementValue;
				
				if (qualifiedName.equalsIgnoreCase("MetadataPrefix"))
					agentMetadataPrefix = elementValue;
				
				if (qualifiedName.equalsIgnoreCase("From"))
					agentFrom = elementValue;

				if (qualifiedName.equalsIgnoreCase("To"))
					agentTo = elementValue; 
				
				if (agentIdentifier != null & agentType != null & 
						agentTargetUrl != null & agentActive != null &
						agentLimit != null & agentMetadataPrefix != null &
						agentStart != null & agentFrom != null & 
						agentTo != null & agentLimitType != null) {
					agents[agentIndex][0] = agentIdentifier;
					agents[agentIndex][1] = agentType;
					agents[agentIndex][2] = agentTargetUrl;
					agents[agentIndex][3] = agentActive;
					agents[agentIndex][4] = agentLimit;
					agents[agentIndex][5] = agentMetadataPrefix;
					agents[agentIndex][6] = agentStart;
					agents[agentIndex][7] = agentLimitType;
					agents[agentIndex][8] = agentFrom;
					agents[agentIndex][9] = agentTo;
					
					agentIndex++;
					
					agentIdentifier = null;
					agentType = null;
					agentTargetUrl = null;
					agentActive = null;
					agentLimit = null;
					agentMetadataPrefix = null;
					agentStart = null;
					agentLimitType = null;
					agentFrom = null;
					agentTo = null;
				}
			}
			else {
				params[paramIndex][0] = module;
				params[paramIndex][1] = qualifiedName;
				params[paramIndex][2] = elementValue;
				paramIndex++;
			}
		}
	
		parseLevel--;
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

	private String dumpParsePath() {
		String res = "";
		for (int c = 0; c < parseLevel; c++) {
			res += ":" + parsePath[c];
		}
		
		if (res.length() > 0) res = res.substring(1);
		
		return res; 
	}
}
