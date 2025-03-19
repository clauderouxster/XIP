package interfaces;

/**
 * Interfaces for all the knowledge bases. Documentation for this please see
 * technical report and research manual for XRCE- Parsing Semantic:
 * "Establishing a generic and efficient Knowledge Based system for document
 * processing on semantic level" Or the white paper of "Knowledge
 * Representation" in XRCE-Parsing Semantic Group.
 * 
 * @author tlu
 * @version 1.0
 */
public interface InputAPI {

    /**
     * This method loads the data, which contains the information described in
     * computable language, for instance XML. By loading the document, it first
     * check the DTD or XML schemata for the structures. And then, it will parse
     * the content in the document and try to understand it using the knowledge
     * we loaded for TBox. Again, there might be obvious inconsistency or
     * conflicts. After the loading process, it might run an internal process,
     * which check the inconsistency, which might be able to found out using
     * inferences.
     * 
     * @param location
     */
    public void loadData(String location);

    /**
     * This method loads the description of the general knowledge, that is, the
     * definition of the conceptions and their relations. Since we usually store
     * them in document, we load it from a URI resource. If there's problems
     * occur by this method, it will returns false. Possible problems might be
     * inconsistent statements, duplicated definition and conflicts.
     * 
     * @param location
     * @param ontologyURL 
     */
    public void loadBackgroundKnowledge(String location, String ontologyURL);

    /**
     * This method bind reasoner for handle the reasoning. Since we want to keep
     * the flexibility of using / choosing different reasoner even within one
     * framework and try the combination to find out the most efficient one.
     * 
     * @param location
     */
    public void bindReasoner(String location);
}
