#ifndef TEXTUAL_BASE_H
# define TEXTUAL_BASE_H


  typedef U32 TextType;
  enum
  {
    TextType_Null          = (1 << 0),

    TextType_Unmeaningfull = (1 << 1),
    TextType_Title         = (1 << 2),
    TextType_Heading1      = (1 << 3),
    TextType_Heading2      = (1 << 4),
    TextType_Heading3      = (1 << 5),
    TextType_Paragraph     = (1 << 6),
    TextType_Link          = (1 << 7),
    TextType_Structural    = (1 << 8),
    TextType_Esthetic      = (1 << 9),
    TextType_Visual        = (1 << 10),
    TextType_Code          = (1 << 11),
    TextType_Date          = (1 << 12),
    TextType_Parent_List   = (1 << 13),
    TextType_Item_List     = (1 << 14),
  };

typedef struct TextualFormat TextualFormat;
struct TextualFormat
{
  String8 pre;
  String8 post;
};

typedef struct Textual Textual;
struct Textual
{
  Textual      *first_sub_textual;
  Textual      *next_sibbling;
  String8       text;
  TextType      type;
  TextualFormat join;
};

typedef struct TextualNode TextualNode;
struct TextualNode
{
  TextualNode *next;
  Textual     textual;
};

typedef struct TextualList TextualList;
struct TextualList
{
  TextualNode *first;
  TextualNode *last;
  U64         node_count;
};
// TODO: Use it for file html parsing
// TODO: it will not belong here
// TODO: better naming
typedef U32 FileFormatType;
enum
{
  FileFormatType_Null  = (1 << 0),
  FileFormatType_HTML  = (1 << 1),
  FileFormatType_TXT   = (1 << 2),
};

typedef struct TextualTable TextualTable;
struct TextualTable
{
  Textual *textuals;
  U64       count;
};

internal TextualTable*
textual_get_table(Arena *arena, Textual *raw_table, U64 size)
{
    TextualTable *table = push_array(arena, TextualTable, 1);
    table->count = size;
    table->textuals = raw_table;
    // NOTE: Check the table
    B32 has_title = 0;
    for(U64 idx = 0; idx < size; ++idx)
    {
      if(raw_table[idx].type == TextType_Title)
      {
        if(has_title)
        {
          // TODO: handle the error correctly (currently still compute)
          printf("ERROR: table has more than one title type\n");
          break;
        }
      }
    }
    return table;
}
#endif