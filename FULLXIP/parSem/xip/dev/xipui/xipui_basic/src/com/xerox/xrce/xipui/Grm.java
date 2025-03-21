/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 * Grm.java
 *
 * Created on March 1, 2007, 3:35 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui;

import java.io.*;
import java.util.*;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 *
 * @author vgrassau
 */
public class Grm {

    // the variable that can be dynamically defined by the xip parser 
    public static final String GLOBAL_VARIABLE = "GRM_PATHNAME";
    //the context of the project
    private ProjectContext context;
    private File grmFile;
    // the map of the variable defined in the grm file
    //only the variable defined in the path instructions
    private Map<String, VarEnv> variablesMap;
    // a collection of variable name, that they can be undefined after the reading
    private Collection<String> candidate4Unkwnon;
    // the xip parser, in order to set some variable with the context
    private Xip xip;

    /** Creates a new instance of Grm 
     *@param xip the xip parser to use
     *@param context the ProjectContext to use
     */
    public Grm(Xip xip, ProjectContext context) {
        this.xip = xip;
        this.context = context;
        variablesMap = new Hashtable();
    }

    /**
     *Return the grm file
     *@return grm file if it was set otherwise null
     */
    public File getGrmFile() {
        return this.grmFile;
    }

    /**
     *Returns the name of the grm file
     *@return the name of the grm file or null if no file was set
     */
    public String getName() {
        String name = null;
        if (grmFile != null) {
            name = grmFile.getName();
        }
        return name;
    }

    /**
     *Return the absolute path of the grm file
     *@return the absolute path of the file if it was set otherwise return null
     */
    public String getAbsolutePath() {
        String path = null;
        if (grmFile != null) {
            path = grmFile.getAbsolutePath();
        }
        return path;
    }

    /**
     *Set the grm file and read the file
     *
     *@param f the grm file
     * @throws java.lang.Exception 
     */
    public void setGrmFile(File f) throws Exception {
        this.grmFile = f;
        init();
    }

    /**
     *Set the grm file and read the file
     *
     *@param f the grm file
     * @throws java.lang.Exception 
     */
    public void setGrmFile(String f) throws Exception {
        setGrmFile(new File(f));
    }

    /**
     *Add the variable names from toadd to the container of variable names
     *add only the new variables names
     *@param container collection of variable names
     *@param toadd the variable name to add
     *
     */
    private void addVariableName(Collection<String> container, Collection<String> toadd) {
        for (Iterator<String> it = toadd.iterator(); it.hasNext();) {
            String objectToAdd = it.next();
            if (!container.contains(objectToAdd)) {
                container.add(objectToAdd);
            }
        }
    }

    /**
     *Read the grm file line per line and try to find some undefined variable.
     *call this method for each modification of the file.
     */
    private void init() throws Exception {
        RandomAccessFile in = new RandomAccessFile(grmFile, "r");
        String line = in.readLine();
        candidate4Unkwnon = new Vector();
        while (line != null) {
            //on va recuperer les variables
            if (isPathLine(line)) {
                line = in.readLine();
                readPathDefinition(line);
            } else {
                if (isUsingVar(line)) {
                    String value = eval(line);
                    if (isUsingVar(value)) {
                        addVariableName(candidate4Unkwnon, extratVariables(value));
                    }
                }
            }
            line = in.readLine();
        }
        in.close();
    }

    /**
     *Returns the undefined variable if they exist. 
     *the variables are resolved with the grm file itself and the ProjectContext NOT with 
     *the system environment
     *@return null if there are no undefined variables, otherwise return a collection of 
     *        variable names
     * @throws java.lang.Exception 
     */
    public Collection<String> resolve() throws Exception {
        init();
        return getUnknownVariables();
    }

    /**
     *Check if a line of a grm file is a path instruction
     *after this line there will have a path definition.
     */
    private boolean isPathLine(String line) {
        Pattern p = Pattern.compile(" *path: *");
        Matcher m = p.matcher(line);
        return m.matches();
    }

    /**
     *Read path definition in the grm file.
     *a path line should be like that : ..*=..*
     *@param the line to read
     */
    private void readPathDefinition(String line) {
        Pattern pName = Pattern.compile("..*=");
        Pattern pValue = Pattern.compile("=..*");
        Matcher m = pName.matcher(line);
        Matcher mValue = pValue.matcher(line);
        VarEnv variable = new VarEnv();
        while (m.find()) {
            String var = m.group();
            var = var.substring(0, var.length() - 1);
            variable.setName(var.trim());
        }
        while (mValue.find()) {
            String value = mValue.group();
            value = value.substring(1, value.length());
            variable.setValue(value.trim());
        }
        this.variablesMap.put(variable.getName(), variable);
    }

    /**
     *Check if variable is used in a String, the value of a variable
     *is called by the $ character before a name of a variable
     *@param value the value to read
     *@return true if a variable is found
     */
    private boolean isUsingVar(String value) {
        Pattern p = Pattern.compile(".*\\$.*");
        Matcher m = p.matcher(value);
        return m.matches();
    }

