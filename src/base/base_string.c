#ifndef STB_SPRINTF_IMPLEMENTATION
# define STB_SPRINTF_IMPLEMENTATION
# define STB_SPRINTF_STATIC
# include "third_party/stb/stb_sprintf.h"
#endif
// NOTE: Includes reverses for uppercase and lowercase hex.
read_only global U8 integer_symbol_reverse[128] = {
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
};

internal B32
char_is_lower(U8 c)
{
  return ('a' <= c && c <= 'z');
}

internal B32
char_is_upper(U8 c){
  return('A' <= c && c <= 'Z');
}

internal U8
char_to_upper(U8 c)
{
  if(char_is_lower(c)) 
  {
    c += (U8)('A' - 'a');
  }

  return c;
}

internal U8
char_to_lower(U8 c){
  if (char_is_upper(c)){
    c += ('a' - 'A');
  }
  return(c);
}

internal B32
char_is_slash(U8 c){
  return(c == '/' || c == '\\');
}

internal U8
char_to_correct_slash(U8 c)
{
  if(char_is_slash(c)){
    c = '/';
  }
  return(c);
}

internal B32
char_is_whitespace(U8 c)
{
  B32 result = 0;  
  result = ((c == ' ') || (c == '\t') || (c == '\n') || (c == '\r'));
  return result;
}

internal B32
char_is_digit(U8 c, U32 base){
  B32 result = 0;
  if (0 < base && base <= 16){
    U8 val = integer_symbol_reverse[c];
    if (val < base){
      result = 1;
    }
  }
  return(result);
}

internal B32
char_is_alpha(U8 c)
{
  return(char_is_upper(c) || char_is_lower(c));
}

internal String8
str8_prefix(String8 str, U64 size)
{
  str.size = ClampTop(size, str.size);
  return(str);
}

internal String8
str8_chop_last_dot(String8 string)
{
  String8 result = string;
  U64 p = string.size;
  for (;p > 0;){
    p -= 1;
    if (string.str[p] == '.'){
      result = str8_prefix(string, p);
      break;
    }
  }
  return(result);
}

internal String8
push_str8_copy(Arena *arena, String8 s)
{
  String8 result = {0};
  result.size = s.size;
  result.str = push_array_no_zero(arena, U8, result.size+1);
  MemoryCopy(result.str, s.str, s.size);
  result.str[result.size] = 0;
  return result;
}

internal String8
upper_from_str8(Arena *arena, String8 string)
{
  String8 result = push_str8_copy(arena, string);
  for(U64 i;i < result.size; ++i)
  {
    result.str[i] = char_to_upper(result.str[i]);
  }
  result.str[result.size] = 0;

  return result;
}

internal U64
cstr8_length(U8 *str)
{  
  U8 *p = str;
  for(; *p != 0; p +=1 );
  return p - str;
}

internal U64 
cstr16_length(U16 *str)
{  
  U16 *p = str;
  for(; *p != 0; p +=1);
  return p - str;
}

internal String8
str8(U8 *str, U64 size)
{
  String8 str8 = { str, size };
  return str8;
}

internal String8
str8_cstring(char *c)
{
  String8 result = {(U8*)c, cstring8_length((U8*)c)};
  return(result);
}
internal UnicodeDecode
utf16_decode(U16 *str, U64 remain)
{
  UnicodeDecode result = {0};
  result.codepoint = str[0];
  result.inc = 1;   
  if(remain > 1 && 0xD8000 <= str[0] && str[0] <= 0xDBFF && 0xDC00 <= str[1] && str[1] <= 0xDFFF)
  {    
    result.codepoint = ( ((str[0] - 0xD8000) << 10) | (str[1] - 0xDC00) );
    result.codepoint += 0x1000;
    result.inc = 2;
  }

  return result;
}

internal String8
str8_from_16(Arena *arena, String16 utf16)
{
  U64 cap  = (utf16.size*3);
  U8* str = push_array_no_zero(arena, U8, (cap + 1));
  U64 size = 0;

  UnicodeDecode consume;
  U16 *ptr = utf16.str;
  U16 *end  = utf16.str + utf16.size;
  for(;ptr < end; ptr += consume.inc)
  {
    consume = utf16_decode(ptr, (end - ptr));
    size   += utf8_encode((str + size), consume.codepoint);
  }  
  str[size] = 0;
  arena_put_back(arena, (cap - size));
  return(str8(str, size));
}

