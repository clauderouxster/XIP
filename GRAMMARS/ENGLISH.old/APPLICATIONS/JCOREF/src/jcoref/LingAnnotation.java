package jcoref;
/*
 * LingAnnotation.java
 *
 * Created on September 14, 2006, 4:51 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

/**
 *
 * @author ait
 */
public interface LingAnnotation {
    
    public long getLeft();
    public long getRight();
    
    public String toVisibleString();
    public String toTipToolString();
    public String toClickString();
    public String toCtrlClickString();

}
