/*
 * Xerox Research Center Europe
 * Copyright (C) 2009 XRCE, All Rights Reserved
 */

package extui.UI_html;


import extui.UI_components.Document;
import extui.UI_components.Refresh;
import extui.UI_components.UserProfile;
import extui.UI_components.UserSession;
import org.apache.wicket.ajax.AjaxRequestTarget;
import org.apache.wicket.ajax.markup.html.form.AjaxButton;
import org.apache.wicket.extensions.ajax.markup.html.modal.ModalWindow;
import org.apache.wicket.markup.html.WebPage;
import org.apache.wicket.markup.html.form.Form;
import org.apache.wicket.markup.html.form.TextField;
import org.apache.wicket.markup.html.resources.StyleSheetReference;
import org.apache.wicket.model.Model;
import org.apache.wicket.model.StringResourceModel;


public class ModalWindowBookmark extends WebPage {

    Refresh refresh;
    private static final long serialVersionUID = 1L;

    /**
     *
     * @param modalWindowPage
     * @param window
     * @param refresh 
     *
     */
    public ModalWindowBookmark(final ResultsPage modalWindowPage, final ModalWindow window, final Refresh refresh) {

        this.refresh = refresh;
        add(new StyleSheetReference("stylesheet", BasePage.class, "styleUI.css"));
        
        @SuppressWarnings("unchecked")
        final TextField nameBook = new TextField("nameBook", new Model());
        @SuppressWarnings("unchecked")
        TextField folderBook = new TextField("folderBook", new Model());
        ModalWindowBookmark mo = this;

        AjaxButton cancel = new AjaxButton("cancel", new StringResourceModel("labelButtonCancel", this, null,"XXXX") ) {
            private static final long serialVersionUID = 1L;
            @Override
            protected void onSubmit(AjaxRequestTarget target, Form<?> form) {
                      window.close(target);
            }

        };

        AjaxButton submit = new AjaxButton("book", new StringResourceModel("labelButtonBookmark", this, null,"XXXX")) {
            private static final long serialVersionUID = 1L;
            @Override
            protected void onSubmit(AjaxRequestTarget target, Form<?> form) {
                Document documentBookmark = refresh.getDocumentBookmark();
                // String for each bookmark: #!#Name: NAME !+! LINK #!#
                // TODO: use of the folder option
                String bookmark = "#!# " + nameBook.getDefaultModelObjectAsString() + " #name# ";
                bookmark += documentBookmark.getTitle() + "#name#";
                bookmark += documentBookmark.getTelRecordID_URL() + "#!#";
                UserProfile user = ((UserSession) getSession()).getUserProfile();
                String books = user.getBookmarks();
                books += bookmark;
                user.setBookmarks(books);
                UserProfile newUser = Application.getManagerDB_Users().addBookmark(user);
                ((UserSession) getSession()).setUserProfile(newUser);
                 window.close(target);
            }
        };


        Form form = new Form("form");
        form.add(nameBook);
        form.add(folderBook);
        form.add(submit);
        form.add(cancel);
        add(form);
    }
}