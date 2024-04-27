#ifndef DATABASE_SQLITE_H
# define DATABASE_SQLITE_H

#include "third_party/sqlite-amalgamation/sqlite3.h"

typedef int                  (sqlite_open)         (const char *filename, sqlite3 **ppDb);
typedef int                  (sqlite_close)        (sqlite3 *);
typedef int                  (sqlite_exec)         (sqlite3*, const char *sql, int (*callback)(void*,int,char**,char**), void *, char **errmsg) ;
typedef int                  (sqlite_prepare)      (sqlite3 *db, const char *zSql, int nByte, sqlite3_stmt **ppStmt, const char **pzTail);
typedef int                  (sqlite_step)         (sqlite3_stmt* stmt);
typedef int                  (sqlite_column_type)  (sqlite3_stmt* stmt, int iCol);
typedef sqlite3_value*       (sqlite_col_value)    (sqlite3_stmt*, int iCol);
typedef int                  (sqlite_bind_int)     (sqlite3_stmt*, int, int);
typedef int                  (sqlite_col_count)    (sqlite3_stmt*);
typedef int                  (sqlite_column_int)   (sqlite3_stmt*, int iCol);
typedef const unsigned char *(sqlite_column_text)  (sqlite3_stmt*, int iCol);
typedef const void*          (sqlite_column_text16)(sqlite3_stmt*, int iCol);
typedef double               (sqlite_column_double)(sqlite3_stmt*, int iCol);
typedef int                  (sqlite_column_bytes) (sqlite3_stmt*, int iCol); // not used
typedef int                  (sqlite_finalize)     (sqlite3_stmt *pStmt);
typedef const char *         (sqlite_col_name)     (sqlite3_stmt*, int N);

typedef struct SQLiteAPI SQLiteAPI;
struct SQLiteAPI
{
  sqlite_open          *open_db; 
  sqlite_close         *close_db;
  sqlite_exec          *exec_query;
  sqlite_prepare       *prepare_query;
  sqlite_step          *step_query; 
  sqlite_column_type   *column_type;
  sqlite_col_value     *column_value;
  sqlite_bind_int      *bind_int;
  sqlite_col_count     *column_count;
  sqlite_column_int    *column_int;
  sqlite_column_text   *column_text;
  sqlite_column_text16 *column_text16;
  sqlite_column_double *column_double;
  sqlite_column_bytes  *column_bytes;
  sqlite_finalize      *finalize_statement;
  sqlite_col_name      *col_name;
};

 
typedef struct SQLiteState SQLiteState;
struct SQLiteState 
{
  sqlite3      *db;
  sqlite3_stmt *statement;
  SQLiteAPI     api;
};

#endif
