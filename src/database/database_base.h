#ifndef DATABASE_BASE_H
# define DATABASE_BASE_H
// TODO: It's quite wrong, macro shouldn't exists
#define COL_TEXT(name, type) { 0, 0, {(U8 *)name, sizeof(name) - 1}, type }

typedef U32 DBError;
enum
{
  DBError_Null          = (0 << 0),
  DBError_Query         = (1 << 0),
  DBError_Connexion     = (1 << 1),
  DBError_Library       = (1 << 2),
};

typedef U32 ColumnType;
enum
{
  ColumnType_Null,
  ColumnType_Integer,
  ColumnType_Float,
  ColumnType_Text,
  ColumnType_Blob,
};

typedef struct ColumnData ColumnData;
struct ColumnData 
{
  ColumnData *next_sibbling;
  ColumnType type;
  TextType   textual_type;
  String8    value;
  String8    name;
};

typedef struct EntryDataDB EntryDataDB;
struct EntryDataDB 
{
  EntryDataDB *next_sibbling;
  ColumnData data;
};

typedef struct EntryDataDBNode EntryDataDBNode;
struct EntryDataDBNode
{
  EntryDataDBNode *next;
  EntryDataDB      entry;
};

typedef struct EntryDataDBList EntryDataDBList;
struct EntryDataDBList
{
  EntryDataDBNode *first;
  EntryDataDBNode *last;
  U64           node_count;
};

typedef U32 TypeDB;
enum
{
  TypeDB_Null,
  TypeDB_SQLITE,
};

typedef struct StateDB StateDB;
struct StateDB
{
  
  OS_Handle lib;
  B32       is_initialized;
  DBError  errors;
  TypeDB    db_type;
  U8        memory[200];
};

typedef U32 StepFlags;
enum
{
  StepFlag_Null,
  StepFlag_Row,
  StepFlag_Done,
  StepFlag_Error,
};

#endif