internal HTMLElementNode*
html_element_list_push(Arena *arena, HTMLElementList *list, HTMLElement element)
{
  HTMLElementNode *node = push_array_no_zero(arena, HTMLElementNode, 1);
  SLLPush(list->first, list->last, node);
  node->element = element;
  node->element.content = push_str8_copy(arena, element.content);
  ++list->node_count;
  return node;
}

internal HTMLElementArray
html_element_array_from_list(Arena *arena, HTMLElementList *list)
{
  HTMLElementArray array = {0};
  array.count = list->node_count;
  array.v = push_array_no_zero(arena, HTMLElement, array.count);
  U64 idx = 0;
  for(HTMLElementNode *n = list->first; n != 0; n = n->next)
  {
    U64 size = sizeof(HTMLElement) + n->element.content.size;
    MemoryCopy(array.v+idx, &n->element, size);
    ++idx;
  }
  return array;
}

internal B32
html_is_in_bounds(HTMLParser *parser, U64 at)
{
  return (at < parser->string.size);
}

internal B32
html_is_parsing(HTMLParser *parser)
{
  B32 result = parser->error.type == HTMLErrorType_Null && html_is_in_bounds(parser, parser->at);
  return result;
}

internal HTMLToken
html_get_token(String8 string, U64 at)
{
  HTMLToken result = {0};
  result.type = RawTokenType_null;
  result.string.str = string.str + at;
  result.string.size = 1;
  U8 value = string.str[at];
  switch(value)
  {
    case '<': 
    {
      if(string.str[at+1] == '/')
      {
        result.string.size = 2;
        result.type = RawTokenType_angle_bracket_open_then_slash;
      }
      else 
      {
        result.type = RawTokenType_angle_bracket_open;
      }
    } break;
    case '/': 
    {
      if(string.str[at+1] == '>')
      {
        result.string.size = 2;
        result.type = RawTokenType_angle_slash_then_bracket_close;
      }
      else 
      {
        result.type = RawTokenType_slash;
      }
      
    } break;
    case '>': 
    { 
      result.type = RawTokenType_angle_bracket_close;
    } break;      
    case ' ': 
    { 
      result.type = RawTokenType_whitespace;
    } break;
    default :
    {
      result.type = RawTokenType_dummy;
    } break;
  }  
  return result;
}
typedef struct Raw Raw;
struct Raw
{
  String8        str;
  FileFormatType file_type;
};

internal HTMLTag 
html_create_tag(Arena *arena, HTMLTagInvariant *inv)
{
  HTMLTag result = {0};
  result.type = inv->type;
  result.enclosing_type = inv->enclosing_type;
  result.tag_name = inv->tag_name;
  result.text_types = inv->text_types;
  return result;
}

internal HTMLTag
html_try_create_get_tag(Arena *arena, Raw raw)
{  
    HTMLTag result = {0};
    AssertAlways(raw.file_type == FileFormatType_HTML);
    String8 split = str8_lit(" </>");
    String8List find_tag = str8_split_by_string_chars(arena, raw.str, split, StringSplitFlag_None);
    
    HTMLTagTable table = html_get_tag_table();
    for(U8 tag_inv_idx = 0; 
      tag_inv_idx < table.tag_count;
      ++tag_inv_idx)
    { 
      HTMLTagInvariant *tag_inv = &table.tags[tag_inv_idx];
      for(String8Node *n = find_tag.first; n != 0; n = n->next)
      {     
        if(str8_match(n->string, tag_inv->tag_name, 0))
        {
          result = html_create_tag(arena, tag_inv);
          break;
        }
      }
    }
    return result;
}

