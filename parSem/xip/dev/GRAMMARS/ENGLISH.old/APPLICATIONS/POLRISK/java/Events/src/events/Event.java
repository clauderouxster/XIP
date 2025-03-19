/*
 * Event.java
 * 
 * Created on Jul 20, 2007, 2:25:23 PM
 * 
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package events;

import java.util.Enumeration;
import java.util.Vector;

/**
 *
 * @author arebotie
 */
public class Event {
    HasCore hasCore;
Vector<Relation> coordinates;
Vector<HasSource> hasSources;
private boolean topic;

    public Event(Core core) {
        hasCore = new  HasCore(this, core);
        this.hasCore = hasCore;
        this.coordinates = new Vector<Relation>();
        this.hasSources = new Vector<HasSource>();
    }
    
    public Event(Core core, boolean information) {
        hasCore = new  HasCore(this, core);
        this.hasCore = hasCore;
        this.topic = information;
        this.coordinates = new Vector<Relation>();
        this.hasSources = new Vector<HasSource>();
    }

    public void setActor (Entity actor){
        HasActor hasActor = new HasActor(this, actor);
        this.coordinates.add(hasActor);
    }
    public void setTarget (Entity target){
        HasTarget hasTarget = new HasTarget(this, target);
        this.coordinates.add(hasTarget);
    }
    public void setTime (Entity time){
        HasTime hasTime = new HasTime(this, time);
        this.coordinates.add(hasTime);
    }
    public void setLoc (Entity loc){
        HasLoc hasLoc = new HasLoc(this, loc);
        this.coordinates.add(hasLoc);
    }
    
     public void setSource (Entity source, String factuality){
        HasSource hasSource = new HasSource(this, source, factuality);
        this.hasSources.add(hasSource);
    }
    
    public Vector<Entity> getActors(){
            Vector<Entity> actors = new Vector<Entity>();
        for (Enumeration<Relation> coord = this.coordinates.elements(); coord.hasMoreElements();) {
            Relation coordinate = coord.nextElement();
            if (coordinate instanceof HasActor){
                Entity actor = (Entity) coordinate.getEntity2();
                actors.add(actor);
               }
        }
             return actors;
    }
    public Vector<Entity> getTargets(){
            Vector<Entity> actors = new Vector<Entity>();
        for (Enumeration<Relation> coord = this.coordinates.elements(); coord.hasMoreElements();) {
            Relation coordinate = coord.nextElement();
            if (coordinate instanceof HasTarget){
                Entity actor = (Entity) coordinate.getEntity2();
                actors.add(actor);
               }
        }
             return actors;
    }
    
  public Vector<Entity> getLocs(){
            Vector<Entity> actors = new Vector<Entity>();
        for (Enumeration<Relation> coord = this.coordinates.elements(); coord.hasMoreElements();) {
            Relation coordinate = coord.nextElement();
            if (coordinate instanceof HasLoc){
                Entity actor = (Entity) coordinate.getEntity2();
                actors.add(actor);
               }
        }
             return actors;
    }
  
  public Vector<Entity> getTimes(){
            Vector<Entity> actors = new Vector<Entity>();
        for (Enumeration<Relation> coord = this.coordinates.elements(); coord.hasMoreElements();) {
            Relation coordinate = coord.nextElement();
            if (coordinate instanceof HasTime){
                Entity actor = (Entity) coordinate.getEntity2();
                actors.add(actor);
               }
        }
             return actors;
    }
  public Vector<Entity> getSources(){
  Vector<Entity> sources = new Vector<Entity>();
  for (Enumeration<HasSource> e = this.hasSources.elements(); e.hasMoreElements();) {
            HasSource hasSource = e.nextElement();
            Entity source = (Entity)hasSource.getEntity2();
            sources.add(source);
  }
  return sources;
  }
  
  public Vector<String> getSourceswithFactuality(){
  Vector<String> pov = new Vector<String>();
  for (Enumeration<HasSource> e = this.hasSources.elements(); e.hasMoreElements();) {
            String sourcewithFactuality = e.nextElement().getSourcewithFactuality();
            pov.add(sourcewithFactuality); 
  }
  return pov;
  }
  
  public Vector<String> getFactuality(){
      Vector<String> fact = new Vector<String>();
      for (Enumeration<HasSource> e = this.hasSources.elements(); e.hasMoreElements();) {
            fact.add(e.nextElement().getFactuality()); 
  }
      return fact;
  }
  
  
    public String toString(){
        String info="";
        if (this.topic){
        info="(topic)";
        }
        return hasCore.getCore().toString() + "" + info + " - Acteur(s) : " + getActors().toString() + " - Cible(s) : " + getTargets().toString() + " - Date(s) : " + getTimes().toString() + " - Lieu(x) : " + getLocs().toString() + " - Source(s) : " + getSourceswithFactuality() + "\n";
    }
}
