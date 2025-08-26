#include <stdio.h>
#include <string.h>
#include "../../sqlite3.h"

static int openDatabaseMinimal(
    const char *zFilename, 
    sqlite3 **ppDb,        
    unsigned int flags,    
    const char *zVfs       
  ){
    sqlite3 *db;                    
    int rc;                         
    int isThreadsafe;               
    char *zOpen = 0;                
    char *zErrMsg = 0;              
    int i; 

    *ppDb = 0;

    isThreadsafe = 0;
    rc = sqlite3_initialize();
    if (rc) return rc;

    db = sqlite3MallocZero( sizeof(sqlite3) );
    if( db==0 ) goto opendb_out;

    db->errMask = (flags & SQLITE_OPEN_EXRESCODE)!=0 ? 0xffffffff : 0xff;
    db->nDb = 2;
    db->eOpenState = SQLITE_STATE_BUSY;
    db->aDb = db->aDbStatic;
    db->autoCommit = 1;
    db->nextAutovac = -1;
    db->nextPagesize = 0;
    db->lookaside.bDisable = 1;
    db->lookaside.sz = 0;
    db->init.azInit = sqlite3StdType; 

    db->flags |= SQLITE_ShortColNames
    | SQLITE_EnableTrigger
    | SQLITE_EnableView
    | SQLITE_CacheSpill;

    sqlite3HashInit(&db->aCollSeq);
    createCollation(db, "BINARY", SQLITE_UTF8, 0, binCollFunc, 0); 

    if( db->mallocFailed ) goto opendb_out;

    if( zFilename==0 ) zFilename = ":memory:";
    rc = sqlite3ParseUri(zVfs, zFilename, &flags, &db->pVfs, &zOpen, &zErrMsg);
    if( rc!=SQLITE_OK ){
        sqlite3_free(zErrMsg);
        goto opendb_out;
    }

    rc = sqlite3BtreeOpen(db->pVfs, zOpen, db, &db->aDb[0].pBt, 0, flags);
    if( rc!=SQLITE_OK ){
        goto opendb_out;
    }

    sqlite3BtreeEnter(db->aDb[0].pBt);
    db->aDb[0].pSchema = sqlite3SchemaGet(db, db->aDb[0].pBt);
    if( !db->mallocFailed ){
        sqlite3SetTextEncoding(db, SCHEMA_ENC(db));
    }
    sqlite3BtreeLeave(db->aDb[0].pBt);
    db->aDb[1].pSchema = sqlite3SchemaGet(db, 0);

    db->eOpenState = SQLITE_STATE_OPEN;
    sqlite3Error(db, SQLITE_OK);

opendb_out:
    if( db ){
        assert( db->mutex!=0 || isThreadsafe==0
            || sqlite3GlobalConfig.bFullMutex==0 );
        sqlite3_mutex_leave(db->mutex);
    }
    rc = sqlite3_errcode(db);
    assert( db!=0 || (rc&0xff)==SQLITE_NOMEM );
    if( (rc&0xff)==SQLITE_NOMEM ){
        sqlite3_close(db);
        db = 0;
    }else if( rc!=SQLITE_OK ){
        db->eOpenState = SQLITE_STATE_SICK;
    }
    *ppDb = db;
    
    sqlite3_free_filename(zOpen);
    return rc;
}
