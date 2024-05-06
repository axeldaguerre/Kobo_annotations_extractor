#pragma comment(lib, "user32")
#pragma comment(lib, "shell32")
#include <Windows.h>
// ================ Memory ==================
internal void * 
os_reserve(U64 size)
{ 
  void *result = VirtualAlloc(0, size, MEM_RESERVE, PAGE_READWRITE);
  return result;
}

internal B32
os_commit(void *ptr, U64 size)
{   
  B32 result = (VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE) != 0);
  return result;
}

internal String8
os_string_from_system_path(Arena *arena)
{
    Temp scratch = temp_begin(arena);
    DWORD size = KB(32);
    U16 *buffer = push_array_no_zero(scratch.arena, U16, size);
    DWORD buffer_size = GetModuleFileNameW(0, (WCHAR*)buffer, size);
    String8 result = str8_from_16(scratch.arena, str16(buffer, buffer_size));
    temp_end(scratch);
    return result;
}

internal void 
os_release(void *ptr, U64 size)
{
    // Size not used in Windows but on used on others
    VirtualFree(ptr, 0, MEM_RELEASE);
}

// ================ File ==================
internal OS_Handle
os_file_open(Arena *arena, String8 path, OS_AccessFlags flags)
{
    OS_Handle result = {0};
    Temp scratch = temp_begin(arena);
    String16 path16 = str16_from_str8(scratch.arena, path);
    DWORD access_flags = 0;
    DWORD share_mode = 0;
    DWORD creation_disposition = OPEN_EXISTING;
    if(flags & OS_AccessFlag_Read)    {access_flags |= GENERIC_READ;}
    if(flags & OS_AccessFlag_Execute) {access_flags |= GENERIC_EXECUTE;}
    if(flags & OS_AccessFlag_ShareRead)  {share_mode |= FILE_SHARE_READ;}
    if(flags & OS_AccessFlag_ShareWrite) {share_mode |= FILE_SHARE_WRITE;}
    if(flags & OS_AccessFlag_Write)   {creation_disposition = CREATE_ALWAYS;}
    if(flags & OS_AccessFlag_Write)   {access_flags |= GENERIC_WRITE;}

    HANDLE file = CreateFileW((WCHAR *)path16.str, access_flags, FILE_SHARE_READ, 0, creation_disposition, 0, 0);
    // DWORD Error = GetLastError();
    if(file != INVALID_HANDLE_VALUE)
    {
        result.u64[0] = (U64)file;
    }
    temp_end(scratch);    
    return result;
}

internal void
os_file_write(OS_Handle file, Rng1U64 rng, void *data)
{   
    if(os_handle_match(file, os_handle_zero())) return;

    HANDLE w32_handle = (HANDLE)file.u64[0];
    U64 to_write_size = (rng.max - rng.min);
    U64 total_write_size = 0;
    for(;total_write_size < to_write_size;)
    {
        void *memory = (void*)((U8*)data + total_write_size);
        U64 write_size = 0;
        U64 amount_64 = (to_write_size - total_write_size);
        // TODO(Axel): Test the maximum write size
        U32 amount_32 = u32_from_u64(amount_64);
        BOOL success  = WriteFile(w32_handle, memory, (DWORD)amount_32,  (DWORD*)&write_size, 0);
        DWORD Error = GetLastError();
        if(Error == 5)
        {
          printf("Access denied: can't write in file");
        }
        if(success == 0) break;
        total_write_size += write_size;
    }
}

internal U64
os_file_read(OS_Handle file, Rng1U64 rng, void *out_data)
{
    if(os_handle_match(file, os_handle_zero())) { return 0; };
    HANDLE w32_handle = (HANDLE)file.u64[0];
    U64 file_size;
    GetFileSizeEx(w32_handle, (LARGE_INTEGER *)&file_size);
    Rng1U64 rng_clamped = rng_1u64( ClampTop(rng.min, file_size), ClampTop(rng.max, file_size) );
    U64 to_read = (rng_clamped.max - rng_clamped.min);
    U64 total_read_size = 0;
    for(;total_read_size < to_read;)
    {
        U64 amount_64 = (to_read - total_read_size);
        // TODO(Axel): Test the maximum read size
        U32 amount_32 = u32_from_u64(amount_64);
        // TODO(Axel): implement asyncronous (overlapped)
        DWORD bytes_read = 0;
        BOOL SUCCESS = ReadFile(w32_handle, (U8*)(out_data) + total_read_size, (DWORD)amount_32, &bytes_read, 0);
        if(SUCCESS == 0) break;
        total_read_size += bytes_read;
    }

    return total_read_size;
}


