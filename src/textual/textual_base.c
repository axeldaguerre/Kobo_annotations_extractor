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
  TextType text_type = TextType_Null;
  for(U8 idx = 0; idx < table->count; ++idx)
  {
    Textual textual = table->textuals[idx];
    if(str8_match(name, textual.text, 0))
    {
      text_type = textual.type;
      break;
    }
  }
  
  return text_type;
}

/*
  NOTE: We don't bother searching inside the whole textual list, 
        because a properly constructed Textual will have the title at first
*/
internal String8
textual_get_title_content(Textual *root)
{
  String8 title = {0};
  for(Textual *textual = root;
      textual != 0;
      textual =  textual->next_sibbling)
  {
    if(textual->type == TextType_Title)
    {
      title = textual->text;
    }
  }  
  
  return title;
}

internal TextType
textual_type_from_raw(RawMeaning raw_meaning)
{
  TextType result = TextType_Null;
  
  switch(raw_meaning.high_level_meaning)
  {
    
    case RawHLMeaning_Contextual:
    {
      if(raw_meaning.strenght == RawStrenght_Lowest)
      {
        result = TextType_Heading4;
      }
      else if(raw_meaning.strenght == RawStrenght_Low)
      {
        result = TextType_Heading3;
      }
      else if(raw_meaning.strenght == RawStrenght_Medium)
      {
        result = TextType_Heading2;
      }
      else if(raw_meaning.strenght == RawStrenght_AboveMedium)
      {
        result = TextType_Heading1;
      }
      else if(raw_meaning.strenght == RawStrenght_Highest)
      {
        result = TextType_Title;
      }
    } break;

    case RawHLMeaning_Time:
    {
      result = TextType_Code;
    } break;
  }
  
  
  return result;
}

internal Textual *
textual_from_raw(Arena *arena, RawData *raw)
{
  Textual *first = {0};
  Textual *last = {0};
  for(RawData *chunk = raw;
      chunk != 0;
      chunk = chunk->next)
  {
    Textual *textual = push_array(arena, Textual, 1);
    String8 text = {0};
    text.str = push_array(arena, U8, chunk->buffer.size);
    text.size = chunk->buffer.size;
    chunk->transformer(chunk->buffer, text);
    textual->text = text;
    textual->type = textual_type_from_raw(chunk->meaning);
    AppendLast(first, last, last->next_sibbling, textual);
  }
  
  return first; 
}