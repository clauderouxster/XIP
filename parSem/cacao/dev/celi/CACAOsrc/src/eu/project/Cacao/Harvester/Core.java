/**
 * 
 */
package eu.project.Cacao.Harvester;

import org.apache.log4j.Logger;

import eu.project.Cacao.Components.Utils.RunTimer;
import eu.project.Cacao.Connectors.DB.MySQL;

/**
 * @author Igor Barsanti - <a href="http://www.gonetwork.it"
 *         target="_blank">Gonetwork Srl</a>
 * 
 */
public class Core {

	private static ConfigurationManager Conf = ConfigurationManager.getInstance();
	private static Logger logger = Logger.getLogger(Core.class);
	private static MySQL cacaodb = MySQL.getInstance();
	private static MainScheduler scheduler = new MainScheduler();

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		
		String configFilePath;
		if (args.length == 1)
			configFilePath = args[0];
		else
			configFilePath = "configuration/HarvesterConfiguration.xml";

		Conf.setConfigurationFile(configFilePath);

		if (!Conf.parse()) {
			logger.error("ERROR: Unable to parse configuration file: " + configFilePath);
			System.exit(1);
		}

		logger.info("Harvester started.");

		logger.debug("Configuration file: " + configFilePath);

		logger.info("Connecting to DB.");
		cacaodb.setServer(Conf.getParam("DBEngine", "Server"));
		cacaodb.setDBname(Conf.getParam("DBEngine", "DataBase"));
		cacaodb.setUsername(Conf.getParam("DBEngine", "User"));
		cacaodb.setPassword(Conf.getParam("DBEngine", "Password"));
		cacaodb.connect();
		
		String[] ags = Conf.getAgents();

		for (int idx = 0; idx < ags.length; idx++) {
			if (Conf.getAgentActive(ags[idx]).equalsIgnoreCase("yes")) {
				logger.info("Found " + ags[idx] + " active configured agent (" + Conf.getAgentType(ags[idx]) + "/" + Conf.getAgentTargetUrl(ags[idx]) + ")");
				OAIHarvestingAgent agent = new OAIHarvestingAgent(ags[idx], Conf.getAgentTargetUrl(ags[idx]), Conf.getAgentMetadataPrefix(ags[idx]), Conf.getAgentStart(ags[idx]),
						Conf.getAgentLimit(ags[idx]), Conf.getAgentLimitType(ags[idx]), Conf.getAgentFrom(ags[idx]), Conf.getAgentTo(ags[idx]));

				scheduler.addAgent(agent);
			} else {
				logger.info("Skipping " + ags[idx] + " inactive configured agent (" + Conf.getAgentType(ags[idx]) + "/" + Conf.getAgentTargetUrl(ags[idx]) + ")");
			}
		}
		
		RunTimer rt=new RunTimer();
		
		scheduler.start();

		logger.info("Close connection to DB.");
		cacaodb.close();

		logger.info("Harvesting Completed. "+rt.getElapsedTimeLabel());

	}

}
