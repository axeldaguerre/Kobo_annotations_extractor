#define ARENA_HEADER_SIZE 128 
#define ARENA_COMMIT_SIZE KB(64)
#define ARENA_RESERVE_SIZE MB(64)

typedef struct Arena Arena;
struct Arena
{
  Arena *current;
  Arena *prev;
  U64    base_pos; // init_cmt granularity
  U64    pos;
  U64    cmt;
  U64    res; 
};

typedef struct Temp Temp;
struct Temp
{  
  Arena *arena;
  U64 pos;
};

internal void * arena_push(Arena *arena, U64 size);
internal void   arena_pop_to(Arena *arena, U64 big_pos_unclamped);
internal void   arena_put_back(Arena *arena, U64 size);

internal Arena* arena_allocate__sized(U64 init_res, U64 init_cmt);
internal Arena* arena_allocate();

internal void temp_end(Temp temp);
internal Temp temp_begin(Arena *arena);

#define temp_begin(arena) temp_begin(arena)
#define temp_end(temp)    temp_end(temp)

#define push_array_no_zero(a, T, c)(T*)(arena_push((a), sizeof(T)*(c)))
#define push_array(a,T,c) (T*)MemoryZero(push_array_no_zero(a,T,c), sizeof(T)*(c))