package knowledgeBases;

/**
 * Generate (or better choose) different KBManager, depending on the models
 * defined by corresponding KBManagers.
 * 
 * @author tlu
 * @version 1.0
 */
public class KBManagerFactory {

    private KBManagerFactory() {
        // private constructor, this is an utility class
    }

    public static KBManager createKBManager(String model) {
        if (model.equals(JenaKBManager.JENA_PELLET_PELLET_MEM)) {
            return new JenaKBManager("jena-pellet", "pellet", "");
        } else if (model.equals(JenaKBManager.JENA_PELLET_PELLET_DB)) {
            return new JenaKBManager("jena-pellet", "pellet", "db");
        } else if (model.equals(JenaKBManager.JENA_OWL_KAON2)) {
            return new JenaKBManager("jena-owl", "kaon", "");
        } else if (model.equals(JenaKBManager.JENA_OWL_PELLET_DB)) {
            // this is the default one
            // it is the same as given nothing like the next case
            return new JenaKBManager("jena-owl", "pellet", "db");
        } else {
            // UNTIL NOW this case is the same as JENA_OWL_PELLET_DB
            return new JenaKBManager("", "", "");
        }
    }
}