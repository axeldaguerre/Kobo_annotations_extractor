internal HTMLElementNode*
html_element_list_push(Arena *arena, HTMLElementList *list, HTMLElement element)
{
  // TODO: it will fail, as we never save anything, without talking about children nodes ...
  HTMLElementNode *node = push_array_no_zero(arena, HTMLElementNode, 1);
  SLLQueuePush_NZ(&html_el_n_g_nil, list->first, list->last, node, next);
  node->element = element;
  ++list->node_count;
  
  return node;
}

internal HTMLTag *
html_tag_from_inv(Arena *arena, HTMLTagInvariant *inv)
{
  HTMLTag *result = push_array(arena, HTMLTag, 1);
  if(inv)
  {
    result->tag = inv->tag;
    result->tag_type = inv->tag_type;
    result->enclosing_type = inv->enclosing_type;
    result->tag_name = inv->tag_name;
    result->meaning = inv->meaning;
  }
    
  return result;
}

internal HTMLParser *
html_init_parser(Arena *arena, String8 str)
{
  HTMLParser *result = push_array(arena, HTMLParser, 1);
  if(str.size)
  {
    result->string = push_str8_copy(arena, str);
    result->string = str;
  }
  result->error.messages = push_array(arena, String8List, 1);
  
  return result;
}

internal String8 
html_print_error_type(HTMLErrorType type)
{
  String8 result = {0};
  switch(type)
  {
    case HTMLErrorType_unexpected_token:
    {
      result = str8_lit("unexpected token"); 
    } break;
    
    case HTMLErrorType_wrong_enclosing_type:
    {
      result = str8_lit("wrong enclosing type"); 
    } break;
    
    default:
    {
      result = str8_lit("Unknown error type"); 
    } break;
  }
  
  return result;
}

internal void
html_parser_set_error(Arena *arena, HTMLParser *parser, U64 at, 
                      HTMLErrorType type, String8 message)
{
  /*
    TODO: Global arena is needed, we can loose the message later. As currently
          we're using only one arena pointer we're fine though
  */ 
  String8 output = {0};
  output = push_str8_cat(arena, output, str8_lit(" at byte: "));
  output = push_str8_cat(arena, output, str8_from_u64(arena, at, 10, 2, 0));
  output = push_str8_cat(arena, output, str8_lit(" error type: "));
  output = push_str8_cat(arena, output, html_print_error_type(type));
  
  parser->error.at = at;
  parser->error.type = type;
  str8_list_push(arena, parser->error.messages, output);
}

internal B32 
html_tag_is_opening(HTMLTag *tag)
{
  B32 result = tag && tag->first_token.type == RawTokenType_angle_bracket_open;
             
  return result;
}

internal void
html_analyse_element(HTMLParser *parser, HTMLElementNode *el)
{
  HTMLErrorType errors = HTMLErrorType_Null;  
  // if (!html_tag_is_opening(el->tags[0]))
  // {
  //   parser->error.type |= HTMLErrorType_wrong_enclosing_type;
  // }
  // 
  // if(!el->tags[1] && el->tags[0]->enclosing_type == HTMLTagEnclosingType_Paired)
  // {
  //   errors |= HTMLErrorType_wrong_enclosing_type;
  // }
  // 
  // else if(el->tags[1] && el->tags[0]->enclosing_type == HTMLTagEnclosingType_Paired && 
  //    el->tags[1]->tag != el->tags[0]->tag)
  // {
  //   errors |= HTMLErrorType_wrong_enclosing_type;
  // }
  // 
  parser->error.type |= errors;
  // parser->error.at = el->tags[0]->first_token.range.min;
}

internal B32
html_is_in_bounds(HTMLParser *parser)
{
  return (parser->at < parser->string.size);
}

internal B32
html_is_parsing(HTMLParser *parser)
{ 
  while(char_is_whitespace(parser->string.str[parser->at]))
  {
    ++parser->at;
  }
  B32 result = parser->error.type == HTMLErrorType_Null && html_is_in_bounds(parser);
  return result;
}

