/*
 * Xerox Research Center Europe
 * Copyright (C) 2009 XRCE, All Rights Reserved
 */

package extui.UI_components;

import extui.UI_ws.SearchWS;
import java.util.Iterator;
import java.util.List;
import org.apache.log4j.Logger;
import org.apache.wicket.markup.repeater.data.IDataProvider;
import org.apache.wicket.model.IModel;

/**
 *
 * @author ctrojahn
 */
public class TagDataProvider implements IDataProvider {

    private static final Logger logger = Logger.getLogger(TagDataProvider.class.getName());
    private Refresh refresh;

    /**
     * @param refresh
     */
    public TagDataProvider(Refresh refresh) {
       this.refresh = refresh;
    }

    public Iterator iterator(int first, int count) {
        //SearchWS search = new SearchWS(refresh);
        //search.getDocs(refresh.getSearchQuery(),first, count);
        List<Tag> l = refresh.getTagCloud();
        return l.iterator();
    }
    /**
     *
     * @return
     */
    public int size() {
        return (int) refresh.getTagCloud().size();
    }

    /**
     *
     * @param object
     * @return
     */
    public IModel model(Object object) {
        return new ModelTag((Tag) object,refresh);
    }

    /**
     *
     */
    public void detach() {
    }
}