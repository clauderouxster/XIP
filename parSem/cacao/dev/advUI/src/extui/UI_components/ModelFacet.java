/*
 * Xerox Research Center Europe
 * Copyright (C) 2009 XRCE, All Rights Reserved
 */

package extui.UI_components;

import org.apache.log4j.Logger;
import org.apache.wicket.model.LoadableDetachableModel;

/**
 *
 * @author ctrojahn
 */
class ModelFacet extends LoadableDetachableModel {

    private static final Logger logger = Logger.getLogger(ModelFacet.class.getName());
    private int id;
    private Refresh refresh;

    /**
     *
     * @param c
     * @param refresh
     */
    public ModelFacet(Facet f, Refresh refresh) {
        this(f.getId());
        this.refresh = refresh;
    }

    /**
     *
     * @param id
     */
    public ModelFacet(int id) {
        if (id == 0) {
            throw new IllegalArgumentException();
        }
        this.id = id;
    }

    @Override
    public int hashCode() {
        return Long.valueOf(id).hashCode();
    }

    @Override
    public boolean equals(final Object obj) {
        if (obj == this) {
            return true;
        } else if (obj == null) {
            return false;
        } else if (obj instanceof ModelFacet) {
            ModelFacet other = (ModelFacet) obj;
            return other.id == this.id;
        }
        return false;
    }

    /**
     *
     * @return
     */
    protected Object load() {
          return refresh.getFacet(id);

    }
}

