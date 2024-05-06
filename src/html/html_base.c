internal HTMLElementNode*
html_element_list_push(Arena *arena, HTMLElementList *list, HTMLElement element)
{
  HTMLElementNode *node = push_array_no_zero(arena, HTMLElementNode, 1);
  SLLPush(list->first, list->last, node);
  node->element = element;
  ++list->node_count;
  
  return node;
}

internal HTMLTag *
html_create_tag(Arena *arena, HTMLTagInvariant *inv)
{
  HTMLTag *result = push_array(arena, HTMLTag, 1);
  result->type = inv->type;
  result->enclosing_type = inv->enclosing_type;
  result->tag_name = inv->tag_name;
  result->text_types = inv->text_types;
  
  return result;
}

internal HTMLTag  *
html_create_tag_from_tag_type(Arena *arena, U64 type)
{
  HTMLTag *result = push_array(arena, HTMLTag, 1);
  HTMLTagTable table = html_get_tag_table();
  for(U32 idx = 0; idx < table.tag_count; ++idx)
  {
    HTMLTagInvariant *invariant = &table.tags[idx];
    if(type == invariant->type)
    {
      result = html_create_tag(arena, invariant);
      break;
    }
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
html_analyse_element(HTMLParser *parser, HTMLElement *el)
{
  HTMLErrorType errors = HTMLErrorType_Null;  
  if (!html_tag_is_opening(el->tags[0]))
  {
    parser->error.type |= HTMLErrorType_wrong_enclosing_type;
  }
  
  if(!el->tags[1] && el->tags[0]->enclosing_type == HTMLTagEnclosingType_Paired)
  {
    errors |= HTMLErrorType_wrong_enclosing_type;
  }
  
  else if(el->tags[1] && el->tags[0]->enclosing_type == HTMLTagEnclosingType_Paired && 
     el->tags[1]->type != el->tags[0]->type)
  {
    errors |= HTMLErrorType_wrong_enclosing_type;
  }
  
  parser->error.type |= errors;
  // parser->error.at = el->tags[0]->first_token.range.min;
}

internal void
html_analyse_parse_for_error(Arena *arena, HTMLParser *parser, HTMLTag tag)
{
  // TODO: push messages will not work because of the way we initialize the parser
  // TODO: check the enclosing type  

  HTMLErrorType type = 0;  
  RawTokenType first_token_flags = RawTokenType_angle_bracket_open|RawTokenType_angle_bracket_open_then_slash;
  RawTokenType last_token_flags = RawTokenType_angle_slash_then_bracket_close|RawTokenType_angle_bracket_close;
  
  if(tag.type == HTMLTagType_NULL)
  {
    // str8_list_push(arena, parser->error.messages, str8_lit("couldn't parse tag"));
    type |= HTMLErrorType_unexpected_token;
    
  }
  
  if(!(tag.first_token.type & first_token_flags))
  {
    // str8_list_push(arena, parser->error.messages, str8_lit("wrong matching tag"));
    type |= HTMLErrorType_unexpected_token;
    
  } 
  
  if(!(tag.last_token.type & last_token_flags))
  {
    // str8_list_push(arena, parser->error.messages, str8_lit("wrong matching tag"));
    type |= HTMLErrorType_unexpected_token;
    
  }  
  parser->error.type = type;
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
    
    HTMLTagTable table = html_get_tag_table();
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

typedef struct Raw Raw;
struct Raw
{
  String8        str;
  FileFormatType file_type;
};

internal HTMLTag *
html_pull_tag_info_from_inv(Arena *arena, HTMLParser *parser)
{
  HTMLTag *result = push_array_no_zero(arena, HTMLTag, 1);
  HTMLToken first_token = html_next_token(parser);
  HTMLToken last_token = html_next_token(parser);
  
  U64 end_byte = ClampTop(parser->string.size, last_token.range.min);
  U64 start_byte = ClampTop(end_byte, first_token.range.max);
  String8 tag_name = str8((parser->string.str + start_byte), end_byte-start_byte);
  
  HTMLTagInvariant *invariant = html_get_invariant_from_tag_name(arena, tag_name);
  if(invariant)
  {    
    result->type = invariant->type;
    result->enclosing_type = invariant->enclosing_type;
    result->tag_name = invariant->tag_name;
    result->text_types = invariant->text_types;
  }
  result->first_token = first_token;
  result->last_token = last_token;
  
  return result;
}

internal HTMLTag *
html_next_tag(Arena *arena, HTMLParser *parser)
{
  HTMLTag *result = {0};
  
  for(;;)
  {
    result = html_pull_tag_info_from_inv(arena, parser);
    
    if(result && parser->skip_until_tag_type == HTMLTagType_NULL) break;
    // NOTE: pre & code can have html meaningfull tokens, but are dummy
    else if(result->type == parser->skip_until_tag_type) break;
    // NOTE: first tag was dummy, but last tag could have been part of a meaningfull one
    parser->at = result->first_token.range.max;
  }
  
  if((result->type & HTMLTagType_PRE) || (result->type & HTMLTagType_CODE))
  {
    if(html_tag_is_opening(result))
    {
      parser->skip_until_tag_type = result->type;
    }
    else
    {
      parser->skip_until_tag_type &= ~result->type;
    }
  }
  
  return result;
}

internal HTMLElement *
html_parse_element(Arena *arena, HTMLParser *parser, HTMLTag *from_tag)
{
  HTMLElement *result = push_array(arena, HTMLElement, 1);  
  result->level_deep = ++parser->level_deep;

  if(from_tag)
  {
    result->tags[0] = from_tag;
  }
  else
  {
    result->tags[0] = html_next_tag(arena, parser);
  }

  if(result->tags[0]->enclosing_type == HTMLTagEnclosingType_Paired)
  {
    HTMLElement *last  = {0};
    HTMLElement *first = {0};
    HTMLTag     *tag   = {0};
    
    for(;;)
    {
      tag = html_next_tag(arena, parser);

      if(!html_is_parsing(parser) || !html_tag_is_opening(tag) &&
          tag->type == result->tags[0]->type)
      {
        result->tags[1] = tag;        
        break;
      }
      
      if(result && tag->type != result->tags[0]->type && !html_tag_is_opening(tag)) 
      {
        html_parser_set_error(arena, parser, tag->first_token.range.min,
                              HTMLErrorType_unexpected_token, 
                              str8_lit("From opening tag, finding a closing tag from different type is not allowed"));
        break;
      }
      
      HTMLElement *el = html_parse_element(arena, parser, tag);
      AppendLast(first, last, last->next_sibbling, el);
    }
    result->first_sub_element = first;
    result->content.str = parser->string.str + result->tags[0]->last_token.range.max;
    result->content.size = result->tags[1]->first_token.range.min - result->tags[0]->last_token.range.max;
  }
  
  html_analyse_element(parser, result);
  --parser->level_deep;
  
  return result;
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
    
    HTMLElementList *el_list = push_array(arena, HTMLElementList, 1);
    HTMLElement *first = {0};
    HTMLElement *last  = {0};
    StringJoin join = {0};
    join.post = str8_lit("\n");
    while(html_is_parsing(parser))
    {
      HTMLElement *element = html_parse_element(arena, parser, 0);
      AppendLast(first, last, last->next_sibbling, element);
      
      if(parser->error.type != HTMLErrorType_Null)
      {
        String8 error_msg = html_get_error_msg(arena, parser, file_name);
        str8_list_push(arena, error_messages, error_msg);
      } 
    }
    if(first)
    {
      html_element_list_push(arena, el_list, *first);
    }
    result = str8_list_join(arena, error_messages, &join);
  }
  return result;     
}

internal HTMLTag *
html_create_tag_from_text_types(Arena *arena, TextType types)
{
  HTMLTag *result = push_array(arena, HTMLTag, 1);
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
  //TODO: Do the real computation (search this line to see other)
  result->tags[0] = html_create_tag_from_text_types(arena, textual.type);
  result->tags[1] = html_create_tag_from_text_types(arena, textual.type);
  
  return result;
}

internal HTMLElement *
html_create_element_from_tag_type(Arena *arena, U64 type)
{
  HTMLElement *result = push_array(arena, HTMLElement, 1);  
  HTMLTagTable table = html_get_tag_table();
  for(U32 idx = 0; idx < table.tag_count; ++idx)
  {
    HTMLTagInvariant *invariant = &table.tags[idx];
    if(type == invariant->type)
    {
      //TODO: Do the real computation (search this line to see other)
      result->tags[0] = html_create_tag(arena, invariant);
      result->tags[1] = html_create_tag(arena, invariant);
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

internal void
html_append_first_sub(HTMLParser *parser, Arena *arena, HTMLElement *root, 
                      U64 tag_type_root, HTMLElement *el_to_append)
{
  //TODO: Never tested
  HTMLTag *invariant = html_create_tag_from_tag_type(arena, tag_type_root);
  
  if(!(tag_type_root & UNIQUE_TAGS) || 
      invariant->enclosing_type != HTMLTagEnclosingType_Paired)
  {
    str8_list_push(arena, parser->error.messages, str8_lit("Elements with unique tag can't have sub element"));
    parser->error.type = HTMLErrorType_wrong_enclosing_type;
  }
  
  for(HTMLElement *root_el = root; 
      root_el != 0; 
      root_el = root_el->first_sub_element)
  {
    if(root_el->tags[0]->type == tag_type_root)
    {
      el_to_append->next_sibbling = root_el->first_sub_element;
      root_el->first_sub_element = el_to_append;
      
      break;
    }
  }
}

internal HTMLElement *
html_get_root_doc(Arena *arena, String8 title)
{
  HTMLElement *doctype = html_create_element_from_tag_type(arena, HTMLTagType_DOCTYPE);
  HTMLElement *html    = html_create_element_from_tag_type(arena, HTMLTagType_HTML);
  HTMLElement *head    = html_create_element_from_tag_type(arena, HTMLTagType_HEAD);
  HTMLElement *body    = html_create_element_from_tag_type(arena, HTMLTagType_BODY);
  HTMLElement *div     = html_create_element_from_tag_type(arena, HTMLTagType_DIV);
  
  doctype->next_sibbling = html;
  html->first_sub_element = head;
  head->next_sibbling = body;
  body->first_sub_element = div;
  
  if(title.size)
  {
    HTMLElement *h1 = html_create_element_from_tag_type(arena, HTMLTagType_H1);
    
    String8 h1_content = push_str8_copy(arena, title);
    h1->content = h1_content;
    div->first_sub_element = h1;
  }
  
  return doctype;
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
          In short: textual / html element's first sub element differ. html element have +1 level relatioship 
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
        AppendLast(first_sub, last_sub, last_sub->next_sibbling, el);
      }
      
      root->next_sibbling = first_sub;
      AppendLast(first, last, last->next_sibbling, root);
      // TODO: macro not usable when unrolling two lists
      last = last_sub;
    }
    result = first;
  }
  
  return result;  
}
  
internal String8
html_str8_from_element(Arena *arena, HTMLElement *el, U8 indent)
{
  String8 result = {0};
  String8List *list_result = push_array(arena, String8List, 1);
  // TODO: check if new implementation fail
  U8 indentation = el->level_deep * indent;
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
      str = push_str8_cat(arena, str, el->tags[0]->tag_name);
      
      switch(el->tags[0]->enclosing_type)
      {
        case HTMLTagEnclosingType_Paired:
        {
          str = push_str8_cat(arena, str, str8_lit(">"));
          if(el->first_sub_element) 
          {
            str = push_str8_cat(arena, str, html_str8_from_element(arena, el->first_sub_element, indent));
          }
          str = push_str8_cat(arena, str, str8_lit("</"));
          str = push_str8_cat(arena, str, el->tags[0]->tag_name);
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
html_try_get_first_element_from_tag_type(HTMLElement *element, U64 type)
{
  HTMLElement *result = {0};
  for(HTMLElement *el = element; el != 0; el = el->next_sibbling)
  {
    if(el->tags[0]->type == type)
    {
      result = el;
      break;
    }
  } 
  return result;
}


internal String8
html_get_title_content(HTMLElement *el)
{
  String8 title_content = {0};
  
  for(HTMLElement *sib = el;
      sib != 0;
      sib = sib->next_sibbling)
  {
    if(el->tags[0]->type == HTMLTagType_H1)
    {
      title_content = el->content;
      break;
    }
    
    for(HTMLElement *sub_el = sib->first_sub_element;
        sub_el != 0;
        sub_el = sub_el->next_sibbling)
    {
      if(sub_el->tags[0]->type == HTMLTagType_H1)
      {
        title_content = sub_el->content;
        break;
      }
      html_get_title_content(sub_el->first_sub_element);
    }
    html_get_title_content(sib->first_sub_element);
  }
  
  return title_content;
}
