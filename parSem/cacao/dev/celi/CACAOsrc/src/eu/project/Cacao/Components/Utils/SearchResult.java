package eu.project.Cacao.Components.Utils;
import java.util.Vector;


public class SearchResult {
	Vector<DocsSearchResult> docs;
	Vector<FacetsSearchResult> facets;
	long totFound;
	
	public SearchResult(Vector<DocsSearchResult> docs, Vector<FacetsSearchResult> facets, long nFound) {
		this.docs = docs;
		this.facets = facets;
		this.totFound = nFound;
	}

	public Vector<DocsSearchResult> getDocs() {
		return docs;
	}

	public Vector<FacetsSearchResult> getFacets() {
		return facets;
	}
	
	public boolean hasErrorOccurred(){
		if(docs == null)
			return true;
		else
			return false;
	}

	public long getTotFound() {
		return totFound;
	}
	
}
