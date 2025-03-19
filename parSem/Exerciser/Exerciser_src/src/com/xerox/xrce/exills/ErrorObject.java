/** 
 ****************************************************************************************
 * Copyright (c) 2005 Xerox Corporation                                                 *
 * Xerox Research Centre Europe - Grenoble                                              *
 * All Rights Reserved                                                                  *
 * Copyright protection claimed includes all forms and matters of copyrightable         *
 * material and information now allowed by statutory or judicial law or                 *
 * hereinafter granted, including without limitation, material generated from           *
 * the software programs which are displayed on the screen such as icons,               *
 * screen display looks, etc. All rights reserved. No part of this document             *
 * may be reproduced, stored in a retrieval system, or transmitted, in any              *
 * form or by any means, electronic, mechanical, photocopied, recorded, or              *
 * otherwise, without prior written consent from XEROX corporation.                     *
 ****************************************************************************************
 */

package com.xerox.xrce.exills;


import java.util.*;
import java.io.*;


/**
 * this bean permit to write some errors from action in a jsp file.
 * Also, this bean has a key, this key refer to a resource file in order to present 
 * the name of an error in several languages
 * @author vgrassau
 */
public class ErrorObject implements Serializable {
    
    private String key;
    private String name;
    private Exception exception;
    
    /** the default error */
    public ErrorObject() {
        setKey("errors.development");
        setException(new Exception());
    }
    
    /** intialize the exception 
     @param e exception
     */
    public void setException(Exception e) {
        this.exception = e;
    }
    
    /**
     *Initialize the key for this error
     *@param key the key
     */
    public void setKey(String key) {
        this.key = key;
        this.name = ResourceBundle.getBundle("com/xerox/xrce/exills/resource/ApplicationResource").getString(key);
    }
    
    /**
     *Return the exception of this error
     *@return exception in order to know cause and message error
     */
    public Exception getException() {
        return this.exception;
    }
    
    /**
     *Return the key of this error
     *@return a key
     */
    public String getKey() {
        return this.key;
    }
    
    /**
     *Return the name, value of the key
     *@return 
     */
    public String getName() {
        return this.name;
    }
    
    /** Creates a new instance of ErrorObject 
     *@param key the key refer to the application resource
     */
    public ErrorObject(String key) {
        setKey(key);
    }
    
    
    /**
     creates a instance of ErrorObject with 2 parameters
     *@param key the key of the resource file to give a name
     *@param e Exception which was catched 
     */
    public ErrorObject(String key,Exception e) {
        setKey(key);
        setException(e);
    }
    
}