    /**
     *With a collection of variable name, try to eval the value given in parameter.
     *if after this method some variable are still not evaluated it means that the variable is not defined
     *in the grm file but may be in the project context.
     *@param c a collection of variable name.
     *@param value the string to evaluate
     *@return the evaluation of the value
     */
    private String subEval(Collection<String> c, String value) {
        for (Iterator<String> it = c.iterator(); it.hasNext();) {
            String varName = it.next();
            //   System.out.println("sous evaluation de : " + value  + " avec la variable : " + varName);
            VarEnv var = variablesMap.get(varName);
            if (var != null) {
                Pattern p = Pattern.compile("\\$" + var.getName());
                Matcher m = p.matcher(value);
                String replacement = var.getValue();
                replacement = replacement.replaceAll("\\$", "\\\\\\$");
                value = m.replaceAll(replacement);
            }
        }
        return value;
    }

    /**
     *Return the defined variables in the grm file
     *@return a collection of variable
     */
    public Collection<VarEnv> getVariables() {
        return this.variablesMap.values();
    }

    /**
     *Eval a String, try to replace all variables used in the given value by the
     *value of the variable. If no variable was set, we return exactly the same value
     *@param value the String value to evaluate
     *@return the value evaluate if some variables was found or the same value as in input
     *
     */
    private String eval(String value) {
        Iterator<String> it = this.variablesMap.keySet().iterator();
        while (it.hasNext()) {
            VarEnv var = variablesMap.get(it.next());
            Pattern p = Pattern.compile("\\$" + var.getName());
            String replacement = var.getValue();
            //if the value containing a Variable, need to replace the $ by \\$
            replacement = replacement.replaceAll("\\$", "\\\\\\$");
            Matcher m = p.matcher(value);
            value = m.replaceAll(replacement);
        }
        //and of the pre evaluation
        // now a check if we need to evaluate some sub variable used
        if (isUsingVar(value)) {
            Collection c = extratVariables(value);
            //System.out.println("les variables dans : " + value + " " + c );
            value = subEval(c, value);
        }
        return value;
    }

    /***
     *This method extract the variable names used in a value.
     *this method split the value when a / or \ character are present. 
     *then for each part try to find a variable and extract only the name
     *@param value the value to check
     *@return collection of the variables names found,
     *        if no variables was found return an empty collection
     */
    private Collection<String> extratVariables(String value) {
        String[] val = value.split("\\\\|/");
        Vector<String> varUnknown = new Vector();
        for (int i = 0; i < val.length; i++) {
            Pattern p = Pattern.compile("\\$\\p{Graph}*");
            Matcher m = p.matcher(val[i]);
            while (m.find()) {
                String varName = m.group();
                varName = varName.substring(1, varName.length());
                if (!varUnknown.contains(varName)) {
                    varUnknown.add(varName);
                }
            }
        }
        return varUnknown;
    }

    /**
     *check if a variable is in the project context
     *if the variable is found, we set it in the xip environment
     *@param variableName
     */
    private boolean existInContext(String variableName) {
        boolean success = false;
        if (!variableName.equals(GLOBAL_VARIABLE)) {
            VarEnv var = context.getVariable(variableName);
            if (var != null) {
                try {
                    xip.setEnv(var.getName(), var.getValue());
                    success = true;
                } catch (Exception e) {
                    success = false;
                }
            }
        } else {
            success = true;
        }
        return success;
    }

    /**
     *Return the undefined variable in the grm file
     *it means the variable that is not in the grm file and not in the project context.
     *if the variable is in the System environment but not redefined in the file or project context, it will be 
     *added to the collection. This method sould be called after the setGrmFile()
     *@return a collection of undefined variable name, null if there no unknwon variable 
     *
     *@see setGrmFile(File)
     *@see setGrmFile(String)
     *@see resolve()
     */
    public Collection<String> getUnknownVariables() {
        //check if the unknownvariable are really undefined :
        Collection<String> undefineds = new Vector();
        for (Iterator<String> it = candidate4Unkwnon.iterator(); it.hasNext();) {
            String varName = it.next();
            //the variable is not defined in the grm file
            if (!existInContext(varName)) {
                undefineds.add(varName);

            }
        }
        if (undefineds.isEmpty()) {
            undefineds = null;
        }
        return undefineds;
    }

    /**
     *Don't use, for test only
     * @param args 
     */
    static public void main(String... args) {
        File file = new File("test.grm");
        try {
            ProjectContext context_ = new ProjectContext("tmp", "tmpTest");
            Grm grm = new Grm(new Xip(), context_);
            grm.setGrmFile(file);
            //   grm.resolve();
            System.out.println(grm.getUnknownVariables());
//            System.out.println(grm.getVariables());
//            System.out.println(grm.eval("$GRM_PATHNAME/../BASIC/ntmscript"));
            System.out.println(grm.eval("\\home\\$toto/$titifin"));
//            System.out.println(grm.eval("/home/$NTM_ROOT/truc"));
//            String value = "/home/$NTM_ROOT/truc";
//            Pattern p = Pattern.compile("\\$NTM_ROOT");
//            Matcher m = p.matcher(value);
//            System.out.println(m.matches());
//            String replace = "$troc/$muche";
//            replace = replace.replaceAll("\\$","\\\\\\$");
//            System.out.println(m.replaceAll(replace));   
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
