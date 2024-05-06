#ifndef DATABASE_BASE_H
# define DATABASE_BASE_H
// TODO: It's quite wrong, macro shouldn't exists
#define COL_TEXT(name, type) { 0, 0, {(U8 *)name, sizeof(name) - 1}, type }

typedef U8 DBError;
enum 
{
  DBError_Null          = 0,
  DBError_Query         = (1 << 1),
  DBError_Connexion     = (1 << 2),
  DBError_Library       = (1 << 3),
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

typedef struct ColumRawMeaning ColumRawMeaning;
struct ColumRawMeaning 
{
  String8    col_name;
  RawMeaning meaning;
};

typedef struct ColumRawMeaningTable ColumRawMeaningTable;
struct ColumRawMeaningTable 
{
  ColumRawMeaning *col_meanings;
  U64             count;
};

typedef struct ColumnDataDB ColumnDataDB;
struct ColumnDataDB 
{
  ColumnDataDB *next_sibbling;
  ColumnType type;
  String8    col_name;
  RawDataa    raw;
};

typedef struct ColumnDataDBNode ColumnDataDBNode;
struct ColumnDataDBNode 
{
  ColumnDataDBNode *next;
  ColumnDataDB      entry;
};

typedef struct ColumnDataDBList ColumnDataDBList;
struct ColumnDataDBList 
{
  ColumnDataDBNode *first;
  ColumnDataDBNode *last;
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
  DBError   errors;
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

typedef struct DBRawDataTransformers DBRawDataTransformers;
struct DBRawDataTransformers
{
  raw_data_transform_func *transformer_int;
  raw_data_transform_func *transformer_float;
  raw_data_transform_func *transformer_blob;
  raw_data_transform_func *transformer_string;
};

internal ColumRawMeaningTable*
database_get_col_raw_meaning_table(Arena *arena, ColumRawMeaning *col_raw_table, U64 size)
{
    ColumRawMeaningTable *table = push_array(arena, ColumRawMeaningTable, 1);
    table->col_meanings = col_raw_table;
    table->count = size;
    
    return table;
}
#endif