/*
 * Xerox Research Center Europe
 * Copyright (C) 2009 XRCE, All Rights Reserved
 */

package extui.UI_html;

import extui.UI_components.UserSession;
import java.util.ArrayList;
import java.util.List;
import org.apache.log4j.Logger;
import org.apache.wicket.markup.html.basic.Label;
import org.apache.wicket.markup.html.list.ListItem;
import org.apache.wicket.markup.html.list.ListView;



/**
 *
 * @author ctrojahn
 */
public class Bookmark extends BasePage {

    private static final long serialVersionUID = 1L;
    private Logger logger = Logger.getLogger(Bookmark.class.getName());
   
    /**
     * 
     */
    public Bookmark() {
        
        String bookmarks = ((UserSession) getSession()).getUserProfile().getBookmarks();

        // String for each bookmark: #!#Name: NAME !+! LINK #!# (see ModalWindowBookmark)
        // TODO: use of the folder option

        String[] list = bookmarks.split("#!#");
        List<String> books = new ArrayList<String>();
        for (int x = 0; x < list.length; x++) {
            if (!list[x].equals("null")) {
                books.add(list[x]);
            }
        }

        ListView listView = new ListView("books", books) {
            @Override
            protected void populateItem(ListItem item) {
                String description[] = item.getDefaultModelObjectAsString().split("#name#");
                String name = "";
                String book = "";
                String title = "";

                name = description[0];
                title = description[1];
                book = description[2];

                item.add(new Label("nameBook", name));
                item.add(new Label("book","<a href='" + book + "'>" + title + "</a>").setEscapeModelStrings(false));
                
            }
        };
        add(listView);
    }
}
