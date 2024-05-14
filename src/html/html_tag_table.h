#ifndef HTML_TAG_TABLE_H
  #define HTML_TAG_TABLE_H
/*
  NOTE: 
    *Element* is specified as the full piece of informations of a specific content
    *Tag* is the data marking the start and end of an element
*/
typedef U8 HTMLTagEnclosingType;
enum 
{ 
  HTMLTagEnclosingType_Null   = 0, 
  
  HTMLTagEnclosingType_Paired = (1 << 1),
  HTMLTagEnclosingType_Unique = (1 << 2),
  HTMLTagEnclosingType_Self   = (1 << 3),
};

typedef U8 TagType;
enum
{
    TagType_Null,
    TagType_Empty, // TODO: useful?  
    TagType_Inline, 
    TagType_Block,  
    TagType_Pre    // TODO: useful?
} ;

typedef struct HTMLTagInvariant HTMLTagInvariant;
struct HTMLTagInvariant
{
  U64                      tag;
  TagType                  tag_type;
  HTMLTagEnclosingType     enclosing_type;
  String8                  tag_name;
  RawMeaning               meaning;
};

internal HTMLTagInvariant 
html_tag_table[] =
{
 #include "html_tag_table.inl"
};

typedef struct HTMLTagInvariantTable HTMLTagInvariantTable;
struct HTMLTagInvariantTable
{
  HTMLTagInvariant *tags;
  U8                tag_count;
};

#endif