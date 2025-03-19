package extui.UI_components;

//import extui.UI_components.*;
import java.util.ArrayList;
import java.util.Hashtable;

/**
 * @author ctrojahn
 */
public class Constant {

    /* the name of the DB */
    /**
     *
     */
    protected static final String dbName = "advUI";
    /* the name of item table in the DB */
    /**
     *
   
    public static final boolean RECURSIVELY = true;
    /*  */
    /**
     *
     */
    public static final boolean NOT_RECURSIVELY = false;
    /*  */
    /**
     *
     */
    public static final boolean DROPANDCREATETABLES = true;
    /*  */
    /**
     *
     */
    public static final boolean NOT_DROPANDCREATETABLES = false;
    /*  */
    /**
     *
     */
    public static final int PAGINIG = 40;
    /*  */
    /**
     *
     */
    public static final int MAX_DOCS_SEARCH = 500;
    /*  */
    /**
     *
     */
    public static final int NUM_THREAD = 2;
    /**
     *
     */
    public static final String COLOR_HIGHLIGHT = "#6c0401";

    /*Color associated to the freq of the tags in the t  target.appendJavascript("window.location.reload()");ag cloud: for example color 1 is used to tags with fre between 1 and 0.66 */
    /**
     *
     */
    public static final String COLOR_1 = "#30698a";
    /**
     *
     */
    public static final String COLOR_2 = "#489ecf";
    /**
     *
     */
    public static final String COLOR_3 = "#969ab5";
    /**
     *
     */
    public static int GREATER_FONT_SIZE = 7;
    /**
     *
     */
    public static int LOWER_FONT_SIZE = 3;
    /*  */
    /**
     *
     */
    public static int TRUNC_TITLE = 150;
    /**
     *
     */
    public static final String[] FACETS = {"language", "libraryID","publisher","teFacet","subject_ClassificationCategory","OAIset"};
    /**
     *
     */
    public static final int SIZE_TAG_CLOUD = 50;
    /**
     *
     */
    public static final int MAX_TOP_N_TITLES = 100;

    // Position 0: field Position 1: correponding facet
    /**
     *
     */
    public static Hashtable MAPPING = new Hashtable();
    /**
     *
     */
    public static Hashtable FACETS_TEMP = new Hashtable();
    /**
     *
     */
    public static Hashtable FIELDS_UI = new Hashtable();

    /* These values are used in the Index1.java and ResultsPage.java and they refer to search options*/
    /**
     * nameUI
     */
    public static ArrayList<NameWrapper> listLanguages = new ArrayList<NameWrapper>();
    /**
     *
     */
    public static ArrayList<NameWrapper> advancedOptions = new ArrayList<NameWrapper>();
   

    public static ArrayList<String> filesCSS = new ArrayList<String>();

  
    /**
     *
     */
    @SuppressWarnings({"unchecked", "unchecked"})
    public static void initializeMapping() {

        // ---------------------------------------------------------------------
        // Facet name for search in WS + name interface (nameUI)
          
        
        MAPPING.put("libraryID","Library");
        MAPPING.put("publisher","Publisher");
        MAPPING.put("subject_ClassificationCategory","Subject");
        MAPPING.put("language","Language");
        MAPPING.put("OAIset","OAI Sets");

 
        // ---------------------------------------------------------------------
        // List of possible languages to the results
        listLanguages.add(new NameWrapper("English", "en", ""));
        listLanguages.add(new NameWrapper("German", "de", ""));
        listLanguages.add(new NameWrapper("French", "fr", ""));
        listLanguages.add(new NameWrapper("Italian", "it", ""));
        listLanguages.add(new NameWrapper("Polish", "pl", ""));

        // Advanced options
        // advancedOptions.add(new NameWrapper("Systran MT", "systran", "Systran MT", true));
        advancedOptions.add(new NameWrapper("SV expansion", "SVexp", "", true));
        advancedOptions.add(new NameWrapper("WN expansion", "WNexp", "", true));
        advancedOptions.add(new NameWrapper("W2C disambiguation", "W2Cexp", "", true));

        // Files CSS: page configuration
        filesCSS.add("styleUI.css");
        filesCSS.add("styleUI1.css");
     
    }
}
