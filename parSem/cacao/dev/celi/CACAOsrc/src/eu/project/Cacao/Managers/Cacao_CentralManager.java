package eu.project.Cacao.Managers;

import eu.project.Cacao.Interfaces.UnifiedRepositoryAccess;


/**
 * This class is charged to coordinate off-line processes and system initialization, DB connectors..
 * It invokes the different Library Connectors (URAs) in order to harvest data
 * @author Alessio
 *
 */
public class Cacao_CentralManager {
    /**
     * Coordinate the harvesting process. It uses the input param in order to access data
     * from libraries
     *
     * @param libraryConnector
     */
    public void harvestDataFromLibrary(UnifiedRepositoryAccess libraryConnector) {
    }
}
