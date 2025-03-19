/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package extui.UI_components;

import java.io.Serializable;
import org.apache.log4j.Logger;
import org.apache.wicket.ajax.AjaxRequestTarget;
import org.apache.wicket.ajax.markup.html.AjaxLink;
import org.apache.wicket.markup.html.WebMarkupContainer;
import org.apache.wicket.markup.html.basic.Label;

/**
 *
 * @author ctrojahn
 */
public class LinkNavigator extends AjaxLink implements Serializable {

    private static final Logger logger = Logger.getLogger(LinkNavigator.class.getName());
    String idL;
    String label;
    ModelData model;
    WebMarkupContainer container;

    /**
     *
     * @param id
     * @param label
     * @param container
     * @param model
     */
    public LinkNavigator(String id, String label, WebMarkupContainer container, ModelData model) {
        super(id);
        this.idL = id;
        this.label = label;
        this.container = container;
        this.model = model;
        add(new Label("label", label));
    }

    /**
     *
     */
    public LinkNavigator() {
        super("link");
    }

    /**
     *
     * @param target
     */
    public void onClick(AjaxRequestTarget target) {
        int start = Integer.parseInt(getLabel());
        // model.setStart(start);
        target.addComponent(container);
    }

    /**
     *
     * @return
     */
    public String getIdL() {
        return idL;
    }

    /**
     *
     * @return
     */
    public String getLabel() {
        return label;
    }

    /**
     *
     * @return
     */
    public WebMarkupContainer getContainer() {
        return container;
    }

    /**
     *
     * @return
     */
    public ModelData getModelExt() {
        return model;
    }
}

