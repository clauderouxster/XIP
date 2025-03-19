/*
 * Copyright Xerox 2009
 */

package com.xerox.xrce.xip2jena.storageconfigs;

import com.xerox.xrce.xip2jena.*;
import com.hp.hpl.jena.sdb.store.DatabaseType;

public class DbConfigMysql implements DbConfig {
    @Override
    public DatabaseType getDbType() {
        return DatabaseType.MySQL;
    }

    @Override
    public String getDbClass() {
        return "com.mysql.jdbc.Driver";
    }

    @Override
    public String getDbUrl() {
        return "jdbc:mysql://localhost:3306/xip2jenarisk";
    }

    @Override
    public String getDbUser() {
        return "x2jwriter";
    }

    @Override
    public String getDbPw() {
        return "";
    }

}
