/* mtest.c - memory-mapped database tester/toy */
/*
 * Copyright 2011 Howard Chu, Symas Corp.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted only as authorized by the OpenLDAP
 * Public License.
 *
 * A copy of this license is available in the file LICENSE in the
 * top-level directory of the distribution or, alternatively, at
 * <http://www.OpenLDAP.org/license.html>.
 */
#define _XOPEN_SOURCE 500		/* srandom(), random() */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifdef _WIN32
#  include "win32compability.h"
#endif
#include "lmdb.h"


int main(int argc,char * argv[])
{
	int i = 0, j = 0, rc;
	MDB_env *env;
	
	MDB_val key, data;
	
	MDB_stat mst;
	MDB_cursor *cursor;
	int count;
	int *values;
	char sval[32];
	size_t v=40000000000UL;

	srandom(time(NULL));

	printf("%d:%d:%d:%d:%d",sizeof(size_t),sizeof(long),sizeof(int),sizeof(_int64),sizeof(long long));

	    count = 1000;
	    values = (int *)malloc(count*sizeof(int));

	    for(i = 0;i<count;i++) {
			values[i] = i;
	    }
    
		
		rc = mdb_env_create(&env);
		rc = mdb_env_set_mapsize(env, v);
		rc = mdb_env_open(env, "C:\\XIP\\Test\\lmdb\\mydb", 0, 0664);

		if (rc) {
			char* s=mdb_strerror(rc);
			printf("Error:%s",s);
			exit(-1);
		}
   
		key.mv_size = sizeof(sval);
		key.mv_data = sval;
		data.mv_size = sizeof(sval);
		data.mv_data = sval;

		printf("Adding %d values\n", count);
	    for (i=0;i<count;i++) {	
			MDB_txn *txn;
			MDB_dbi dbi;

			rc = mdb_txn_begin(env, NULL, 0, &txn);
			rc = mdb_open(txn, NULL, 0, &dbi);
			
			sprintf(sval, "%03x %d foo bar", values[i], values[i]);
			rc = mdb_put(txn, dbi, &key, &data, 0);

			rc = mdb_txn_commit(txn);
			mdb_close(env, dbi);

			printf("%i\n", i);
	    }

		mdb_env_close(env);

	return 0;
}
