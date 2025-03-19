/**
 *
 */
package eu.project.Cacao.Harvester;

import java.util.Vector;

import org.apache.log4j.Logger;


/**
 * @author Igor Barsanti - <a href="http://www.gonetwork.it" target="_blank">Gonetwork Srl</a>
 *
 */
public class MainScheduler {
	private Logger logger = Logger.getLogger(MainScheduler.class);
    Vector<OAIHarvestingAgent> agents = new Vector<OAIHarvestingAgent>();
    int aidx = 0;

    public void start() {
        logger.info("Scheduler started");

        logger.info("Start harvesting..");

        for (int a = 0; a < agents.size(); a++) {
            OAIHarvestingAgent agent = agents.get(a);
            agent.harvest();
        }
    }

    public String addAgent(OAIHarvestingAgent agent) {
        logger.info("HarvestingAgent " + agent.identifier +
            " added to scheduler");
        agents.add(aidx, agent);
        aidx++;

        return Integer.toString(aidx);
    }

    public OAIHarvestingAgent getAgent(String agentIdentifier) {
        try {
            int getIdx = Integer.parseInt(agentIdentifier);

            return agents.get(getIdx);
        } catch (Exception e) {
            return null;
        }
    }
}
