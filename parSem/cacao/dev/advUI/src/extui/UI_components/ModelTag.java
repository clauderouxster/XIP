/*
 * Xerox Research Center Europe
 * Copyright (C) 2009 XRCE, All Rights Reserved
 */

package extui.UI_components;

import org.apache.wicket.model.LoadableDetachableModel;
import org.apache.log4j.Logger;

public class ModelTag extends LoadableDetachableModel {

    private static final Logger logger = Logger.getLogger(ModelTag.class.getName());
    private int id;
    private Refresh refresh;

    /**
     *
     * @param c
     * @param refresh
     */
    public ModelTag(Tag c, Refresh refresh) {
        this(c.getId());
        this.refresh = refresh;
    }

    /**
     *
     * @param id
     */
    public ModelTag(int id) {
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
        } else if (obj instanceof ModelData) {
            ModelTag other = (ModelTag) obj;
            return other.id == this.id;
        }
        return false;
    }

    /**
     *
     * @return
     */
    protected Object load() {
        return refresh.getTag(id);
    }
}