internal HTMLTag
html_get_next_tag(Arena *arena, HTMLParser *parser)
{
  HTMLToken token             = {0};
  HTMLTag result              = {0};
  Rng1U64 range               = {0};
  U64 at                      = parser->at;
  
  while(html_is_in_bounds(parser, at) && !(token.type & FIRST_TOKEN_TAG_FLAGS))
  {
     token = html_get_token(parser->string, at);
     at   += token.string.size;
  }
  if(token.type == RawTokenType_null) return result;
  range.min = at;
  
  while(html_is_in_bounds(parser, at) && !(token.type & LAST_TOKEN_TAG_FLAGS))
  {
     token = html_get_token(parser->string, at);
     at   += token.string.size;
  } 
  if(token.type == RawTokenType_null) return result;
  
  U64 size = at - range.min;
  range.max = range.min + size + 1;
  String8 str = str8(parser->string.str + range.min, size);
  Raw raw = {0};
  raw.file_type = FileFormatType_HTML;
  raw.str = str;
  
  HTMLTag tag = html_try_create_get_tag(arena, raw);
  
  if(tag.type != RawTokenType_null)
  {
    result = tag;
    if(result.range[0].max == 0)
    {
      result.range[0] = range;
    }
    else
    {
      result.range[1] = range;
    }
    
    parser->at = at;
  }
  else
  {
    // NOTE: End of file
    parser->at = parser->string.size;
  }
  return result;
}

internal HTMLElement *
html_parse_element_paired(Arena *arena, HTMLParser *parser, HTMLTag *end);
internal HTMLElement *
html_parse_element(Arena *arena, HTMLParser *parser)
{
  HTMLElement *result = push_array_no_zero(arena, HTMLElement, 1);
  result->content = str8(parser->string.str+parser->at, parser->string.size);
  HTMLTag opening_tag = html_get_next_tag(arena, parser);
  if(parser->at >= parser->string.size) return result;
  switch(opening_tag.enclosing_type)
  {
    case HTMLTagEnclosingType_Paired:
    {
      // TODO(URGENT): Adding the firstSubEl will break the program using it like web_checker_main
      result->first_sub_element = html_parse_element_paired(arena, parser, &opening_tag);
      html_get_next_tag(arena, parser); // NOTE: closing tag
    } break;
    case HTMLTagEnclosingType_Unique:
    {
    } break;
    case HTMLTagEnclosingType_Self:
    {
    } break;    
  }
  result->tag = opening_tag;
  
  return result;
}

internal HTMLElement *
html_parse_element_paired(Arena *arena, HTMLParser *parser, HTMLTag *tag)
{
  HTMLElement *last_child = {0};
  HTMLElement *first_child = {0};
  for(;html_is_parsing(parser);) 
  {
    HTMLTag next_tag = html_get_next_tag(arena, parser);
    if(next_tag.type == tag->type)
    {
      break;
    }
    HTMLElement *element = html_parse_element(arena, parser);
    if(element)
    { 
      // TODO: MACRO or procedure (search to see others)     
      if(last_child)
      {
        last_child->next_sibbling = element;
      }
      else
      {
        first_child = element;
      }
      last_child = element;  
    }
  }
  return first_child;
}

internal String8
html_get_error_msg(Arena *arena, HTMLParser *parser, String8 file_path)
{
  String8List list = {0};
  str8_list_push(arena, &list, str8_lit("file path:"));
  str8_list_push(arena, &list, file_path);
  str8_list_push(arena, &list, str8_lit("Unexpected token:"));
  str8_list_push(arena, &list, str8_lit("at byte:"));
  str8_list_push(arena, &list, str8_from_u64(arena, parser->error.at, 10, 7, 0));
  str8_list_push(arena, &list, str8_lit("Error type:"));
  
  HTMLError error = parser->error;
  
  if(error.type & HTMLErrorType_unexpected_token)
  {
    str8_list_push(arena, &list, str8_lit("Unexpected token:\n"));
  }
  if(error.type & HTMLErrorType_wrong_enclosing_type)
  {
    str8_list_push(arena, &list, str8_lit("Wrong enclosing type:\n"));
  }  
  String8 result = str8_list_join_TO_DELETE(arena, &list, str8_lit("\n"));
  return result;
}

