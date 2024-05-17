internal B32
raw_is_nil(RawDataNode *raw)
{
  return raw == 0 || raw == &raw_node_g_nil;
}
