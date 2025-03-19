package eu.project.Cacao.Components.Utils;

import java.io.IOException;
import java.util.BitSet;
import java.util.HashSet;

import org.apache.lucene.index.IndexReader;
import org.apache.lucene.index.Term;
import org.apache.lucene.index.TermDocs;

/**
 * A TermFilter based on regular expressions. It can operate in two ways
 * A regular expression can be passed in the constructor and then each term is evaluated against that reg exp.
 * Invalid suffixes can be specified (in addition or in alternativa to the previous option) by means of  addInvalidSuffix method;
 * RegExpTermFilter checks if the term ends with an unsupported suffix and if it does, it is discarded.
 * @author Alessio
 *
 */
public class RegExpTermFilter implements TermFilter{
	private int minFreq;
	private IndexReader indexReader;
	private String regExp;
	private HashSet<String> invalidSuffixes, allowedFields;
	private BitSet toAccept=null;
	private int frequency=0;
	/**
	 * @param minFreq
	 * @param indexReader
	 * @param regExp
	 */
	public RegExpTermFilter(int minFreq, IndexReader indexReader, String regExp) {
		this.minFreq = minFreq;
		this.indexReader = indexReader;
		this.regExp = regExp;
		this.invalidSuffixes=new HashSet<String>();
		this.allowedFields=new HashSet<String>();
	}
	
	public RegExpTermFilter(int minFreq, String luceneIndexDir, String regExp) throws IOException {
		this.minFreq = minFreq;
		this.indexReader = IndexReader.open(luceneIndexDir);
		this.regExp = regExp;
		this.invalidSuffixes=new HashSet<String>();
		this.allowedFields=new HashSet<String>();
	}
	
	public void addInvalidSuffix(String s){
		this.invalidSuffixes.add(s);
	}
	
	public void addAllowedField(String s){
		this.allowedFields.add(s);
	}
	
	public void setBitSet(BitSet toAccept){
		this.toAccept=toAccept;
	}
	/**
	 * filters out terms on the base of reg exp and those of low frequency
	 */
	public  boolean acceptTerm(Term term) throws IOException {
		/* character filter */

		String termText = term.text();
		
		if(this.allowedFields.size()>0 && !this.allowedFields.contains(term.field()))
			return false;
			
		
		if (regExp!=null && !termText.matches(regExp) ){
			return false;
		}
		
		else if(this.invalidSuffixes.size()>0){
			for(String suff: this.invalidSuffixes){
				if(termText.endsWith(suff) || termText.startsWith("_"))
					return false;
			}
		}
		
		/* freqency filter */
		this.frequency= 0;
		TermDocs tDocs = indexReader.termDocs(term);
		while (tDocs.next()) {
			int doc  = tDocs.doc();
			if(this.toAccept!=null && !toAccept.get(doc))
				continue;
			this.frequency += tDocs.freq();
		}
		if (this.frequency < minFreq) {
			return false;
		}

		
		//System.out.println(termText+" - "+term.field());
		return true;
	}

	public void setIndexReader(IndexReader idxReader) {
		this.indexReader=idxReader;
	}

	public int getTermFrequency() {
		return frequency;
	}
	
}