internal String8
html_parse(Arena *arena, OS_FileInfoList *info_list)
{
  String8 result = {0};
  String8List *error_messages = push_array(arena, String8List, 1);
  for(OS_FileInfoNode *node = info_list->first;
      node != 0;
      node = node->next) 
  {
    StringMatchFlags match_flags = StringMatchFlag_CaseInsensitive;
    if(!str8_match(node->info.props.extension, str8_lit("html"), match_flags)) continue;

    U64 size_file = node->info.props.size;
    String8 file_name = push_str8_cat(arena, node->info.root_path, node->info.name);
    
    OS_Handle handle = os_file_open(arena, file_name, OS_AccessFlag_Read);
    if(os_handle_match(handle, os_handle_zero())) continue;

    U8 *memory = push_array_no_zero(arena, U8, size_file);
    U64 size = os_file_read(handle, rng_1u64(0, size_file), memory);
    HTMLParser parser = {0};
    parser.string.str = memory;
    parser.string.size = size;
    
    HTMLElementList *el_list = push_array(arena, HTMLElementList, 1);
    HTMLElement *first_el = {0};
    HTMLElement *last_el  = {0};
    while(html_is_parsing(&parser))
    {
      HTMLElement *el = html_parse_element(arena, &parser);
      if(!el) break;
      if(!first_el) first_el = el;
      if(last_el) last_el->next_sibbling = el;
      last_el = el;
      
      if(parser.error.type != HTMLErrorType_Null)
      {
        String8 error_msg = html_get_error_msg(arena, &parser, file_name);
        str8_list_push(arena, error_messages, error_msg);
      } 
    }
    if(first_el)
    {
      html_element_list_push(arena, el_list, *first_el);
    }
    result = str8_list_join_TO_DELETE(arena, error_messages, str8_lit("\n"));
  } 
  return result;     
}

internal HTMLTag
html_create_tag_from_text_types(Arena *arena, TextType types)
{
  HTMLTag result = {0};
  HTMLTagTable table = html_get_tag_table();
  for(U32 idx = 0; idx < table.tag_count; ++idx)
  {
    HTMLTagInvariant *invariant = &table.tags[idx];
    if(types & invariant->text_types)
    {
      result = html_create_tag(arena, invariant);
      break;
    }
  }
  return result;
}

internal HTMLElement * 
html_create_element_from_textual(Arena *arena, Textual textual)
{
  HTMLElement *result = push_array(arena, HTMLElement, 1);
  result->content = textual.text;
  result->tag = html_create_tag_from_text_types(arena, textual.type);
  return result;
}

internal HTMLElement *
html_create_element_from_tag_type(Arena *arena, HTMLTagType type)
{
  HTMLElement *result = push_array(arena, HTMLElement, 1);  
  HTMLTagTable table = html_get_tag_table();
  for(U32 idx = 0; idx < table.tag_count; ++idx)
  {
    HTMLTagInvariant *invariant = &table.tags[idx];
    if(type == invariant->type)
    {
      result->tag = html_create_tag(arena, invariant);
      break;
    }
  }
  return result;
}

internal void
html_append_into(Arena *arena, HTMLElement *first_el, HTMLElement *root)
{
  for(HTMLElement *last_el = root; 
      last_el !=0; 
      last_el = last_el->first_sub_element)
  {
    if(!last_el->first_sub_element)
    {
      last_el->first_sub_element = first_el;
      break;
    }
  }
}

internal HTMLElement *
html_get_root_doc(Arena *arena)
{
  HTMLElement *first_el = push_array(arena, HTMLElement, 1);
  HTMLElement *last_el = {0};
  HTMLTagType types[] = { HTMLTag_html, HTMLTag_head, HTMLTag_body, HTMLTag_div };
  for(U8 tag_idx = 0; tag_idx < ArrayCount(types); ++tag_idx)
  {    
    HTMLTagType tag_type = types[tag_idx];
    HTMLElement *element = html_create_element_from_tag_type(arena, tag_type);
    // TODO: MACRO or procedure (search to see others)
    if(last_el)
    {
      last_el->first_sub_element = element;
    }
    else
    {
      first_el = element;
    }
    last_el = element;
  }
  return first_el;
}
/*
    NOTE: HTMLElement and Textual differ when it comes to sibbling and first_sub
          there is work to do in terms of the implementation of the data types. Textual 
          sub permits to keep tracks of the related parent, but in HTMLElement an element 
          have relation not currently saved like a h2 is a child of the h1. Maybe we could 
          make a table or use the inl implementation in order to explicitely have this relation.
          The reason it's not clear how the implementation can be good, is that textual and html document differs
          in terms of conceptual meaning. In high level, html permits meaning representation of text and more 
          compared to textual that is only relation between text 
          So we unroll the first_sub_textual AND unroll the next_sibbling textual
          In short: textual / html element's first sub element differ. htmlelement have +1 level relatioship 
  */
