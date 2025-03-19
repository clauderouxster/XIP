package eu.project.Cacao.Components.Utils;

import java.io.IOException;
import java.util.BitSet;

import org.apache.lucene.index.IndexReader;
import org.apache.lucene.index.Term;

/**
 * Base Interface for TermFilters. 
 * @author Alessio
 *
 */
public interface TermFilter {
	/**
	 * Takes 
	 * @param term a lucene Term as input
	 * @return true if is a term has to be included as an element of the  TermVector
	 * @throws IOException
	 */
	public boolean acceptTerm(Term term) throws IOException;
	public void setIndexReader(IndexReader idxReader);
	public void setBitSet(BitSet toAccept);
	public void addAllowedField(String luceneFieldName);
	public int getTermFrequency();
}
