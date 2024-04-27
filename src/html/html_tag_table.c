internal HTMLTagInvariant html_tag_table[] =
{
 #include "html_tag_table.inl"
};

internal HTMLTagTable
html_get_tag_table()
{
  HTMLTagTable result = {0};
  result.tag_count = ArrayCount(html_tag_table);
  result.tags = html_tag_table;
  return result;
}

