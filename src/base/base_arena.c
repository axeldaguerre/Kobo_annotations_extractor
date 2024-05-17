internal Arena *
arena_allocate__sized(U64 init_res, U64 init_cmt)
{ 
  init_res = ClampBottom(ARENA_HEADER_SIZE,  init_res);
  init_cmt = ClampBottom(ARENA_HEADER_SIZE,  init_cmt);
  
  void *memory = os_reserve(init_res);
  if(!os_commit(memory, init_cmt)) {
    os_release(memory, init_cmt);
    memory = 0;
  }  
  AssertAlways(memory);
  
  Arena *arena = (Arena*)memory;
  if(arena) {
    arena->prev     = 0;
    arena->current  = arena;
    arena->base_pos = 0;
    arena->pos      = ARENA_HEADER_SIZE;
    arena->cmt      = init_cmt;
    arena->res      = init_res;
  }
  return arena;
}

internal Arena *
arena_allocate()
{
  U64 init_cmt = ARENA_COMMIT_SIZE;
  U64 init_res = ARENA_RESERVE_SIZE;
  Arena *arena = arena_allocate__sized(init_res, init_cmt);
  return arena;
}

internal U64
arena_pos_addr(Arena *arena)
{
  Arena *current = arena->current;
  U64 pos = current->base_pos + current->pos;
  return pos;
}

internal void *
arena_push(Arena *arena, U64 size)
{
  // TODO: memory alignment, profile it first to assert its awesomeness
  Arena *current = arena->current;
  AssertAlways(current != 0);
  U64 pos_new = current->pos + size;
  if(pos_new > arena->res)
  {
    /*
      TODO(Axel): is memory space alignment granurality useful here? 
      afaik win32 does it by itself.
    */    
    Arena *new_block = arena_allocate();
    if(new_block) 
    {
      new_block->base_pos = current->base_pos + current->res;
      SLLStackPush_N(arena->current, new_block, prev);
      current = new_block;
      pos_new = size;
    }
  }
  
  if(current->cmt < pos_new) 
  { 
    U64 cmt_new_size = pos_new - current->cmt;
    U64 cmt_new_size_clamped = ClampTop(current->res, cmt_new_size);
    B32 is_cmt_ok = os_commit((U8*)current + current->cmt, cmt_new_size_clamped);
    if(is_cmt_ok)
    {
        current->cmt += cmt_new_size_clamped;
    }
    //DWORD Error = GetLastError();
    AssertAlways(is_cmt_ok);
    AssertAlways(current->cmt <= current->res);
  }
  
  void *memory = 0;
  if(current->cmt >= pos_new)
  {
    memory = (U8*)current + current->pos;
    current->pos = pos_new;
  }
  
  AssertAlways(memory != 0);
  
  if(memory == 0)
  { 
    // os_graphical_message(arena, 1, str8_lit("Fatal Allocation Failure"), str8_lit("Unexpected memory allocation failure."));
    //os_exit_process(1);
  }
  return memory;
}

internal void
arena_pop_to(Arena *arena, U64 big_pos_unclamped)
{
  U64 big_pos = ClampBottom(ARENA_HEADER_SIZE, big_pos_unclamped);
  
  // unroll the chain (when more than one arena in the temp)
  Arena *current = arena->current;
  for (Arena *prev = 0; current->base_pos >= big_pos; current = prev) {
    prev = current->prev;
    os_release(current, current->res);
  }
  AssertAlways(current);
  arena->current = current;
  
  // compute arena-relative position
  U64 new_pos = big_pos - current->base_pos;
  AssertAlways(new_pos <= current->pos);
  
  // poison popped memory block
  AsanPoisonMemoryRegion((U8*)current + new_pos, (current->pos - new_pos));
  
  // update position
  current->pos = new_pos;
}

internal void  
arena_put_back(Arena *arena, U64 size)
{ 
  U64 old_pos = arena_pos_addr(arena);
  U64 new_pos = old_pos - size;
  arena_pop_to(arena, new_pos);
}

internal void
arena_clear(Arena *arena)
{
  arena_pop_to(arena, 0);
}

internal Temp
temp_begin(Arena *arena)
{
  U64 pos = arena_pos_addr(arena);
  Temp temp = {arena, pos};
  return temp;
}

internal void
temp_end(Temp temp)
{
  arena_pop_to(temp.arena, temp.pos);
}