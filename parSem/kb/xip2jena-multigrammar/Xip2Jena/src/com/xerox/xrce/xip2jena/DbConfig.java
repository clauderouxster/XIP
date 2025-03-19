/*
 * Copyright Xerox 2009
 */

package com.xerox.xrce.xip2jena;

import com.hp.hpl.jena.sdb.store.DatabaseType;

public interface DbConfig {
    public DatabaseType getDbType();
    public String getDbClass();
    public String getDbUrl();
    public String getDbUser();
    public String getDbPw();
}
