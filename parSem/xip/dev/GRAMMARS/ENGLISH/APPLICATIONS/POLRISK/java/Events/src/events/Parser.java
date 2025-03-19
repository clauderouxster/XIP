/*
 * Parser.java
 *
 * Created on Jul 20, 2007, 12:27:03 PM
 *
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package events;

import com.xerox.jxip.*;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Vector;

/**
 *
 * @author arebotie
 */
public class Parser {

    private XipResultManager xipResultManager;
    private int gHandler;
    private JXip jxip;
    private XipResult xipResult;

    public Parser() throws Exception {
        jxip = new JXip();
    }

    public void loadGrammar(String grmFile) throws Exception {
        gHandler = jxip.XipGrmFile(grmFile, 0, true);
    }

    public void sentenceProcessing() {
        Vector<Event> SentenceEvents = new Vector<Event>();
        Vector<Entity> SentenceEntities = new Vector<Entity>();
        try {
            xipResult = xipResultManager.getXipResult();
            for (Enumeration enumeration = xipResult.getXipUnits().elements(); enumeration.hasMoreElements();) {
                XipUnit xipUnit = (XipUnit) enumeration.nextElement();
                System.out.println("\n-------------------- " + xipUnit.getSentenceString() + "--------------------\n");
                XipNode xipNode = xipUnit.getRoot();
                //créer les événements
                Vector<XipDependency> deps = xipUnit.getDependenciesByName("CORE");
                for (Enumeration<XipDependency> e = deps.elements(); e.hasMoreElements();) {
                    XipDependency dep = e.nextElement();
                    XipNode node = dep.getArg(1);
                    Core core = new Core(node);
                    boolean top=false;
                    if (dep.hasFeatures("topic")){
                    top=true;
                    }
                    Event event = new Event(core, top);
                    SentenceEvents.add(event);
                }
                String type = "";
                //ajouter les coordonnées
                deps = xipUnit.getDependenciesByName("COORDINATE");
                                for (Enumeration<XipDependency> e = deps.elements(); e.hasMoreElements();) {
                        XipDependency dependency = e.nextElement();
                        //ne pas tenir compte des COORDINATEs qui ont le trait merged : elles sont créées lorsqu'il y a plusieurs coordonnées du même type pour n'afficher qu'un événement dans les dépendances EVENT et dans le tableau (programme colorPR)
                   if (!dependency.hasFeatures("merged")){
                     XipNode core = dependency.getArg(1);
                    XipNode node = dependency.getArg(2);
                    if (dependency.hasFeatures("actor"))
                    {type = "actor";
                    }
                    if (dependency.hasFeatures("target"))
                    {type = "target";
                    }
                    if (dependency.hasFeatures("loc-pr"))
                    {type = "loc";
                    }
                    if (dependency.hasFeatures("time"))
                    {type = "time";
                    }
                    
                    //créer ou trouver l'Entity correspondant à la coordonnée
                    //a. la créer de toute façon
                    Entity coordinate = new Entity(node);
//                    System.out.println("coordonnée trouvée : "+ coordinate);
                    //b. la chercher dans les Entity déjà créées pour cette phrase
                    boolean i = false;
                    for (Enumeration<Entity> entities = SentenceEntities.elements(); entities.hasMoreElements();) {
                        Entity entity = entities.nextElement();
                        if (coordinate.xipnode == entity.xipnode) {
                            coordinate = entity;
                            i = true;
                        }
                    }
                    //c. si elle a été créée, l'ajouter à la liste des Entity de la phrase
                    if (i == false) {
                        SentenceEntities.add(coordinate);
                    }
                    // d. créer la relation entre événement et coordonnée
                    for (Enumeration<Event> events = SentenceEvents.elements(); events.hasMoreElements();) {
                        Event event = events.nextElement();
                                if (event.hasCore.getCore().xipnode == core) {
                                    if (type.equals("actor")){
                            event.setActor(coordinate);
                                    }
                                    if (type.equals("target")){
                            event.setTarget(coordinate);
                                    }
                                    if (type.equals("time")){
                            event.setTime(coordinate);
                                    }
                                    if (type.equals("loc")){
                            event.setLoc(coordinate);
                                    }
                        }
                    }
                }
                                }
                String factuality = "";
                // ajouter les sources
              Vector<XipDependency> sources = xipUnit.getDependenciesByName("SRC");
  
                        for (Enumeration<XipDependency> e = sources.elements(); e.hasMoreElements();) {
                    XipDependency dependency = e.nextElement();
                    if (!dependency.hasFeatures("merged")){
                   XipNode core = dependency.getArg(1);
                    XipNode node = dependency.getArg(2);
                    if (dependency.hasFeatures("factual"))
                    {factuality = "factual";
                    }
                    if (dependency.hasFeatures("possible"))
                    {factuality = "possible";
                    }
                    if (dependency.hasFeatures("counter-factual"))
                    {factuality = "counter-factual";
                    }
                    
                    //créer ou trouver l'Entity correspondant à la source
                    //a. la créer de toute façon
                    Entity source = new Entity(node);
//                    System.out.println("source trouvée : "+ coordinate);
                    //b. la chercher dans les Entity déjà créées pour cette phrase
                    boolean i = false;
                    for (Enumeration<Entity> entities = SentenceEntities.elements(); entities.hasMoreElements();) {
                        Entity entity = entities.nextElement();
                        if (source.xipnode == entity.xipnode) {
                            source = entity;
                            i = true;
                        }
                    }
                    //c. si elle a été créée, l'ajouter à la liste des Entity de la phrase
                    if (i == false) {
                        SentenceEntities.add(source);
                    }
                    // d. créer la relation entre événement et coordonnée
                    for (Enumeration<Event> events = SentenceEvents.elements(); events.hasMoreElements();) {
                        Event event = events.nextElement();
                                if (event.hasCore.getCore().xipnode == core) {
                                    if (factuality.equals("factual")){
                            event.setSource(source, factuality);
                                    }
                                    if (factuality.equals("possible")){
                            event.setSource(source, factuality);
                                    }
                                    if (factuality.equals("counter-factual")){
                            event.setSource(source, factuality);
                                    }
                                    
                        }
                    }
                }  
                
                        } 
            }
        } catch (Exception ex) {
            System.out.println(ex);
            ex.printStackTrace();
            System.exit(-1);
        }
        System.out.println(SentenceEvents);
    }

