internal U32 
u32_from_u64(U64 x) 
{
  U32 x32 = (x > max_U32)?max_U32:(U32)x;
  return(x32);
}

internal U64
u64_from_high_low_u32(U32 high, U32 low)
{
  U64 result = 0;
  result = (U64)(low) | ((U64)high << 32);
  return result;
}

  