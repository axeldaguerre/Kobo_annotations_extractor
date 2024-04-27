#ifndef HTML_BASE_H
#define HTML_BASE_H

typedef U32 RawTokenType;
enum
{
  RawTokenType_null                           = (1 << 0),
  
  RawTokenType_dummy                          = (1 << 1),
  RawTokenType_whitespace                     = (1 << 2),
  RawTokenType_angle_bracket_open             = (1 << 3),
  RawTokenType_angle_bracket_close            = (1 << 4),
  RawTokenType_angle_bracket_open_then_slash  = (1 << 5),
  RawTokenType_angle_slash_then_bracket_close = (1 << 6),
  RawTokenType_slash                          = (1 << 7),
  
  RawTokenType_Error                          = (1 << 30),
};

#define FIRST_TOKEN_TAG_FLAGS (RawTokenType_angle_bracket_open | RawTokenType_angle_bracket_open_then_slash)
#define LAST_TOKEN_TAG_FLAGS (RawTokenType_angle_slash_then_bracket_close | RawTokenType_angle_bracket_close)

typedef struct HTMLToken HTMLToken;
struct HTMLToken
{
  RawTokenType type;
  String8      string;
};

typedef struct HTMLTag HTMLTag;
struct HTMLTag
{  
  HTMLTagType          type;
  HTMLTagEnclosingType enclosing_type;
  String8              tag_name;
  TextType             text_types;
  Rng1U64              range[2];
};

typedef struct HTMLElement HTMLElement;
struct HTMLElement
{
  HTMLElement *first_sub_element;
  HTMLElement *next_sibbling;
  HTMLTag      tag;
  String8      content;
};

typedef struct HTMLElementNode HTMLElementNode;
struct HTMLElementNode
{
  HTMLElementNode *next;
  HTMLElement      element;
};

typedef struct HTMLElementList HTMLElementList;
struct HTMLElementList
{
  HTMLElementNode *first;
  HTMLElementNode *last;
  U64              node_count;
};

typedef struct HTMLElementArray HTMLElementArray;
struct HTMLElementArray
{
  HTMLElement *v;
  U64 count;
};

typedef U32 HTMLErrorType;
enum
{
  HTMLErrorType_Null                 = (0),
  
  HTMLErrorType_unexpected_token     = (0 << 1),
  HTMLErrorType_wrong_enclosing_type = (1 << 1),
};


typedef struct HTMLError HTMLError;
struct HTMLError
{
  HTMLErrorType type;
  U64           at;
};

typedef struct HTMLParser HTMLParser;
struct HTMLParser
{
  String8   string;
  HTMLError error;
  U8        level_deep;
  U8        space_by_indent;
  U64       at;
};

typedef struct ReadingContent ReadingContent;
struct ReadingContent
{
  HTMLElementList html;
  String8          text;
};

#endif
