#include <Windows.h>

typedef struct W32_FileIter W32_FileIter; 
struct W32_FileIter
{
    HANDLE handle;
    WIN32_FIND_DATAW find_data;
};


internal void *        os_reserve(U64 size);
internal B32           os_commit(void *ptr, U64 size);
internal void          os_release(void *ptr, U64 size);

internal void          os_graphical_message(Arena *arena, B32 error, String8 title, String8 message);

internal String8       os_string_from_system_path(Arena *arena);

internal void          os_exit_process(S32 exit_code);

internal OS_Handle     os_file_open(Arena *arena, String8 path, OS_AccessFlags flags);
internal void          os_file_write(OS_Handle file, Rng1U64 rng, void *data);
internal U64           os_file_read(OS_Handle file, Rng1U64 rng, void *out_data);
internal void          os_file_close(OS_Handle file);
internal OS_FileIter * os_file_iter_begin(Arena *arena, String8 query, OS_FileIterFlags flags);
internal B32           os_file_iter_next(Arena *arena, OS_FileIter *iter, OS_FileInfo *out_info);
internal void          os_file_iter_end(OS_FileIter *iter);
internal OS_Handle     os_library_open(Arena *arena, String8 path);
internal void          os_library_close(OS_Handle lib);
internal VoidProc*     os_library_load_proc(Arena *arena, OS_Handle lib, String8 name);