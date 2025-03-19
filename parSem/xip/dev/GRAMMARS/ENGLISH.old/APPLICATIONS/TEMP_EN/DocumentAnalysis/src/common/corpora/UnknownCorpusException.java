/*
 * UnknownCorpusException.java
 *
 * Created on December 8, 2006, 3:02 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package common.corpora;

/**
 * Exception raised when the specified corpus is unknown.
 * @author xtannier
 */
public class UnknownCorpusException extends java.lang.Exception {
    
    private static final long serialVersionUID = 1L;

    /**
     * Creates a new instance of <code>UnknownCorpusException</code> without detail message.
     */
    public UnknownCorpusException() {
        super();
    }
    
    
    /**
     * Constructs an instance of <code>UnknownCorpusException</code> with the specified detail message.
     * @param corpus the corpus name.
     */
    public UnknownCorpusException(String corpus) {
        super("Exception: Corpus " + corpus + " is unknown.");
    }
}
