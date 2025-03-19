/*
 * Xerox Research Center Europe
 * Copyright (C) 2009 XRCE, All Rights Reserved
 */

package extui.UI_components;

import java.util.Enumeration;
import java.util.Iterator;
import java.util.List;
import org.apache.log4j.Logger;
import org.apache.wicket.markup.repeater.data.IDataProvider;
import org.apache.wicket.model.IModel;

/**
 *
 * @author ctrojahn
 */
public class FacetDataProvider implements IDataProvider {

    private static final Logger logger = Logger.getLogger(FacetDataProvider.class.getName());
    private Refresh refresh;

    /**
     * @param refresh
     */
    public FacetDataProvider(Refresh refresh) {
       this.refresh = refresh;
    }

    public Iterator iterator(int first, int count) {
        // TODO: as facetDataProvider and DocumentDataProvider are called in each update of the interface
        // not instantiate searchWS both in FacetDataProvider and DocumentDataProvider
        //SearchWS search = new SearchWS(refresh);
        //  search.getFacets();
        List<Facet> l = refresh.getListFacets();
        return l.iterator();
    }
    /**
     *
     * @return
     */
    public int size() {
        return refresh.getListFacets().size();
    }

    /**
     *
     * @param object
     * @return
     */
    public IModel model(Object object) {
        return new ModelFacet((Facet) object,refresh);
    }

    /**
     *
     */
    public void detach() {
    }
}

