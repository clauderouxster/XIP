/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
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
package com.xerox.xrce.xipui.exception;

/**
 *
 * @author Vianney Grassaud
 */
public class UnknownVarEnvException extends Exception {

    public final String variableName;
    public static final String msg = " is unknown, you should create it and read again your environment.";

    /** Creates a new instance of UnknownVarEnvException
     * @param varName 
     */
    public UnknownVarEnvException(String varName) {
        super(varName + msg);
        variableName = varName;
    }

    public UnknownVarEnvException(UnknownVarEnvException e) {
        super(e.variableName + msg);
        variableName = e.variableName;
    }
}