    public void tagProcessing() {
        try {
            System.out.println("\n ---------------------------- tagProcessing ----------------------\n");
            Hashtable<String, String> xmlTagI = xipResultManager.getXmlTagInfo();
            System.out.println("xmlTagInfo = " + xmlTagI.toString());
        } catch (Exception ex) {
            System.out.println(ex);
            ex.printStackTrace();
        }
    }

    /**
     *  Parse a String
     *
     * @param  input  the string to parse
     */
    public void parseString(String input) throws Exception {
        xipResultManager = new XipResultManager();
        xipResultManager.xipResultBuilderFromString(gHandler, input, this, "sentenceProcessing");
    }

/**
     *  Parse a text file
     *
     * @param  textFile  the text file to parse
     */
    public void parseTextFile(String textFile) throws Exception {
        xipResultManager = new XipResultManager();
        xipResultManager.xipResultBuilderFromTextFile(gHandler, textFile, this, "sentenceProcessing");
    }

/**
     *  Parse a xml file
     *
     * @param  xmlFile  the text file to parse
     */
    public void parseXmlFile(String xmlFile) throws Exception {
        xipResultManager = new XipResultManager();
        xipResultManager.xipResultBuilderFromXmlFile(gHandler, xmlFile, this, "sentenceProcessing", "tagProcessing");
    }
}
