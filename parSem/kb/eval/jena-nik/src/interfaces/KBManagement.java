package interfaces;

/**
 * This interface offers the management of the knowledge, especially for the
 * description of the knowledge. It give the flexibility of manipulating the
 * knowledge base after it's loaded like adding reified statement, adding rules
 * on fly.
 * 
 * @author tlu
 * @version 1.0
 */
public interface KBManagement {

    /**
     * Fetch all the rules in order to show them, or handle with them.
     * 
     * @return the list of rules (written in String)
     */
    public java.util.List<String> getRules();

    /**
     * Render (show) the knowledge in the tools' appropriate way.
     */
    public void renderKnowledge();

    /**
     * Add rule
     * 
     * @param rule
     * @return true for success
     */
    public boolean addRules(String rule);

    /**
     * Remove rule.
     * 
     * @param rule
     * @return true for success
     */
    public boolean deleteRules(String rule);

    /**
     * add statement violently, without any kind of checking. See also
     * "addStatementWithChecking".
     * 
     * @param statement
     * @return true for success
     */
    public boolean addStatementWithoutChecking(String statement);

    /**
     * Add statement after checking all possibility for merging.
     * <ol>
     * <li>If there is already the same statement, we are not going to add it
     * again.</li>
     * <li>If there's some information stored about the "subject" of the
     * statement, we need to check that information, to merge the two things to
     * one and add new information to the system, if it's new.</li>
     * <li>The same for the "Object" in the statement.</li>
     * <li>We need to handle conflicting information.</li>
     * </ol>
     * 
     * @param statement
     * @return true if successfully added
     */
    public boolean addStatementWithChecking(String statement);

    /**
     * Remove a statement. Dangerous, suggest not to use.
     * 
     * @param statement
     * @return true for success
     */
    public boolean deleteKnowledge(String statement);

    /**
     * Clean the database input and close the connection, if there's a database
     * bound in the background.
     */
    public void clean();
}
