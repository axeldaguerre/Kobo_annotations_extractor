
internal HTMLTagInvariantTable
html_get_tag_table()
{
  HTMLTagInvariantTable result = {0};
  result.tag_count = ArrayCount(html_tag_table);
  result.tags = html_tag_table;
  
  return result;
}
