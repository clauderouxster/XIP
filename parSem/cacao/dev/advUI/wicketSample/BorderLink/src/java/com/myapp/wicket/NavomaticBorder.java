/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.myapp.wicket;

import org.apache.wicket.markup.html.border.Border;
import org.apache.wicket.markup.html.border.BoxBorder;
/**
 *
 * @author ctrojahn
 */
class NavomaticBorder extends Border{

    public NavomaticBorder(final String string)
    {
           super(string);
           add(new BoxBorder("navigationBorder"));
           add(new BoxBorder("bodyBorder"));

    }
}
