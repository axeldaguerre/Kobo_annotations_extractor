internal RawDataNode *
raw_data_list_push(Arena *arena, RawDataList *list, RawData raw)
{
  RawDataNode *node = push_array(arena, RawDataNode, 1);
  MemoryCopy(node->raw.buffer.data, raw.buffer.data, raw.buffer.size);
  SLLQueuePush(list->first, list->last, node);
  ++list->node_count;
}

internal void
raw_default_integer_transformer(Buffer buffer, String8 output)
{

  if(output.size < buffer.size)
  {
    stbsp_sprintf((char *)output.str, "%d", buffer.data);
    output.size = buffer.size;
  }
}

internal void
raw_default_float_transformer(Buffer buffer, String8 output)
{
  if(output.size < buffer.size)
  {
    stbsp_sprintf((char *)output.str, "%d", buffer.data);
    output.size = buffer.size;
  }
}

