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
public class DocumentDataProvider implements IDataProvider {

    private static final Logger logger = Logger.getLogger(DocumentDataProvider.class.getName());
    private Refresh refresh;
   
    /**
     * @param refresh
     */
    public DocumentDataProvider(Refresh refresh) {
       this.refresh = refresh;
    }

    public Iterator iterator(int first, int count) {
        SearchWS search = new SearchWS(refresh);
        search.getDocs(refresh.getSearchQuery(),first, count);
        List<Document> l = refresh.getListDocuments();
        return l.iterator();
    }
    /**
     *
     * @return
     */
    public int size() {
        return (int) refresh.getContRecords();
    }

    /**
     *
     * @param object
     * @return
     */
    public IModel model(Object object) {
        return new ModelData((Document) object,refresh);
    }

    /**
     *
     */
    public void detach() {
    }
}