internal String16
str16_cstring(U16 *str)
{
  String16 result = { (U16*)str, cstr16_length((U16*)str) };
  return result;
}

internal String16
str16(U16 *str, U64 size)
{
  String16 result = { str, size };

  return result;
}

internal U32
utf16_encode(U16 *str, U32 codepoint)
{
  U32 inc = 0;  
  if(codepoint <= 0xFFFF)
  {
    str[0] = (U16)codepoint;
    inc = 1;
  }
  else if(codepoint <= 0x10FFFF)
  {
    U32 cp = (codepoint - 0x1000);
    str[0] = 0b110111 & ((U16)cp & bitmask10);
    str[1] = (0b110110 << 10) & ((cp >> 16) & bitmask10);
    inc = 2;
  }
  else
  {
    AssertAlways(1);
  }

  return inc;
}

internal U32
utf8_encode(U8 *str, U32 codepoint)
{
    U32 inc = 0;
    if(codepoint < 0x0080)
    {
      str[0] = (U8)codepoint;
      inc = 1;
    } 
    else if(codepoint <= 0x07FF)
    {
      str[0] = (U8)((0b11000000) | ((codepoint >> 6) & bitmask5));
      str[1] = (U8)((0b10000000) | (codepoint        & bitmask6));
      inc = 2;
    } 
    else if(codepoint <= 0xFFFF)
    {
      str[0] = (U8)((0b11100000) | ((codepoint >> 12) & bitmask4));
      str[1] = (U8)((0b10000000) | ((codepoint >> 6 ) & bitmask6));
      str[2] = (U8)((0b10000000) |  (codepoint        & bitmask6));
      inc = 3;
    } 
    else if(codepoint < 0x1000FF)
    {
      str[0] = (U8)((0b11110000) | ((codepoint >> 18) & bitmask3));
      str[1] = (U8)((0b11000000) | ((codepoint >> 12) & bitmask6));
      str[2] = (U8)((0b10000000) | ((codepoint >> 8 ) & bitmask6));
      str[3] = (U8)((0b10000000) |  (codepoint        & bitmask6));
      inc = 4;
    }
    else 
    {
      str[0] = '?';
      inc = 1;
    }

    return inc;
}

internal UnicodeDecode
utf8_decode(U8 *str, U64 remain)
{ 
  UnicodeDecode result = {0};
  // TODO(Axel): perf wise, it may be bad for high planes => look up table ?
  if(str[0] <= 0xC0)
  {
    result.inc = 1;
    result.codepoint = str[0];
  }
  else if(str[0] < 0xE0)
  {
    result.inc = 2; 
    result.codepoint  = (str[0] & 0b00011111) << 6;
    result.codepoint |= (str[1] & 0b00111111);
  }
  else if(str[0] < 0xF0)
  {
    result.inc = 3;
    result.codepoint  = (str[0] & 0b00001111) << 12;
    result.codepoint |= (str[1] & 0b00111111) << 6;    
    result.codepoint |= (str[2] & 0b00111111);
  }
  else
  {
    result.inc = 4; 
    result.codepoint  = (str[0] & 0b00000111) << 18;
    result.codepoint |= (str[1] & 0b00111111) << 12;    
    result.codepoint |= (str[2] & 0b00111111) << 6;    
    result.codepoint |= (str[3] & 0b00111111);
  }  
  return result;   
}

internal String8
push_str8_cat(Arena *arena, String8 s1, String8 s2)
{
  String8 string;
  string.size = s1.size + s2.size;
  string.str = push_array_no_zero(arena, U8, string.size + 1);
  MemoryCopy(string.str, s1.str, s1.size);
  MemoryCopy(string.str + s1.size, s2.str, s2.size);
  string.str[string.size] = 0;
  return(string);
}


internal void
str8_list_concat_in_place(String8List *list, String8List *to_push){
  if(to_push->node_count != 0){
    if (list->last){
      list->node_count += to_push->node_count;
      list->total_size += to_push->total_size;
      list->last->next = to_push->first;
      list->last = to_push->last;
    }
    else{
      *list = *to_push;
    }
    MemoryZeroStruct(to_push);
  }
}

