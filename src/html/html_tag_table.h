#ifndef HTML_TAG_TABLE_H
  #define HTML_TAG_TABLE_H
/*
  NOTE: 
    *Element* is specified as the full piece of informations of a specific content
    *Tag* is the data marking the start and end of an element
*/
typedef U32 HTMLTagType;
enum
{
  HTMLTag_None,

#define TAG(Tag, ...) HTMLTag_##Tag,
#include "html_tag_table.inl"
  
  HTMLTag_Count,
};

typedef U32 HTMLEnclosingType;
enum
{  
  HTMLEnclosingType_Null,
  HTMLEnclosingType_opening,
  HTMLEnclosingType_closing, 
};

typedef U32 HTMLTagEnclosingType;
enum
{  
  HTMLTagEnclosingType_Paired,
  HTMLTagEnclosingType_Unique,
  HTMLTagEnclosingType_Self, 
};

typedef struct HTMLTagInvariant HTMLTagInvariant;
struct HTMLTagInvariant
{
  HTMLTagType          type;
  HTMLTagEnclosingType enclosing_type;
  String8              tag_name;
  TextType             text_types;
};

typedef struct HTMLTagTable HTMLTagTable;
struct HTMLTagTable
{
  HTMLTagInvariant *tags;
  U8                tag_count;
};

#endif