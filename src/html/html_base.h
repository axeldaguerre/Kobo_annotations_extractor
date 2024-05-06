#ifndef HTML_BASE_H
#define HTML_BASE_H

#define UNIQUE_TAGS HTMLTagType_MAIN|HTMLTagType_DOCTYPE|HTMLTagType_HTML|HTMLTagType_BODY|HTMLTagType_FOOTER
// TODO: better names
typedef U32 RawTokenType;
enum
{
  RawTokenType_null                           = 0,
  
  RawTokenType_whitespace                     = (1 << 0),
  RawTokenType_angle_bracket_open             = (1 << 1),
  RawTokenType_angle_bracket_close            = (1 << 2),
  RawTokenType_angle_bracket_open_then_slash  = (1 << 3),
  RawTokenType_angle_slash_then_bracket_close = (1 << 4),
  RawTokenType_slash                          = (1 << 5),
};

typedef struct HTMLToken HTMLToken;
struct HTMLToken
{
  RawTokenType type;
  Rng1U64      range;
};

typedef struct HTMLTag HTMLTag;
struct HTMLTag
{  
  /*
    TODO: Get rid of the text_type, user should be able to define them. HTML spec is very 
          dense and hard to code on tag meaning, some tags have different meaning and are 
          sometimes obscure. 
  */
  U64                  type;
  HTMLTagEnclosingType enclosing_type;
  HTMLToken            first_token;
  HTMLToken            last_token;
  String8              tag_name;
  TextType             text_types;
};

typedef struct HTMLElement HTMLElement;
struct HTMLElement
{
  
  HTMLElement *first_sub_element;
  HTMLElement *next_sibbling;
  HTMLTag     *tags[2];
  String8      content;
  U8           level_deep;
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

typedef U32 HTMLErrorType;
enum
{
  HTMLErrorType_Null                 = (0),
  
  HTMLErrorType_unexpected_token     = (1 << 1),
  HTMLErrorType_wrong_enclosing_type = (2 << 1),
};


typedef struct HTMLError HTMLError;
struct HTMLError
{
  // TODO: Thanks to the messages push, we should may be able to delete others
  HTMLErrorType type;
  String8List  *messages;
  U64 at;
};

typedef struct HTMLParser HTMLParser;
struct HTMLParser
{
  String8       string;
  U64           skip_until_tag_type;
  HTMLError     error;
  U8            level_deep;
  U8            space_by_indent;
  U64           at;
};

typedef struct ReadingContent ReadingContent;
struct ReadingContent
{
  HTMLElementList html;
  String8          text;
};

#endif
