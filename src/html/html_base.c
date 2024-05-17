internal B32
html_is_nil(HTMLElementNode *node)
{
  return node == 0 || node == &html_el_n_g_nil;
}

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

internal B32  
html_child_is_inside_parent_tags(Arena *arena, HTMLElementNode *parent, HTMLElementNode *child)
{
  // TODO: It's a very simple implementation, will change in the future when doing more complex stuff. But even now for simple cases it's not complete.
  B32 result = 1;
  if(!parent || !child) return result;
  HTMLTag *parent_tag = parent->element.tags[0];
  HTMLTag *child_tag = child->element.tags[0];

  if(parent_tag->tag == HTMLTag_DIV)
  {
    return 1;
  }
  
  if(child_tag->flow_type == HTMLTagFlowContentType_Block)
  {
    if(parent_tag->tag == HTMLTag_H1       ||
       parent_tag->tag == HTMLTag_H2       ||
       parent_tag->tag == HTMLTag_H3       ||
       parent_tag->tag == HTMLTag_H4       ||
       parent_tag->tag == HTMLTag_H5       ||
       parent_tag->tag == HTMLTag_H6       ||
       parent_tag->tag == HTMLTag_A        ||
       parent_tag->tag == HTMLTag_B        ||
       parent_tag->tag == HTMLTag_BR       ||
       parent_tag->tag == HTMLTag_BUTTON   ||
       parent_tag->tag == HTMLTag_CODE     ||
       parent_tag->tag == HTMLTag_EM       ||
       parent_tag->tag == HTMLTag_IMG      ||
       parent_tag->tag == HTMLTag_INPUT    ||
       parent_tag->tag == HTMLTag_LABEL    ||
       parent_tag->tag == HTMLTag_SMALL    ||
       parent_tag->tag == HTMLTag_SPAN     ||
       parent_tag->tag == HTMLTag_STRONG   ||
       parent_tag->tag == HTMLTag_TEXTAREA ||
       parent_tag->tag == HTMLTag_U)
    {
      result = 0;
    }
  }
  
  return result;
}

internal HTMLTag *
html_tag_from_inv(Arena *arena, HTMLTagInvariant *inv)
{
  HTMLTag *result = push_array(arena, HTMLTag, 1);
  if(inv)
  {
    result->tag = inv->tag;
    result->content_type = inv->content_type;
    result->flow_type = inv->flow_type;
    result->enclosing_type = inv->enclosing_type;
    result->tag_name = inv->tag_name;
    result->meaning = inv->meaning;
  }
    
  return result;
}