internal OS_FileIter*
os_file_iter_begin(Arena *arena, String8 query, OS_FileIterFlags flags)
{
    OS_FileIter *iter = push_array(arena, OS_FileIter, 1);    
    iter->query = query;
    iter->flags = flags;
    W32_FileIter *w32_iter = (W32_FileIter *)iter->memory;
    String16 path_16 = str16_from_str8(arena, iter->query);
    w32_iter->handle = FindFirstFileW((WCHAR *) path_16.str, &w32_iter->find_data);
    return iter;
}

internal B32
os_file_iter_next(Arena *arena, OS_FileIter *iter, OS_FileInfo *out_info)
{
    B32 result = 0;
    W32_FileIter *w32_iter = (W32_FileIter*)iter->memory;
    
    if(!(iter->flags & OS_FileIterFlag_Done) && w32_iter->handle != INVALID_HANDLE_VALUE) 
    {      
      do {
          B32 usable_file = 1;
          FilePropertiesFlags flags_properties = FilePropertyFlag_Unknown;
          DWORD attributes =  w32_iter->find_data.dwFileAttributes;
          WCHAR *file_name = w32_iter->find_data.cFileName;
          if (file_name[0] == '.')
          {
            if (iter->flags & OS_FileIterFlag_SkipHiddenFiles){
            usable_file = 0;
            }
          }
          else if (file_name[1] == 0 || file_name[0] == 0)
          {
            usable_file = 0;
          }
          else if (file_name[1] == '.' && file_name[2] == 0)
          {
            usable_file = 0;
          }
          
          if(attributes & FILE_ATTRIBUTE_DIRECTORY)
          {
            flags_properties |= FilePropertyFlag_IsFolder;
            if(iter->flags & OS_FileIterFlag_SkipFolders)
            {
                usable_file = 0;
            }
          }
          else {
            if(iter->flags & OS_FileIterFlag_SkipFiles)
            {
              usable_file = 0;
            }        
          }
          
          if(usable_file)
          {
            result = 1;
            out_info->name              = str8_from_16(arena, str16_cstring((U16*)file_name));
            out_info->props.size        = u64_from_high_low_u32(w32_iter->find_data.nFileSizeHigh, w32_iter->find_data.nFileSizeLow);
            out_info->props.extension   = str8_cut_from_last_dot(out_info->name); //TODO: remove this as it's in the name
            out_info->props.flags       = flags_properties;
            if(!FindNextFileW(w32_iter->handle, &w32_iter->find_data))
            {
                iter->flags |= OS_FileIterFlag_Done;
            }
            break;
          }
          
      } while(FindNextFileW(w32_iter->handle, &w32_iter->find_data));
    }
    
    if(!result)
    {
      iter->flags |= OS_FileIterFlag_Done;
    }
    return result;
}

internal void
os_file_iter_end(OS_FileIter *iter)
{
  W32_FileIter *w32_iter = (W32_FileIter*)iter->memory;
  FindClose(w32_iter->handle);
}

internal void
os_exit_process(S32 exit_code){
  exit(exit_code);
}

internal String8 os_current_directory(Arena *arena)
{
  Temp scratch = temp_begin(arena);
  DWORD length = GetCurrentDirectoryW(0, 0);
  U16 *memory = push_array_no_zero(scratch.arena, U16, length + 1);
  length = GetCurrentDirectoryW(length + 1, (WCHAR*)memory);
  temp_end(scratch);
  String8 dir_path = str8_from_16(arena, str16(memory, length));
  return dir_path;
}

internal String8 os_exe_path(Arena *arena)
{
  Temp scratch = temp_begin(arena);
  DWORD size = KB(32);
  U16 *buffer = push_array_no_zero(scratch.arena, U16, size);
  DWORD length = GetModuleFileNameW(0, (WCHAR*)buffer, size);
  temp_end(scratch);
  String8 exe_path = str8_from_16(scratch.arena, str16(buffer, length));
  return exe_path;
}

internal OS_Handle
os_library_open(Arena *arena, String8 path)
{
  Temp scratch = temp_begin(arena);
  String16 path16 = str16_from_str8(scratch.arena, path);
  HMODULE mod = LoadLibraryW((LPCWSTR)path16.str);
  OS_Handle result = { (U64)mod };
  temp_end(scratch);
  return(result);
}

internal VoidProc*
os_library_load_proc(Arena *arena, OS_Handle lib, String8 name)
{
  Temp scratch = temp_begin(arena);
  HMODULE mod = (HMODULE)lib.u64[0];
  name = push_str8_copy(scratch.arena, name);
  VoidProc *result = (VoidProc*)GetProcAddress(mod, (LPCSTR)name.str);
  temp_end(scratch);
  return(result);
}

internal void
os_library_close(OS_Handle lib)
{
  HMODULE mod = (HMODULE)lib.u64[0];
  FreeLibrary(mod);
}

