package eu.project.Cacao.Components.Utils;
import java.util.List;
import java.util.Vector;

import org.apache.solr.client.solrj.response.FacetField;
import org.apache.solr.client.solrj.response.FacetField.Count;
import org.w3c.dom.Document;
import org.w3c.dom.Element;



public class FacetsSearchResult {
	public static final String facetResultElmTag="Facet", fieldNameAttribute="filed", fieldValueAttribute="value", 
	countAttribute="count";
	
	private String facetField, facetValue;
	private long count;
	
	public FacetsSearchResult(String facetField, String facetValue, long count) {
		super();
		this.facetField = facetField;
		this.facetValue = facetValue;
		this.count = count;
	}
	
	public String getFacetField() {
		return facetField;
	}
	public String getFacetValue() {
		return facetValue;
	}
	public long getFacetCount() {
		return count;
	}
	
	static public Vector<FacetsSearchResult> createFacetsSearchResultFromFacetField(FacetField f){
		Vector<FacetsSearchResult> result=new Vector<FacetsSearchResult>();
		String name = f.getName();
		List<Count> values=f.getValues();
		for(Count c: values){
			String value=c.getName();
			long count=c.getCount();
			FacetsSearchResult fsr=new FacetsSearchResult(name, value, count);
			
			if(count>0 && value.trim().length()>0){
				//System.out.println("> "+name+" "+" : "+value+" ("+count+")");
				result.add(fsr);
			}
		}
		
		return result;
	}
	
	public Element toXMLElement(Document doc){
		Element result = doc.createElement(FacetsSearchResult.facetResultElmTag);
		result.setAttribute(FacetsSearchResult.fieldNameAttribute, this.facetField);
		result.setAttribute(FacetsSearchResult.fieldValueAttribute, this.facetValue);
		result.setAttribute(FacetsSearchResult.countAttribute, ""+this.count);
		return result;
	}
	
	static public FacetsSearchResult createFromXMLElement(Element e){
    	String fn=e.getAttribute(FacetsSearchResult.fieldNameAttribute);
    	String fv=e.getAttribute(FacetsSearchResult.fieldValueAttribute);
    	String count=e.getAttribute(FacetsSearchResult.countAttribute);
    	
    	return new FacetsSearchResult(fn, fv, Long.parseLong(count));
    }
}
