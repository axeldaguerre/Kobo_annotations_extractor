#ifndef DATABASE_BASE_H
# define DATABASE_BASE_H

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
typedef int                  (sqlite_column_int64) (sqlite3_stmt*, int iCol);
typedef const unsigned char *(sqlite_column_text)  (sqlite3_stmt*, int iCol);
typedef const void *         (sqlite_column_blob)  (sqlite3_stmt*, int iCol);
typedef const void *         (sqlite_column_text16)(sqlite3_stmt*, int iCol);
typedef double               (sqlite_column_double)(sqlite3_stmt*, int iCol);
typedef int                  (sqlite_column_bytes) (sqlite3_stmt*, int iCol); // not used
typedef int                  (sqlite_finalize)     (sqlite3_stmt *pStmt);
typedef const char *         (sqlite_col_name)     (sqlite3_stmt*, int N);
typedef const char *         (sqlite_errmsg)       (sqlite3 *db);
typedef int                  (sqlite_errcode)       (sqlite3 *db);

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
  sqlite_column_int64  *column_int64;
  sqlite_column_text   *column_text;
  sqlite_column_blob   *column_blob;
  sqlite_column_text16 *column_text16;
  sqlite_column_double *column_double;
  sqlite_column_bytes  *column_bytes;
  sqlite_finalize      *finalize_statement;
  sqlite_col_name      *col_name;
  sqlite_errmsg        *errmsg;
  sqlite_errcode       *errcode;
};

typedef U8 DBError;
enum 
{
  DBError_Null          = 0,
  DBError_Query         = (1 << 1),
  DBError_Connexion     = (1 << 2),
  DBError_Library       = (1 << 3),
};

typedef struct KoboColumnNameTable KoboColumnNameTable;
struct KoboColumnNameTable
{
  String8 *names;
  U64 count;
};
 
typedef struct SQLiteState SQLiteState;
struct SQLiteState 
{
  sqlite3      *db;
  sqlite3_stmt *statement;
  int           col_count;
  int           step_code;
  SQLiteAPI     api;
  OS_Handle     lib;
  B32           is_initialized;
  DBError       errors;
};
/*
  NOTE: 
    'Bookmark' table:
      - BookmarkID:  
      - VolumeID:                 Book title (RawStrenght_Highest) RawSemantic_Summary
      - ContentID:
      - StartContainerPath:
      - StartContainerChildIndex:
      - StartOffset:
      - EndContainerPath:
      - EndContainerChildIndex:
      - EndOffset:
      - Text:                      The part of the text highlighted during the annotation creation (RawSemantic_Details)
      - Annotation:                annotation title, User input when creating annotation (RawStrenght_High) RawSemantic_Summary
      - ExtraAnnotationData:
      - DateCreated:
      - ChapterProgress:
      - Hidden:
      - Version:
      - DateModified:
      - Creator:
      - UUID:
      - UserID:
      - SyncTime:
      - Published:
      - ContextString:
      - Type:		
      
      *All other columns from Bookmark are RawStrenght_AboveMedium & RawSemantic_Details*
*/

/*
    NOTE: -"Annotation" column is text written by Kobbo User when adding the annotation .
          -"VolumeID" column is the book title in which the annotation was written .
          -"Text" column is the highlighted text while adding the annotation .
          - All other columns from Bookmark are (RawStrenght_BelowMedium)
  */
#endif