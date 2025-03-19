/*
 * Xerox Research Center Europe
 * Copyright (C) 2009 XRCE, All Rights Reserved
 */

package extui.UI_components;

import java.util.Iterator;
import org.apache.log4j.Logger;
import org.apache.wicket.markup.repeater.data.IDataProvider;
import org.apache.wicket.model.IModel;

/**
 *
 * @author ctrojahn
 */
public class TranslationsDataProvider implements IDataProvider {

    private static final Logger logger = Logger.getLogger(TranslationsDataProvider.class.getName());
    private Refresh refresh;

    /**
     * @param refresh
     */
    public TranslationsDataProvider(Refresh refresh) {
       this.refresh = refresh;
    }

    public Iterator iterator(int first, int count) {
           return refresh.getListTranslations().iterator();
    }
    /**
     *
     * @return
     */
    public int size() {
        return refresh.getListTranslations().size();
    }

    /**
     *
     * @param object
     * @return
     */
    public IModel model(Object object) {
        return new ModelTranslation((Facet) object,refresh);
    }

    /**
     *
     */
    public void detach() {
    }
}

