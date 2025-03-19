package interfaces;

import java.util.List;

/**
 * This interface allow the user (client component) to pose query here, ignoring
 * the query details about the language. There are two major methods here:
 * querySingleItem or isTrue
 * 
 * @author tlu
 * @version 1.0
 */
public interface QueryAPI {

    /**
     * In this method, user write down the restriction in appropriate language
     * as String. We use SPARQL. The system only needs to know the simplified
     * version of query. And write the constrains into the string argument. As a
     * result, the user will get a list of Object, which should be the answers.
     * 
     * @param restrictions
     * @return a list of answers (Object real type depends on the tools'
     *         specification
     */
    public List querySingleItem(String restrictions);

    /**
     * This method test if an given statement is true or false. In the first deliverable, this 
     * method is rarely implemented or used.
     * 
     * @param queryStatement
     * @return true or false
     */
    public boolean isTrue(String queryStatement);
}