internal HTMLToken
html_get_token_type(String8 string, U64 at)
{
  HTMLToken result = {0};
  result.range.min = at;
  result.range.max = at+1;
  
  U8 value = string.str[at];
  
  switch(value)
  {
    case '<': 
    {
      if(string.str[at+1] == '/')
      {
        ++result.range.max; 
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
        ++result.range.max; 
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
  }
  
  return result;  
}

/*
  NOTE: We use only the first chunk of character to determine the tag, it permits to skip the 
        meaningfull tokens inside element's without complexify the computation. 
        Whitespace is computed, by consequence will fail.
*/
internal HTMLTagInvariant *
html_get_invariant_from_tag_name(Arena *arena, String8 tag_name)
{  
    HTMLTagInvariant *result = {0};
    String8 split = str8_lit(" \t\r\n");
    String8List find_tag = str8_split_by_string_chars(arena, tag_name, split, StringSplitFlag_KeepEmpties);
    
    HTMLTagInvariantTable table = html_get_tag_table();
    if(find_tag.total_size)
    {
      String8 string = find_tag.first->string;    
      B32 is_match = 0;
      for(U8 tag_inv_idx = 0; 
          tag_inv_idx < table.tag_count;
          ++tag_inv_idx)
      {
        HTMLTagInvariant *tag_inv = &table.tags[tag_inv_idx];
        if(str8_match(string, tag_inv->tag_name, 0))
        {
          is_match = 1;
          result = tag_inv;
          break;
        }
        if(is_match) break;
      }
    }
    
    return result;
}

/*
  NOTE: In html, you will need to parse *any* meaningfull token for parsing before knowing if it's a dummy char or not
        Like having '<' could be dummy OR it could be the start of the html tag. Spec lack efficiency in that regard.
        We currently avoid the complexity and time wasted by asking to escape all meaningfull tokens in the element's content.
*/
internal HTMLToken
html_next_token(HTMLParser *parser)
{
  HTMLToken result = {0};    
  RawTokenType  match = RawTokenType_angle_bracket_open            |
                        RawTokenType_angle_bracket_close           |
                        RawTokenType_angle_bracket_open_then_slash |
                        RawTokenType_angle_slash_then_bracket_close;
  
  U64 at = parser->at;
   
  while(html_is_parsing(parser) && !(result.type & match))
  {
    result = html_get_token_type(parser->string, at);
    at += result.range.max-result.range.min;
  }
  parser->at = at;
  
  return result;
}

internal HTMLTag *
html_next_tag(Arena *arena, HTMLParser *parser)
{
  HTMLTag *tag = push_array_no_zero(arena, HTMLTag, 1);
  
  for(;;)
  {
    tag->first_token = html_next_token(parser);
    tag->last_token = html_next_token(parser);
  
    U64 str_end = ClampTop(parser->string.size, tag->last_token.range.min);
    U64 str_start = ClampTop(str_end, tag->first_token.range.max);
    String8 tag_name = str8((parser->string.str + str_start), str_end-str_start);
    tag = html_tag_from_inv(arena, html_get_invariant_from_tag_name(arena, tag_name));
    
    if(tag && parser->skip_until_tag == HTMLTag_NULL) break;
    // NOTE: pre & code can have html meaningfull tokens, but are dummy
    else if(tag->tag == parser->skip_until_tag) break;
    // NOTE: first tag was dummy, but last tag could have been part of a meaningfull one
    parser->at = tag->first_token.range.max;
  }
  
  if((tag->tag & HTMLTag_PRE) || (tag->tag & HTMLTag_CODE))
  {
    if(html_tag_is_opening(tag))
    {
      parser->skip_until_tag = tag->tag;
    }
    else
    {
      parser->skip_until_tag &= ~tag->tag;
    }
  }
  
  return tag;
}

internal HTMLElementNode *
html_parse_element(Arena *arena, HTMLParser *parser, HTMLTag *from_tag)
{
  HTMLElementNode *root_n = push_array(arena, HTMLElementNode, 1);  
  root_n->element.level_deep = ++parser->level_deep;

  if(from_tag)
  {
    root_n->element.tags[0] = from_tag;
  }
  else
  {
    root_n->element.tags[0] = html_next_tag(arena, parser);
  }

  if(root_n->element.tags[0]->enclosing_type == HTMLTagEnclosingType_Paired)
  {
    HTMLTag *tag   = {0};
    for(;;)
    {
      tag = html_next_tag(arena, parser);

      if(!html_is_parsing(parser) || 
         (!html_tag_is_opening(tag) && tag->tag == root_n->element.tags[0]->tag))
      {
        root_n->element.tags[1] = tag;
        break;
      }
      
      if(tag->tag != root_n->element.tags[0]->tag && 
         !html_tag_is_opening(tag)) 
      {
        html_parser_set_error(arena, parser, tag->first_token.range.min,
                              HTMLErrorType_unexpected_token, 
                              str8_lit("From opening tag, finding a closing tag from different type is not allowed"));
        break;
      }
      HTMLElementNode *el_node = html_parse_element(arena, parser, tag);
      DLLPushBack_NPZ(&html_el_n_g_nil, 
                      root_n->first, root_n->last, 
                      el_node, next, prev);
    }
    
    root_n->element.raw.data.str = parser->string.str + root_n->element.tags[0]->last_token.range.max;
    root_n->element.raw.data.size = root_n->element.tags[1]->first_token.range.min - root_n->element.tags[0]->last_token.range.max;
    root_n->element.raw.meaning = root_n->element.tags[1]->meaning;
  }
  
  html_analyse_element(parser, root_n);
  --parser->level_deep;
  
  return root_n;
}

internal String8
html_get_error_msg(Arena *arena, HTMLParser *parser, String8 file_path)
{
  // TODO: add the warning to escape char like '<' == '&lt;' '>' = '&gt;' 
  /*
    TODO: we should try to keep parsing until a certain point of level failure
          that way you can provide a more complete error log
  */ 
  String8List list = {0};
  str8_list_push(arena, &list, str8_lit("file path:"));
  str8_list_push(arena, &list, file_path);
  
  for(String8Node *n = parser->error.messages->first;
      n != 0;
      n = n->next)
  {
    str8_list_push(arena, &list, n->string); 
  }
  
  StringJoin join = {0};
  join.post = str8_lit("\n");
  String8 result = str8_list_join(arena, &list, &join);
  
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

    String8 file_name = push_str8_cat(arena, node->info.root_path, node->info.name);
    
    OS_Handle handle = os_file_open(arena, file_name, OS_AccessFlag_Read);
    if(os_handle_match(handle, os_handle_zero())) continue;
    
    U64 size_file = node->info.props.size;
    U8 *memory = push_array_no_zero(arena, U8, size_file);
    U64 size = os_file_read(handle, rng_1u64(0, size_file), memory);
    HTMLParser *parser = html_init_parser(arena, str8(memory, size));
    
    StringJoin join = {0};
    join.post = str8_lit("\n");
    HTMLElementNode *root_n = push_array(arena, HTMLElementNode, 1);
    while(html_is_parsing(parser))
    {
      HTMLElementNode *el_n = html_parse_element(arena, parser, 0);
      DLLPushBack_NPZ(&html_el_n_g_nil, root_n->first, root_n->last, el_n, next, prev);
      
      if(parser->error.type != HTMLErrorType_Null)
      {
        String8 error_msg = html_get_error_msg(arena, parser, file_name);
        str8_list_push(arena, error_messages, error_msg);
      } 
    }
    result = str8_list_join(arena, error_messages, &join);
  }
  return result;     
}

internal HTMLElementNode *
html_create_element_from_tag_type(Arena *arena, U64 type)
{
  HTMLElementNode *result = push_array(arena, HTMLElementNode, 1);  
  HTMLTagInvariantTable table = html_get_tag_table();
  for(U32 idx = 0; idx < table.tag_count; ++idx)
  {
    HTMLTagInvariant *invariant = &table.tags[idx];
    if(type == invariant->tag)
    {
      //TODO: Do the real computation (search this line to see other)
      result->element.tags[0] = html_tag_from_inv(arena, invariant);
      result->element.tags[1] = html_tag_from_inv(arena, invariant);
      break;
    }
  }
  
  return result;
}

internal HTMLElementNode *
html_get_root_doc(Arena *arena, String8 title)
{
  HTMLElementNode *doctype = html_create_element_from_tag_type(arena, HTMLTag_DOCTYPE);
  HTMLElementNode *html    = html_create_element_from_tag_type(arena, HTMLTag_HTML);
  HTMLElementNode *head    = html_create_element_from_tag_type(arena, HTMLTag_HEAD);
  HTMLElementNode *body    = html_create_element_from_tag_type(arena, HTMLTag_BODY);
  HTMLElementNode *div     = html_create_element_from_tag_type(arena, HTMLTag_DIV);
  
  DLLPushBack_NPZ(&html_el_n_g_nil, body->first, body->last, div, next, prev);
  DLLPushBack_NPZ(&html_el_n_g_nil, head->first, head->last, body, next, prev);
  DLLPushBack_NPZ(&html_el_n_g_nil, html->first, html->last, head, next, prev);
  DLLPushBack_NPZ(&html_el_n_g_nil, doctype->first, doctype->last, html, next, prev);
  
  if(title.size)
  {
    HTMLElementNode *h1 = html_create_element_from_tag_type(arena, HTMLTag_H1);
    
    String8 h1_content = push_str8_copy(arena, title);
    DLLPushBack_NPZ(&html_el_n_g_nil, div->first, div->last, h1, next, prev);
  }
  
  return doctype;
}
  
internal String8
html_str8_from_element(Arena *arena, HTMLElementNode *root, U8 indent)
{
  String8 result = {0};
  if(!root) return result;
  
  String8List *list_result = push_array(arena, String8List, 1);
  
  U8 indentation = root->element.level_deep * indent;
  String8 tab_space = {0};
  while(indentation)
  {
    tab_space = push_str8_cat(arena, tab_space, str8_lit(" "));
    --indentation;
  }
  str8_list_push(arena, list_result, tab_space);
  
  HTMLElementNode *n = root;
  for(;
      n != 0 && n != &html_el_n_g_nil;
      n = n->next)
  {
    String8 str = push_str8f(arena, "%S", tab_space);
    str = push_str8_cat(arena, str, push_str8f(arena, "<%S", n->element.tags[0]->tag_name));
    switch(n->element.tags[0]->enclosing_type)
    {
      case HTMLTagEnclosingType_Paired:
      {
        str = push_str8_cat(arena, str, push_str8f(arena, ">%S", n->element.raw.data));
        String8 closing_tag = push_str8f(arena, "</%S>", n->element.tags[0]->tag_name);
        // NOTE: Recursion for children nodes 
        String8 children_str8 = html_str8_from_element(arena, n->first, indent);
        if(children_str8.size == 0 ||
           n->first->element.tags[0]->tag_type == TagType_Block)
        {
          str = push_str8_cat(arena, str, push_str8f(arena, "%S%S", closing_tag, children_str8));
        }
        else if(n->first->element.tags[0]->tag_type == TagType_Inline)
        {
          str = push_str8_cat(arena, str, push_str8f(arena, "%S%S", children_str8, closing_tag));
        }
        
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
    
  StringJoin join = {0};
  join.sep = str8_lit("\n");
  result = str8_list_join(arena, list_result, &join); 
  
  return result;
}

internal HTMLTagInvariant *
html_find_first_tag_from_meaning(Arena *arena, RawMeaning meaning)
{
  HTMLTagInvariant *result = push_array(arena, HTMLTagInvariant, 1);
  HTMLTagInvariantTable table = html_get_tag_table();  
  for(U8 idx = 0; idx < table.tag_count; ++idx)
  {
    HTMLTagInvariant *invariant = table.tags + idx;
    if(invariant->meaning.strenght == meaning.strenght && 
       invariant->meaning.semantic_flags & meaning.semantic_flags)
    {
      result = invariant;
      break;
    }
  }
  
  return result;
}

internal HTMLElementNode *
html_element_from_raw_node(Arena *arena, RawDataNode *raw_n)
{
  // TODO: previous is not set for annotation's details
    HTMLElementNode *root_n = push_array(arena, HTMLElementNode, 1);
    HTMLTag *tag = html_tag_from_inv(arena, html_find_first_tag_from_meaning(arena, raw_n->raw.meaning));
    root_n->element.tags[0] = root_n->element.tags[1] = tag;
    root_n->element.raw.meaning = raw_n->raw.meaning;
    root_n->element.raw.data = push_str8_copy(arena, raw_n->raw.data);
    
    for(RawDataNode *n = raw_n->first;
        n != &raw_node_g_nil && n != 0;
        n = n->next)
    {
      HTMLElementNode *element_n = html_element_from_raw_node(arena, n);
      DLLPushBack_NPZ(&html_el_n_g_nil, root_n->first, root_n->last, element_n, next, prev);
    }
    
    return root_n;
}