internal HTMLParser *
html_init_parser(Arena *arena, String8 str, 
                 HTMLParserOutput *output)
{
  HTMLParser *parser = push_array(arena, HTMLParser, 1);
  if(str.size)
  {
    parser->string = push_str8_copy(arena, str);
    parser->string = str;
  }
  parser->error.messages = push_array(arena, String8List, 1);  
  parser->output = output;
  if(parser->output)
  {
    U64 count = parser->output->space_by_indent;
    while(count)
    {
      parser->output->indent_str_one = push_str8_cat(arena, parser->output->indent_str, parser->output->indent_str_one);
      --count;
    }
  }
  // parser->output.indent_ws = push_str8_copy(arena, str8_lit(""));
  
  return parser;
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
    HTMLParser *parser = html_init_parser(arena, str8(memory, size), {0});
    
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
html_get_root_doc_content_node(HTMLElementNode *root)
{
  // TODO: will break one day
  HTMLElementNode *result = root->next->first->next->first;
  return result;
  
}

internal HTMLElementAttributeNode *
html_attribute_push(Arena *arena, HTMLElementAttribute attribute, 
                    HTMLElementAttributeList *list)
{  
  HTMLElementAttributeNode *n = push_array(arena, HTMLElementAttributeNode, 1);
  SLLQueuePush(list->first, list->last, n);
  n->attribute = attribute;
  
  return n;
}

internal void
html_create_attribute_push(Arena *arena, String8 name, String8 value, HTMLElement *el)
{
  HTMLElementAttribute *attribute = push_array(arena, HTMLElementAttribute, 1);
  attribute->name = name;
  attribute->value = value;
  
  if(!el->attributes)
  {
    el->attributes = push_array(arena, HTMLElementAttributeList, 1); 
  }
  html_attribute_push(arena, *attribute, el->attributes);
}

/*
  NOTE: Return the element in which the content will have to be appends
*/
internal HTMLElementNode *
html_create_root_doc(Arena *arena, String8 doc_title)
{
  HTMLElementNode *root = push_array(arena, HTMLElementNode, 1);
  
  HTMLElementNode *doctype = html_create_element_from_tag_type(arena, HTMLTag_DOCTYPE);
  DLLPushBack_NPZ(&html_el_n_g_nil, root->first, root->last, doctype, next, prev);
  doctype->root = root;
  html_create_attribute_push(arena, {0}, str8_lit("html"), &doctype->element);
  
  HTMLElementNode *html = html_create_element_from_tag_type(arena, HTMLTag_HTML);  
  DLLPushBack_NPZ(&html_el_n_g_nil, root->first, root->last, html, next, prev);
  html->root = root;
  html_create_attribute_push(arena, str8_lit("lang"), str8_lit("en"), &html->element);
  
  HTMLElementNode *head  = html_create_element_from_tag_type(arena, HTMLTag_HEAD);
  head->parent = html; 
  head->root = root;
  
  HTMLElementNode *body = html_create_element_from_tag_type(arena, HTMLTag_BODY);
  body->parent = html; 
  body->root = root;
  
  HTMLElementNode *div = html_create_element_from_tag_type(arena, HTMLTag_DIV);
  div->parent = body;  
  div->root = root;
  
  DLLPushBack_NPZ(&html_el_n_g_nil, body->first, body->last, div, next, prev);
  DLLPushBack_NPZ(&html_el_n_g_nil, html->first, html->last, head, next, prev);
  DLLPushBack_NPZ(&html_el_n_g_nil, html->first, html->last, body, next, prev);
  doctype->next = html;
  
  if(doc_title.size == 0) return doctype;
  HTMLElementNode *title = html_create_element_from_tag_type(arena, HTMLTag_TITLE);
  title->element.raw.data = push_str8_copy(arena, doc_title);
  title->parent = head; 
  DLLPushBack_NPZ(&html_el_n_g_nil, head->first, head->last, title, next, prev);
  
  return div;
}

internal String8
html_str8_from_element_no_check(Arena *arena, HTMLParser *parser, HTMLElementNode *root)
{
  String8 result = {0};
  if(!root) return result;
  String8 indent_ws = {0};
  U64 indentation_count = ClampBottom(0, (parser->open_tag_count * parser->output->space_by_indent));
  while(indentation_count)
  {
    indent_ws = push_str8_cat(arena, parser->output->indent_str, indent_ws);
    --indentation_count;
  }
  
  StringJoin join = {0};
  join.sep = indent_ws;
  String8 el_str8 = {0};
  for(HTMLElementNode *n = root;
      !html_is_nil(n);
      n = n->next)
  {
    HTMLTag *tag = n->element.tags[0];
    // NOTE: '§' is for indentation   
    el_str8 = push_str8_cat(arena, el_str8, str8_lit("§")); 
    el_str8 = push_str8_cat(arena, el_str8, push_str8f(arena, "<%S", tag->tag_name));
    HTMLElementAttributeList *attributes = n->element.attributes;
    // TODO: avoid the check, constructor (function)?
    if(attributes)
    {
      for(HTMLElementAttributeNode *attribute_n = attributes->first;
          attribute_n != 0;
          attribute_n = attribute_n->next)
      {
          el_str8 = push_str8_cat(arena, el_str8, str8_lit(" ")); 
          if(attribute_n->attribute.name.size !=0)
          {
            el_str8 = push_str8_cat(arena, el_str8, push_str8f(arena, "%S=\"%S\"", attribute_n->attribute.name, attribute_n->attribute.value)); 
          }
          else
          {
            el_str8 = push_str8_cat(arena, el_str8, attribute_n->attribute.value);
          }
      }
    }    
    switch(tag->enclosing_type)
    {
      case HTMLTagEnclosingType_Paired:
      {
        el_str8 = push_str8_cat(arena, el_str8, str8_lit(">"));
        el_str8 = push_str8_cat(arena, el_str8, n->element.raw.data);
        
        B32 are_childrens_inside = html_child_is_inside_parent_tags(arena, n, n->first);        
        String8 children_el_str8 = html_str8_from_element_no_check(arena, parser, n->first);
        if(!are_childrens_inside)
        {
          parser->open_tag_count++;
          el_str8 = push_str8_cat(arena, el_str8, push_str8f(arena, "</%S>", tag->tag_name)); 
        }
        if(children_el_str8.size)
        {
          el_str8 = push_str8_cat(arena, el_str8, str8_lit("\n§"));
          el_str8 = push_str8_cat(arena, el_str8, children_el_str8);
        }
        if(are_childrens_inside)
        {
          el_str8 = push_str8_cat(arena, el_str8, push_str8f(arena, "</%S>", tag->tag_name)); 
          
        }
      } break;
      case HTMLTagEnclosingType_Self:
      {
        el_str8 = push_str8_cat(arena, el_str8, str8_lit("/>"));
      } break;
      case HTMLTagEnclosingType_Unique:
      {
        el_str8 = push_str8_cat(arena, el_str8, str8_lit(">"));
      } break;
    }
    //TODO: a function exist for that Clamp, computation is failing anyway
    if(parser->open_tag_count == 0)
    {
      parser->open_tag_count = 0;
    }
    else
    {
      --parser->open_tag_count;
    }
    el_str8 = push_str8_cat(arena, el_str8, str8_lit("\n§"));
  }
    
  String8List list_result = str8_split_by_string_chars(arena, el_str8, str8_lit("§"), 0);
  result = str8_list_join(arena, &list_result, &join); 
  
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
html_create_el_from_raw(Arena *arena, RawData *raw)
{
  HTMLElementNode *result = push_array(arena, HTMLElementNode, 1);
  HTMLTag *tag = html_tag_from_inv(arena, html_find_first_tag_from_meaning(arena, raw->meaning));
  result->element.tags[0] = result->element.tags[1] = tag;
  result->element.raw.meaning = raw->meaning;
  result->element.raw.data = push_str8_copy(arena, raw->data);
  
  return result;
}

internal HTMLElementNode *
html_append_to_default_doc(Arena *arena, String8 doc_title, HTMLElementNode *el)
{
  HTMLElementNode *root_doc = html_create_root_doc(arena, doc_title);
  // TODO (URGENT): will break
  el->parent = root_doc->next->first->next->first; // div
  root_doc->next->first->next->first = el;         // el->parent = div
  
  return root_doc;
}

internal void
html_el_node_from_raw_node(Arena *arena, RawDataNode *raw_node, HTMLElementNode *parent)
{    
    /*
      NOTE: RawData and HTMLElement differ in terms of relation.
            - RawData's next and first means relation in their meaning
            - HTMElement's next and first means between the parent tag or sibbling to them
    */
  for(RawDataNode *n = raw_node;
      !raw_is_nil(n);
      n = n->next)
  {
    HTMLElementNode *el_node = html_create_el_from_raw(arena, &n->raw);
    el_node->parent = parent;
    el_node->root = parent->root;
    html_el_node_from_raw_node(arena, n->first, el_node);
    DLLPushBack_NPZ(&html_el_n_g_nil, parent->first, parent->last, el_node, next, prev);
    if(n->next && !n->next->parent) break; // NOTE: the next node is a root node, it's a new html doc
  }
}
