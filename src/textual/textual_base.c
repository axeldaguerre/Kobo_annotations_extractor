internal TextualNode*
textual_list_push(Arena *arena, TextualList *list, Textual *textual)
{ 
  TextualNode *node = push_array(arena, TextualNode, 1);
  MemoryCopyStruct(&node->textual, textual);
  node->textual.text = push_str8_copy(arena, textual->text);
  SLLQueuePush(list->first, list->last, node);
  list->node_count += 1;
  return node;
}

internal TextType 
textual_type_from_name(String8 name, TextualTable *table)
{
  TextType result = TextType_Null;
  for(U8 idx = 0; idx < table->count; ++idx)
  {
    Textual textual = table->textuals[idx];
    if(str8_match(name, textual.text, 0))
    {
      result = textual.type;
      break;
    }
  }
  return result;
}