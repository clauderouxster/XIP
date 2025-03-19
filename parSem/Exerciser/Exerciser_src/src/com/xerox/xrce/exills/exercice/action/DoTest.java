/*
 * DoTest.java
 *
 * Created on February 1, 2007, 3:20 PM
 */

package com.xerox.xrce.exills.exercice.action;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import org.apache.struts.action.ActionForm;
import org.apache.struts.action.ActionMapping;
import org.apache.struts.action.ActionForward;

import java.io.*;
import com.xerox.xrce.exills.*;
import com.xerox.xrce.exills.exercice.ExerciceListener;


/**
 *
 * @author vgrassau
 * @version
 */

public class DoTest extends ExerciserAction {
    
    public ActionForward executeAction(ActionMapping mapping, ActionForm  form,HttpServletRequest request, HttpServletResponse response)            throws Exception {
        try {
            if (! isUserPresent()) {
                return sessionTimeout(mapping,request);
            }
            
            String page = request.getParameter("PAGE");
            ExerciceListener el = (ExerciceListener)session.getAttribute(Constants.EXERCICE_LISTENER);
            File src = new File(el.getPathExercice()+ "/" + page);
            String page_test =  "/"+ user.getProject() +"/" + ExerciceListener.FOLDER_EXERCICE  + "/" + page;
            
            File dst = new File(System.getenv("CATALINA_HOME") + "/webapps/Exerciser" + page_test);
            
            File parent = dst.getParentFile();
            parent.mkdirs();
            
            if (!dst.exists()) {
                InputStream in = new FileInputStream(src);
                OutputStream out_ = new FileOutputStream(dst);
                
                // Transfer bytes from in to out
                byte[] buf = new byte[1024];
                int len;
                while ((len = in.read(buf)) > 0) {
                    out_.write(buf, 0, len);
                }
                
                in.close();
                out_.close();
                
            }
            
            
            
            session.setAttribute("PAGE_TEST",page_test);
            
            return mapping.findForward(SUCCESS);
            
        } catch (Exception e) {
            throw e;
        }
        
        
    }
}