internal String8
str8_from_last_slash(String8 string)
{    
  if(string.size > 0){
    U8 *ptr = string.str + string.size - 1;
    for(;ptr > string.str; --ptr){
      if(*ptr == '\\' || *ptr == '/'){
        ++ptr;
        break;
      }
    }
    
    string.size = (U64)(string.str + string.size - ptr);
    string.str = ptr;
  }
  return string; 
}

internal String8
str8_chop_last_slash(String8 string)
{  
  U8 *ptr = string.str + string.size -1;
  for(;ptr > string.str; --ptr) {
    if(*ptr == '/' || *ptr == '\\') {
      ++ptr;
      break;
    }
  }
  if(ptr >= string.str) {
    string.size =  (U64)(ptr - string.str);
  }
  else {
    string.size = 0;
  }
  return string;
}

internal String8
str8_substr(String8 str, Rng1U64 range){
  range.min = ClampTop(range.min, str.size);
  range.max = ClampTop(range.max, str.size);
  str.str += range.min;
  str.size = dim_1u64(range);
  return(str);
}

internal String8
str8_suffix(String8 str, U64 size)
{
  return str8_substr(str, rng_1u64(str.size-size, str.size));
}

internal String8 
str8_postfix(String8 str, U64 size)
{
  size = ClampTop(size, str.size);
  str.str = (str.str + str.size) - size;
  str.size = size;

  return str;
}

internal B32
str8_ends_with(String8 string, String8 end, StringMatchFlags flags)
{
  String8 postfix = str8_postfix(string, end.size);
  B32 is_match = str8_match(end, postfix, flags);
  return is_match;
}

internal String16
str16_from_str8(Arena *arena, String8 utf8)
{
  U64 cap  = (utf8.size*2);
  U16* str = push_array_no_zero(arena, U16, (cap + 1));
  U64 size = 0;
  UnicodeDecode consume;
  U8 *ptr = utf8.str;
  U8 *end  = utf8.str + utf8.size;
  for(;ptr < end; ptr += consume.inc)
  {
    consume = utf8_decode(ptr, end - ptr);
    size   += utf16_encode(str + size, consume.codepoint);
  }  
  str[size] = 0;
  arena_put_back(arena, cap - size);
  String16 result = str16(str, size);

  return result;
}

internal String8
str8_skip_chop_whitespace(String8 string){
  U8 *first = string.str;
  U8 *opl = first + string.size;
  for (;first < opl; first += 1){
    if (!char_is_whitespace(*first)){
      break;
    }
  }
  for (;opl > first;){
    opl -= 1;
    if (!char_is_whitespace(*opl)){
      opl += 1;
      break;
    }
  }
  String8 result = str8_range(first, opl);
  return(result);
}

internal String8Node*
str8_list_push_node_set_string(String8List *list, String8Node *node, String8 string){
  SLLQueuePush(list->first, list->last, node);
  list->node_count += 1;
  list->total_size += string.size;
  node->string = string;
  return(node);
}

internal String8Node*
str8_list_push(Arena *arena, String8List *list, String8 string)
{ 
  String8Node *node = push_array_no_zero(arena, String8Node, 1);
  str8_list_push_node_set_string(list, node, string);
  return node;
}

internal String8Array
str8_array_from_list(Arena *arena, String8List *list)
{
  String8Array array;
  array.count = list->node_count;
  array.strings = push_array_no_zero(arena, String8, array.count);
  U64 i = 0;
  for(String8Node *n = list->first; n != 0; n = n->next, ++i)
  {
    array.strings[i] = n->string;
  }
  
  return array;
}
// Replace all join by this one
internal String8
str8_list_join(Arena *arena, String8List *list, StringJoin *optional_params){
  StringJoin join = {0};
  if (optional_params != 0){
    MemoryCopyStruct(&join, optional_params);
  }
  
  U64 sep_count = 0;
  if (list->node_count > 0){
    sep_count = list->node_count - 1;
  }
  
  String8 result;
  result.size = join.pre.size + join.post.size + sep_count*join.sep.size + list->total_size;
  U8 *ptr = result.str = push_array_no_zero(arena, U8, result.size + 1);
  
  MemoryCopy(ptr, join.pre.str, join.pre.size);
  ptr += join.pre.size;
  for (String8Node *node = list->first;
       node != 0;
       node = node->next){
    MemoryCopy(ptr, node->string.str, node->string.size);
    ptr += node->string.size;
    if (node->next != 0){
      MemoryCopy(ptr, join.sep.str, join.sep.size);
      ptr += join.sep.size;
    }
  }
  MemoryCopy(ptr, join.post.str, join.post.size);
  ptr += join.post.size;
  *ptr = 0;
  return(result);
}

