#ifndef BASE_RAW_H
  #define BASE_RAW_H
  
typedef U32 RawSemantic;
enum 
{
  RawSemantic_Null         = 0,
  
  RawSemantic_Summary      = (1<<0),
  RawSemantic_Details      = (1<<1),
  RawSemantic_Link         = (1<<2),
  RawSemantic_Time         = (1<<3),
  RawSemantic_Number       = (1<<4),
  RawSemantic_Step         = (1<<5),
  RawSemantic_Visual       = (1<<6),
};

typedef U32 RawStrenght;
enum
{
  RawStrenght_Null,
  
  RawStrenght_Lowest,
  RawStrenght_Low,
  RawStrenght_BelowMedium,
  RawStrenght_Medium,
  RawStrenght_AboveMedium,
  RawStrenght_High,
  RawStrenght_Highest,
};

typedef struct RawMeaning RawMeaning;
struct RawMeaning
{
  RawStrenght strenght;
  RawSemantic semantic_flags;
};

typedef struct RawData RawData;
struct RawData
{  
  String8     data;
  RawMeaning  meaning;  
};

/*
  TODO: because the memory used by the structure can be high
          why not use an entire "huge" arena
*/
typedef struct RawDataNode RawDataNode;
struct RawDataNode
{
  RawDataNode   *parent;    /* Tree links */
  RawDataNode   *next;  
  RawDataNode   *prev;
  RawDataNode   *first;
  RawDataNode   *last;
  
  RawData        raw;
};


// NOTE: https://www.rfleury.com/p/the-easiest-way-to-handle-errors
read_only global RawDataNode raw_node_g_nil =
{
  /* Tree links */
  &raw_node_g_nil,
  &raw_node_g_nil,
  &raw_node_g_nil,
  &raw_node_g_nil,
  &raw_node_g_nil,
  
  0,
};

#endif