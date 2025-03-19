/**
 *
 */
package eu.project.Cacao.Harvester;



/**
 * @author Igor Barsanti - <a href="http://www.gonetwork.it" target="_blank">Gonetwork Srl</a>
 *
 */
public abstract class HarvestingAgent {
    String identifier;

    /**
     * @param identifier
     */
    public HarvestingAgent(String identifier) {
        super();
        this.identifier = identifier;
    }
}
