internal RawDataNode *
raw_data_list_push(Arena *arena, RawDataList *list, RawData raw)
{
  RawDataNode *node = push_array(arena, RawDataNode, 1);
  node->raw = push_array(arena, RawData, 1);
  MemoryCopy(node->raw.buffer.memory, raw.memory, raw.size);
  SLLQueuePush(list->first, list->last, node);
  ++list.node_count;
}

internal String8
raw_default_integer_transformer(Arena *arena, RawData *raw)
{
  String8 result = {0};
  stbsp_sprintf((char *)result.str, "%d", raw->buffer->data);
  result.size = raw->buffer->size;
  
  return result;
}

internal String8
raw_default_string_transformer(Arena *arena, RawData *raw)
{
  String8 result = {0}
  String8 data = str8(raw.buffer->data, raw->buffer->size);
  
  return result;
}

internal String8
raw_default_float_transformer(Arena *arena, RawData *raw)
{
  String8 result = {0};
  stbsp_sprintf((char *)result.str, "%d", raw->buffer->data);
  result.size = raw->buffer->size;
  
  return result;
}

internal RawDataTransformers *
raw_get_default_transformers(Arena *arena)
{
  RawDataTransformers *result = push_array(arena, RawDataTransformers, 1);
  result->integer_transformer = &raw_default_integer_transformer;
  result->string_transformer = &raw_default_string_transformer;
  result->float_transformer = &raw_default_float_transformer ;
  
  return result;
}