#ifndef RAW_DATA_H
  #define RAW_DATA_H

typedef struct Buffer Buffer;
struct Buffer
{
  void *data;
  U64  size;
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

typedef struct RawMeaning RawMeaning;
struct RawMeaning
{
  RawStrenght       strenght;
  RawHLMeaning      high_level_meaning;
};

typedef struct RawMeaningTable RawMeaningTable;
struct RawMeaningTable
{
  RawMeaning   meanings;
  U64          count;
};

typedef U8 RawDataType;
enum
{
  RawDataType_null,
  
  RawDataType_int,
  RawDataType_float,
  RawDataType_string,  
};

typedef struct RawData RawData;
struct RawData
{
  RawData                  *next;
  RawDataType               type;
  Buffer                    buffer;
  RawMeaning                meaning;  
};

typedef struct RawDataNode RawDataNode;
struct RawDataNode
{
  RawDataNode *next;
  RawData      raw;
};

typedef struct RawDataList RawDataList;
struct RawDataList
{
  RawDataNode *first;
  RawDataNode *last;
  U64          node_count;
};

#endif