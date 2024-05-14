#ifndef BASE_STRING_H
#define BASE_STRING_H

// String Types
typedef struct String8 String8;
struct String8 
{
  U8 *str;
  U64 size;
};
  
typedef struct String16 String16;
struct String16 
{
  U16 *str;
  U64 size;
};

// Unicode 

typedef struct UnicodeDecode UnicodeDecode;
struct UnicodeDecode
{
  U32 codepoint;
  U32 inc;
};

// List & Array Types 

typedef struct String8Node String8Node;
struct String8Node
{
  String8Node *next;
  String8     string;
};

typedef struct String8List String8List;
struct String8List
{
  String8Node *first;
  String8Node *last;
  U64         node_count;
  U64         total_size;
};

typedef struct String8Array String8Array;
struct String8Array
{
  String8 *strings;
  U64     count;
};
////////////////////////////////
//~ Axel: String Matching, Splitting, & Joining Types

typedef struct StringJoin StringJoin;
struct StringJoin
{
  String8 pre;
  String8 sep;
  String8 post;
};

typedef U32 StringMatchFlags;
enum
{
  StringMatchFlag_CaseInsensitive  = (1 << 0),
  StringMatchFlag_RightSideSloppy  = (1 << 1),
  StringMatchFlag_SlashInsensitive = (1 << 2),
  
};

typedef U32 StringSplitFlags;
enum
{
  StringSplitFlag_None = (1 << 0),
  StringSplitFlag_KeepEmpties = (1 << 1),
};

typedef enum PathStyle
{
  PathStyle_Relative,
  PathStyle_WindowsAbsolute,
  PathStyle_UnixAbsolute,
  
#if OS_WINDOWS
  PathStyle_SystemAbsolute = PathStyle_WindowsAbsolute
#elif OS_LINUX
  PathStyle_SystemAbsolute = PathStyle_UnixAbsolute
#else
# error "absolute path style is undefined for this OS"
#endif
} PathStyle;

read_only global U8 integer_symbols[16] = {
  '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F',
};

internal B32           char_is_upper(U8 c);
internal B32           char_is_lower(U8 c);
internal B32           char_is_alpha(U8 c);
internal B32           char_is_digit(U8 c, U32 base);
internal B32           char_is_whitespace(U8 c);
internal U8            char_to_upper(U8 c);
internal U8            char_to_lower(U8 c);
internal U8            char_to_correct_slash(U8 c);

internal B32           str8_ends_with(String8 string, String8 end, StringMatchFlags flags);
internal B32           str8_match(String8 a, String8 b, StringMatchFlags);

internal String16      str16(U16 *str, U64 size);
internal U64           cstr16_length(U16 *str);
internal String16      str16_cstring(U16 *str);
internal String16      str16_from_str8(Arena *arena, String8 str);
internal String8       str8(U8 *str, U64 size);
internal String8       str8_cstring(char *c);

internal U64           cstr8_length(U8 *str);

internal String8       str8_from_16(Arena *arena, String16 str);
internal U32           utf8_encode(U8 *str, U32 codepoint);

internal String8       push_str8_copy(Arena *arena, String8 string);
internal String8       push_str8_cat(Arena *arena, String8 string, String8 cat);

internal String8Node*  str8_list_push(Arena *arena, String8List *list, String8 str);
internal String8       str8_list_join(Arena *arena, String8List *list, StringJoin *optional_params);

internal String8       str8_postfix(String8 str, U64 size);
internal String8       str8_from_16(Arena *arena, String16 utf16);
internal String8       str8_chop_last_slash(String8 str);

internal String8List   str8_split(Arena *arena, String8 string, U8 *split_chars, U64 split_char_count, StringSplitFlags flags);
internal String8List   str8_split_path(Arena *arena, String8 string);
internal String8List   str8_split_by_string_chars(Arena *arena, String8 string, String8 split_chars, StringSplitFlags flags);

internal String8       str8_range(U8 *first, U8 *one_past_last);
internal String8       str8_cut_from_last_dot(String8 string);
internal U64           str8_find_needle(String8 string, U64 start_pos, String8 needle, StringMatchFlags flags);
internal String8       str8_chop_last_dot(String8 string);
internal String8       str8_prefix(String8 str, U64 size);

internal S64           sign_from_str8(String8 string, String8 *string_tail);

internal String8       push_str8fv(Arena *arena, char *fmt, va_list args);
internal String8       push_str8f(Arena *arena, char *fmt, ...);

internal String8       cstyle_hex_from_u64(Arena *arena, U64 x, B32 caps);
internal S64           s64_from_str8(String8 string, U32 radix);
internal F64           f64_from_str8(String8 string);

internal String8       str8_from_u64(Arena *arena, U64 u64, U32 radix, U8 min_digits, U8 digit_group_separator);
internal String8       str8_from_s64(Arena *arena, S64 s64, U32 radix, U8 min_digits, U8 digit_group_separator);

internal U64           cstring8_length(U8 *c);
internal String8       str8_skip_chop_whitespace(String8 string);
internal String8       str8_substr(String8 str, Rng1U64 range);

#define str8_lit(S)      str8((U8*)(S), sizeof(S) - 1)
#define str8_lit_comp(S) {(U8*)(S), sizeof(S) - 1}
#define str16_lit(S)     str16((U16*)(S), sizeof(S) - 1)

#endif  