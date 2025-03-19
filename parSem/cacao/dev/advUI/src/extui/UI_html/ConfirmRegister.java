/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package extui.UI_html;
import extui.UI_components.UserProfile;
import org.apache.wicket.PageParameters;
import org.apache.wicket.markup.html.basic.Label;
import org.apache.wicket.markup.html.form.Button;
import org.apache.wicket.markup.html.form.Form;

/**
 *
 * @author ctrojahn
 */
public class ConfirmRegister extends BasePage {
    
    private static final long serialVersionUID = 1L;
    /**
     *
     * @param user
     */
    public ConfirmRegister(UserProfile user)
    {
        super();

        
        Form form = new Form("form");
        Button confirm = new Button("confirm");
        Button back = new Button("back");

        form.add(confirm);
        form.add(back);
        add(form);

        

    }

    /**
     *
     * @param params
     */
    public ConfirmRegister(PageParameters params) {
        //TODO:  process page parameters
    }
}

