/*
 * Copyright Xerox 2009
 */


// Using an embedded database rather than a database server (e.g. MySQL)
// for the time being because it's easier to set up.  Currently using
// Derby simply because it's included in the Sun JDK. The received opinion
// on the web seems to be that HSQLDB is better, can always try that later.
// For large databases (over 8GB?) will probably want to switch to
// a client/server db.

package com.xerox.xrce.xip2jena.storageconfigs;

import com.xerox.xrce.xip2jena.*;
import com.hp.hpl.jena.sdb.store.DatabaseType;

public class DbConfigDerby implements DbConfig {
    private String path;

    public DbConfigDerby (String path) {
        this.path = path;
    }

    @Override
    public DatabaseType getDbType() {
        return DatabaseType.Derby;
    }

    @Override
    public String getDbClass() {
        return "org.apache.derby.jdbc.EmbeddedDriver";
    }

    @Override
    public String getDbUrl() {
        return "jdbc:derby:"+path+";create=true";
    }

    @Override
    public String getDbUser() {
        return "";
    }

    @Override
    public String getDbPw() {
        return "";
    }

}
