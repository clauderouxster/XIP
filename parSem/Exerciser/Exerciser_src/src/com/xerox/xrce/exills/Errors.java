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
 * This class extends the class of Vector in order to specify some operation on 
 * a list of errorObjects.
 * @author vgrassau
 */
public class Errors extends Vector implements Serializable {
    
    /** Creates a new instance of Errors */
    public Errors() {
     super(); //calls the Vector() constructor
    }
    
    /**
     *Add a ErrorObject in the vector 
     *@param error the error to insert
     */
    public void addErrorObject(ErrorObject error) {
        add(error);
    }
    
    
    
}//end of the class