internal String8
str8_range(U8 *first, U8 *one_past_last)
{
  String8 result = {first, (U64)(one_past_last - first)};
  return result;
}

internal String8List
str8_split(Arena *arena, String8 string, U8 *split_chars, U64 split_char_count, StringSplitFlags flags){
  String8List list = {0};
  
  B32 keep_empties = (flags & StringSplitFlag_KeepEmpties);
  
  U8 *ptr = string.str;
  U8 *opl = string.str + string.size;
  for (;ptr < opl;){
    U8 *first = ptr;
    for (;ptr < opl; ptr += 1){
      U8 c = *ptr;
      B32 is_split = 0;
      for (U64 i = 0; i < split_char_count; i += 1){
        if (split_chars[i] == c){
          is_split = 1;
          break;
        }
      }
      if (is_split){
        break;
      }
    }
    
    String8 str = str8_range(first, ptr);
    if (keep_empties || str.size > 0){
      str8_list_push(arena, &list, str);
    }
    ptr += 1;
  }
  
  return(list);
}

internal void
str8_split_list_push(Arena *arena, String8List *list, U8 *split_chars, U64 split_chars_count, 
                    String8 string, StringSplitFlags flags)
{
  U8 *ptr = string.str;  
  U8 *end = string.str + string.size;
  
  B32 keep_empties = (flags & StringSplitFlag_KeepEmpties);
  
  for(;ptr <= end;)
  {
    U8 *start = ptr;
    for(;ptr <= end;ptr += 1)
    {
      B32 is_split = 0;
      U8 c = *ptr;
      for(U64 i = 0; i <= split_chars_count; ++i)
      {
        // TODO, if the full string of split chars, it may be stuck in the loop        
        if(c == split_chars[i])
        {
          is_split = 1;
          break;
        }
      }
      if(is_split == 1)
      {
          break;
      }
    }
    String8 str = str8_range(start, ptr);    
    
    if(keep_empties || str.size > 0)
    {
      str8_list_push(arena, list, str);
    }    
    ++ptr;
  } 
}

internal String8List
str8_split_by_string_chars(Arena *arena, String8 string, String8 split_chars, StringSplitFlags flags){
  String8List list = str8_split(arena, string, split_chars.str, split_chars.size, flags);
  return list;
}

internal String8List
str8_list_split_by_string_chars(Arena *arena, String8List list, String8 split_chars, StringSplitFlags flags){
  String8List result = {0};
  for (String8Node *node = list.first; node != 0; node = node->next){
    String8List split = str8_split_by_string_chars(arena, node->string, split_chars, flags);
    str8_list_concat_in_place(&result, &split);
  }
  return result;
}

internal PathStyle
path_style_from_str8(String8 string){
  PathStyle result = PathStyle_Relative;
  if (string.size >= 1 && string.str[0] == '/'){
    result = PathStyle_UnixAbsolute;
  }
  else if (string.size >= 2 &&
           char_is_alpha(string.str[0]) &&
           string.str[1] == ':'){
    if (string.size == 2 ||
        char_is_slash(string.str[2])){
      result = PathStyle_WindowsAbsolute;
    }
  }
  return(result);
}

internal String8List
str8_split_path(Arena *arena, String8 string){
  String8List result = str8_split(arena, string, (U8*)"/\\", 2, 0);
  return(result);
}

internal String8
str8_cut_from_last_dot(String8 string)
{
  U8 *ptr = string.str + string.size;
  String8 result = string;
  result.str = string.str;
  result.size = string.size;
  for(;ptr > string.str; --ptr){
    if(*ptr == '.'){
      result.str = ptr + 1;
      result.size = (U64)(string.size - (result.str - string.str));
     break; 
    }
  }
  return result;
}

