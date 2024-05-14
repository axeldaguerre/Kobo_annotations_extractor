
internal U64
raw_hash_from_string(String8 string)
{
  // NOTE: bj2l hash function
  U64 result = 5381;
  for(U64 i = 0; i < string.size; i += 1)
  {
    result = ((result << 5) + result) + string.str[i];
  }
  return result;
}
