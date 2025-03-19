package eu.project.Cacao.Components.Utils;

import java.io.UnsupportedEncodingException;
import java.text.Normalizer.Form;

import org.w3c.dom.DOMException;
import org.w3c.dom.Document;
import org.w3c.dom.Element;

/**
 * The confidence field is not used in equals and hashCode overridden methods so 2 NLPtoken instances 
 * that differ only for confidence are considered equal !! <- NB
 * @author Alessio
 *
 */
public class NLPtoken {
	
    private static final int HASH_PRIME = 103;
    public String lemma, surface, category, lang, neCategory;
    public boolean isNE, isMW, isMWexpr;
    private boolean isConfidenceSetted;
    
    //used to express the degree of similarity in expansions OR the accuracy/confidence for translations OR boast for lucene queries
    private float confidence;
    
    public NLPtoken(String surface, String category, String lemma, String neCategory, String lang) {
        this.lemma = lemma;
        this.surface = surface;
        this.category = category;
        this.lang = lang;
        this.neCategory = neCategory;
        if(this.neCategory==null || this.neCategory.length()==0)
        	this.isNE=false;
        else
        	this.isNE=true;
        
        if(this.surface.trim().indexOf(' ')>0)
        	this.isMW = true;
        else
        	this.isMW = false;
        
        if(lemma!= null && lemma.trim().indexOf(' ')>0 && lemma.trim().indexOf('_')>0)
        	this.isMWexpr = true;
        else
        	this.isMWexpr = false;
        this.confidence = 0;
        this.isConfidenceSetted=false;
    }

    public String getLemma_Category() {
    	if(this.lemma!=null && this.lemma.indexOf('_')>0 && this.lemma.indexOf(' ')>0)
    		return this.lemma;
    	
        if ((category != null) && (category.length() > 0)) {
        	if(this.lemma !=null && this.lemma.length()>0)
        		return this.lemma + "_" + this.category;
        	else
        		return this.surface + "_" + this.category;
        } 
     
        if(this.lemma !=null && this.lemma.length()>0)
        	return this.lemma;
        else
        	return this.surface;
    }

    public String toString() {
        return this.surface + " " + this.category + " " + this.lemma + " " +
        "Named_Entity("+this.isNE+") "+this.lang+" ["+this.confidence+"]";
    }
    
    public Element toXMLElement(Document doc, String tagName){
    	Element tokElm=doc.createElement(tagName);
    	tokElm.setTextContent(this.surface);
    	
    	if(this.lemma!=null)
    		tokElm.setAttribute("lemma", this.lemma);
    	if(this.category!=null)
    		tokElm.setAttribute("category", this.category);
    	tokElm.setAttribute("isNE", Boolean.toString(this.isNE));
    	if(this.isNE){
    		tokElm.setAttribute("NEcategory", this.neCategory);
    	}
    	tokElm.setAttribute("lang", this.lang);
    	if(this.isConfidenceSetted)
    		tokElm.setAttribute("confidence", Float.toString(this.confidence));
    	return tokElm;
    }
    
    
    static public NLPtoken createFromXMLElement(Element e){

    	String srfc = e.getTextContent();
    	String lemma=e.getAttribute("lemma");
    	String category=e.getAttribute("category");
    	String lang=e.getAttribute("lang");
    	String NEcategory=e.getAttribute("NEcategory");
    	String confidence=e.getAttribute("confidence");
    	
    	NLPtoken result = new NLPtoken(srfc, category, lemma, NEcategory, lang);
    	if(confidence != null && confidence.length()>0){
    		result.setConfidence(Float.parseFloat(confidence));
    	}
    	return result;
    }
    
    //In order to enable a class 4 Hash based containers it must override equals(Object) and not overload with another Class
    //It must also implement the hashCode method
    public boolean equals(Object obj) {
        if ((obj == null) || !this.getClass().equals(obj.getClass())) {
            return false;
        }

        NLPtoken x = (NLPtoken) obj;
        
        if( (lemma==null && x.lemma!=null) || (x.lemma==null && lemma!=null) || (lemma!= null && x.lemma!=null && !lemma.equalsIgnoreCase(x.lemma)) )
        	return false;
        
        if( (surface==null && x.surface!=null) || (x.surface==null && surface!=null) || (surface!= null && x.surface!=null && !surface.equalsIgnoreCase(x.surface)) )
        	return false;
        
        if( (category==null && x.category!=null && x.category.length()>0) || (x.category==null && category!=null && category.length()>0) || (category!= null && x.category!=null && !category.equalsIgnoreCase(x.category)) )
        	return false;
        
        if( (lang==null && x.lang!=null) || (x.lang==null && lang!=null) || (lang!= null && x.lang!=null && !lang.equalsIgnoreCase(x.lang)) )
        	return false;
        
        if( (neCategory==null && x.neCategory!=null && x.neCategory.length()>0) || (x.neCategory==null && neCategory!=null && neCategory.length()>0) || (neCategory!= null && x.neCategory!=null && !neCategory.equalsIgnoreCase(x.neCategory)) )
        	return false;
        
        return true;
    }

    public int hashCode() {
        int hash = 0;
        
        if(surface!=null)
        	hash = surface.toUpperCase().hashCode();
        if(lemma!=null)
        	hash = (HASH_PRIME * hash) + lemma.toUpperCase().hashCode();
        if(category!=null)
        	hash = (HASH_PRIME * hash) + category.hashCode();
        if(lang!=null)
        	hash = (HASH_PRIME * hash) + lang.hashCode();
        if (isNE)
            hash = (HASH_PRIME * hash) + neCategory.hashCode();
        
        return hash;
    }

	public float getConfidence() {
		return confidence;
	}

	public void setConfidence(float confidence) {
		this.confidence = confidence;
		this.isConfidenceSetted=true;
	}

	protected boolean isMWexpr() {
		return isMWexpr;
	}
}