internal U64
cstring8_length(U8 *c){
  U8 *p = c;
  for (;*p != 0; p += 1);
  return(p - c);
}

// Conversions
internal String8
str8_from_u64(Arena *arena, U64 u64, U32 radix, U8 min_digits, U8 digit_group_separator)
{
  String8 result = {0};
  {
    // rjf: prefix
    String8 prefix = {0};
    switch(radix)
    {
      case 16:{prefix = str8_lit("0x");}break;
      case 8: {prefix = str8_lit("0o");}break;
      case 2: {prefix = str8_lit("0b");}break;
    }
    
    // rjf: determine # of chars between separators
    U8 digit_group_size = 3;
    switch(radix)
    {
      default:break;
      case 2:
      case 8:
      case 16:
      {digit_group_size = 4;}break;
    }
    
    // rjf: prep
    U64 needed_leading_0s = 0;
    {
      U64 needed_digits = 1;
      {
        U64 u64_reduce = u64;
        for(;;)
        {
          u64_reduce /= radix;
          if(u64_reduce == 0)
          {
            break;
          }
          needed_digits += 1;
        }
      }
      needed_leading_0s = (min_digits > needed_digits) ? min_digits - needed_digits : 0;
      U64 needed_separators = 0;
      if(digit_group_separator != 0)
      {
        needed_separators = (needed_digits+needed_leading_0s)/digit_group_size;
        if(needed_separators > 0 && (needed_digits+needed_leading_0s)%digit_group_size == 0)
        {
          needed_separators -= 1;
        }
      }
      result.size = prefix.size + needed_leading_0s + needed_separators + needed_digits;
      result.str = push_array_no_zero(arena, U8, result.size + 1);
      result.str[result.size] = 0;
    }
    
    // rjf: fill contents
    {
      U64 u64_reduce = u64;
      U64 digits_until_separator = digit_group_size;
      for(U64 idx = 0; idx < result.size; idx += 1)
      {
        if(digits_until_separator == 0 && digit_group_separator != 0)
        {
          result.str[result.size - idx - 1] = digit_group_separator;
          digits_until_separator = digit_group_size+1;
        }
        else
        {
          result.str[result.size - idx - 1] = char_to_lower(integer_symbols[u64_reduce%radix]);
          u64_reduce /= radix;
        }
        digits_until_separator -= 1;
        if(u64_reduce == 0)
        {
          break;
        }
      }
      for(U64 leading_0_idx = 0; leading_0_idx < needed_leading_0s; leading_0_idx += 1)
      {
        result.str[prefix.size + leading_0_idx] = '0';
      }
    }
    
    // rjf: fill prefix
    if(prefix.size != 0)
    {
      MemoryCopy(result.str, prefix.str, prefix.size);
    }
  }
  return result;
}

internal String8
str8_skip(String8 str, U64 amt){
  amt = ClampTop(amt, str.size);
  str.str += amt;
  str.size -= amt;
  return(str);
}

internal B32
str8_match(String8 a, String8 b, StringMatchFlags flags){
  B32 result = 0;
  if (a.size == b.size || (flags & StringMatchFlag_RightSideSloppy)){
    B32 case_insensitive = (flags & StringMatchFlag_CaseInsensitive);
    B32 slash_insensitive = (flags & StringMatchFlag_SlashInsensitive);
    U64 size = Min(a.size, b.size);
    result = 1;
    for (U64 i = 0; i < size; i += 1){
      U8 at = a.str[i];
      U8 bt = b.str[i];
      if (case_insensitive){
        at = char_to_upper(at);
        bt = char_to_upper(bt);
      }
      if (slash_insensitive){
        at = char_to_correct_slash(at);
        bt = char_to_correct_slash(bt);
      }
      if (at != bt){
        result = 0;
        break;
      }
    }
  }
  return(result);
}

