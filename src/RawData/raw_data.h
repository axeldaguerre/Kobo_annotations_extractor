#ifndef RAW_DATA_H
  #define RAW_DATA_H


typedef struct Buffer;
struct Buffer
{
  U8   *data;
  U64   size;
};

typedef U32 RawHLMeaning;
enum
{
  RawHLMeaning_Null       = 0,
  
  RawHLMeaning_Contextual = (1<<0),
  RawHLMeaning_Time       = (1<<1),
};

typedef U32 RawStrenght;
enum
{
  RawStrenght_Null,
  
  RawStrenght_Lowest,
  RawStrenght_Low,
  RawStrenght_Medium,
  RawStrenght_AboveMedium,
  RawStrenght_Highest,
};

typedef struct RawMeaning;
struct RawMeaning
{
  RawStrenght       strenght;
  RawHLMeaning      high_level_meaning;
};

typedef struct RawMeaningTable;
struct RawMeaningTable
{
  RawMeaning   meanings;
  U64          count;
};


typedef void RawDataTransform(Buffer *data);

typedef struct RawData;
struct RawData
{
  U8               *buffer;
  RawDataTransform *transformer;
  RawMeaning        meaning;
  
};


typedef struct RawDataNode;
struct RawDataNode
{
  RawData *next;
  RawData  raw;
};

typedef struct RawDataList;
struct RawDataList
{
  RawDataNode *first;
  RawDataNode *last;
  U64          node_count;
};

typedef struct RawDataTransformers;
struct RawDataTransformers
{
  RawDataTransform integer_transformer;
  RawDataTransform string_transformer;
  RawDataTransform float_transformer;
};


internal RawMeaningTable*
textual_get_table(Arena *arena, ColumRawMeaning *col_raw_table, U64 size)
{
    RawMeaningTable *table = push_array(arena, RawMeaningTable, 1);
    table->meanings = col_raw_table;
    table->count = size;
    return table;
}
#endif