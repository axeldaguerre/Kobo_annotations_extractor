#ifndef HTML_TAG_TABLE_H
  #define HTML_TAG_TABLE_H
/*
  NOTE: 
    *Element* is specified as the full piece of informations of a specific content
    *Tag* is the data marking the start and end of an element
*/


enum HTMLTagEnclosingType: U32
{ 
  HTMLTagEnclosingType_Null   = 0, 
  
  HTMLTagEnclosingType_Paired = (1 << 1),
  HTMLTagEnclosingType_Unique = (1 << 2),
  HTMLTagEnclosingType_Self   = (1 << 3),
};

typedef struct HTMLTagInvariant HTMLTagInvariant;
struct HTMLTagInvariant
{
  // TODO: is it ok to not having an enum ?
  U64          type;
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