internal HTMLElement *
html_element_from_textual(Arena *arena, Textual *textual)
{
  HTMLElement *result = {0};
  
  if(textual)
  {
    // NOTE: Unrolling sub and next sibbling from textual    
    HTMLElement *first = {0};
    HTMLElement *last = {0};
    for(Textual *sibbling_textual = textual;
        sibbling_textual != 0;
        sibbling_textual = sibbling_textual->next_sibbling)
    {
      HTMLElement *root = html_create_element_from_textual(arena, *sibbling_textual);
      
      HTMLElement *first_sub = {0};
      HTMLElement *last_sub = {0};
      for(Textual *sub_textual = sibbling_textual->first_sub_textual;
          sub_textual != 0;
          sub_textual = sub_textual->first_sub_textual)
      {        
        HTMLElement *el = html_create_element_from_textual(arena, *sub_textual);
        if(last_sub)
        {
          last_sub->next_sibbling = el; 
        }
        else
        {
          first_sub = el;
        }
        last_sub = el;
      }
      
      root->next_sibbling = first_sub;
      if(last)
      {
        last->next_sibbling = root; 
      }
      else
      {
        first = root;
      }
      last = last_sub;
    }
    result = first;
  }
  
  return result;  
}
  
internal String8
html_str8_from_element(Arena *arena, HTMLElement *el, HTMLParser *parser)
{
  String8 result = {0};
  String8List *list_result = push_array(arena, String8List, 1);
  
  U8 indentation = parser->level_deep++ * parser->space_by_indent;
  String8 tab_space = {0};
  while(indentation)
  {
    tab_space = push_str8_cat(arena, tab_space, str8_lit(" "));
    --indentation;
  }
  str8_list_push(arena, list_result, tab_space);
  for(;
      el != 0;
      el = el->next_sibbling)
  {
    if(el)
    {
      String8 str = tab_space;
      str = push_str8_cat(arena, str, str8_lit("<"));
      str = push_str8_cat(arena, str, el->tag.tag_name);
      
      switch(el->tag.enclosing_type)
      {
        case HTMLTagEnclosingType_Paired:
        {
          str = push_str8_cat(arena, str, str8_lit(">"));
          if(el->first_sub_element) 
          {
            str = push_str8_cat(arena, str, html_str8_from_element(arena, el->first_sub_element, parser));
          }
          str = push_str8_cat(arena, str, el->content);
          str = push_str8_cat(arena, str, str8_lit("</"));
          str = push_str8_cat(arena, str, el->tag.tag_name);
          str = push_str8_cat(arena, str, str8_lit(">"));
        } break;
        case HTMLTagEnclosingType_Unique:
        {
          str = push_str8_cat(arena, str ,str8_lit(">"));
        } break;        
        case HTMLTagEnclosingType_Self:
        {
          str = push_str8_cat(arena, str ,str8_lit("/>"));
        } break;
        
      }  
      str = push_str8_cat(arena, str, str8_lit("\n"));
      str8_list_push(arena, list_result, str);
    }
  }
    
  StringJoin join = {0};
  join.sep = str8_lit("\n");
  result = str8_list_join(arena, list_result, &join); 
  
  return result;
}

internal HTMLElement *
html_try_get_first_element_from_tag_type(HTMLElement *element, HTMLTagType type)
{
  HTMLElement *result = {0};
  for(HTMLElement *el = element; el != 0; el = el->next_sibbling)
  {
    if(el->tag.type == type)
    {
      result = el;
      break;
    }
  } 
  return result;
}