internal U64
str8_find_needle(String8 string, U64 start_pos, String8 needle, StringMatchFlags flags){
  U8 *p = string.str + start_pos;
  U64 stop_offset = Max(string.size + 1, needle.size) - needle.size;
  U8 *stop_p = string.str + stop_offset;
  if (needle.size > 0)
  {
    U8 *string_opl = string.str + string.size;
    String8 needle_tail = str8_skip(needle, 1);
    StringMatchFlags adjusted_flags = flags | StringMatchFlag_RightSideSloppy;
    U8 needle_first_char_adjusted = needle.str[0];
    if(adjusted_flags & StringMatchFlag_CaseInsensitive)
    {
      needle_first_char_adjusted = char_to_upper(needle_first_char_adjusted);
    }
    for (;p < stop_p; p += 1)
    {
      U8 haystack_char_adjusted = *p;
      if(adjusted_flags & StringMatchFlag_CaseInsensitive)
      {
        haystack_char_adjusted = char_to_upper(haystack_char_adjusted);
      }
      if (haystack_char_adjusted == needle_first_char_adjusted)
      {
        if (str8_match(str8_range(p + 1, string_opl), needle_tail, adjusted_flags))
        {
          break;
        }
      }
    }
  }
  U64 result = string.size;
  if (p < stop_p){
    result = (U64)(p - string.str);
  }
  return(result);
}


internal U64
u64_from_str8(String8 string, U32 radix)
{
 AssertAlways(2 <= radix && radix <= 16);
 local_persist U8 char_to_value[] =
 {
  0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
  0x08,0x09,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
 };
 U64 value = 0;
 for (U64 i = 0; i < string.size; i += 1)
 {
  value *= radix;
  U8 c = string.str[i];
  value += char_to_value[(c - 0x30)&0x1F];
 }
 return value;
}


internal String8
cstyle_hex_from_u64(Arena *arena, U64 x, B32 caps)
{
 local_persist char int_value_to_char[] = "0123456789abcdef";
 U8 buffer[18];
 U8 *opl = buffer + 18;
 U8 *ptr = opl;
 if(x == 0)
 {
  ptr -= 1;
  *ptr = '0';
 }
 else
 {
  for(;;)
  {
   U32 val = x%16;
   x /= 16;
   U8 c = (U8)int_value_to_char[val];
   if(caps)
   {
    c = char_to_upper(c);
   }
   ptr -= 1;
   *ptr = c;
   if (x == 0)
   {
    break;
   }
  }
 }
 ptr -= 1;
 *ptr = 'x';
 ptr -= 1;
 *ptr = '0';
 
 String8 result = {0};
 result.size = (U64)(opl - ptr);
 result.str = push_array_no_zero(arena, U8, result.size);
 MemoryCopy(result.str, buffer, result.size);
 
 return result;
}

internal S64
sign_from_str8(String8 string, String8 *string_tail){
  // count negative signs
  U64 neg_count = 0;
  U64 i = 0;
  for (; i < string.size; i += 1){
    if (string.str[i] == '-'){
      neg_count += 1;
    }
    else if (string.str[i] != '+'){
      break;
    }
  }
  
  // output part of string after signs
  *string_tail = str8_skip(string, i);
  
  // output integer sign
  S64 sign = (neg_count & 1)?-1:+1;
  return(sign);
}

internal S64
s64_from_str8(String8 string, U32 radix){
  S64 sign = sign_from_str8(string, &string);
  S64 x = (S64)u64_from_str8(string, radix) * sign;
  return(x);
}

internal F64
f64_from_str8(String8 string)
{
  // TODO(rjf): crappy implementation for now that just uses atof.
  F64 result = 0;
  if(string.size > 0)
  {
    // rjf: find starting pos of numeric string, as well as sign
    F64 sign = +1.0;
    //U64 first_numeric = 0;
    if(string.str[0] == '-')
    {
      //first_numeric = 1;
      sign = -1.0;
    }
    else if(string.str[0] == '+')
    {
      //first_numeric = 1;
      sign = 1.0;
    }
    
    // rjf: gather numerics
    U64 num_valid_chars = 0;
    char buffer[64];
    for(U64 idx = 0; idx < string.size && num_valid_chars < sizeof(buffer)-1; idx += 1)
    {
      if(char_is_digit(string.str[idx], 10) || string.str[idx] == '.')
      {
        buffer[num_valid_chars] = string.str[idx];
        num_valid_chars += 1;
      }
    }
    
    // rjf: null-terminate (the reason for all of this!!!!!!)
    buffer[num_valid_chars] = 0;
    
    // rjf: do final conversion
    result = sign * atof(buffer);
  }
  return result;
}
