#ifndef OS_CORE_H
#define OS_CORE_H

typedef struct OS_Handle OS_Handle;
struct OS_Handle
{
  U64 u64[1];
};

typedef struct OS_HandleNode OS_HandleNode;
struct OS_HandleNode
{
  OS_HandleNode *next;
  OS_Handle handle;
};

typedef struct OS_HandleList OS_HandleList;
struct OS_HandleList
{
  OS_HandleNode *first;
  OS_HandleNode *last;
  U64           count;
};

typedef struct OS_HandleArray OS_HandleArray;
struct OS_HandleArray
{
  OS_Handle *handles;
  U64       count;
};

typedef U32 OS_FileIterFlags;
enum 
{
  OS_FileIterFlag_None           = (1 << 0),
  OS_FileIterFlag_SkipFolders    = (1 << 1),
  OS_FileIterFlag_SkipSubFolders = (1 << 2),
  OS_FileIterFlag_SkipFiles      = (1 << 3),
  OS_FileIterFlag_SkipHiddenFiles= (1 << 4),
  OS_FileIterFlag_Done           = (1 << 31)
};

typedef struct OS_FileIter OS_FileIter;
struct OS_FileIter 
{ 
  OS_FileIterFlags flags;
  String8          query;
  U8               memory[600];
};

typedef U32 FilePropertiesFlags;
enum 
{
   FilePropertyFlag_Unknown = (1 << 0),
   FilePropertyFlag_IsFolder = (1 << 1),
} os_handle_array_from_info_list;

typedef struct FileProperties FileProperties;
struct FileProperties
{
  U64                 size;
  String8             extension;
  FilePropertiesFlags flags;
};

typedef struct OS_FileInfo OS_FileInfo;
struct OS_FileInfo
{  
  String8             name; 
  String8             root_path; 
  FileProperties      props;
  
};

typedef struct OS_FileInfoNode OS_FileInfoNode;
struct OS_FileInfoNode
{
  OS_FileInfoNode *next;
  OS_FileInfo      info;
};

typedef struct OS_FileInfoList OS_FileInfoList;
struct OS_FileInfoList
{
  OS_FileInfoNode *first;
  OS_FileInfoNode *last;
  U64              count;
};

typedef struct OS_FileInfoArray OS_FileInfoArray;
struct OS_FileInfoArray
{
  OS_FileInfo *infos;
  U64          count;  
};

typedef U32 OS_AccessFlags;
enum
{
  OS_AccessFlag_Read       = (1<<0),
  OS_AccessFlag_Write      = (1<<1),
  OS_AccessFlag_Execute    = (1<<2),
  OS_AccessFlag_ShareRead  = (1<<3),
  OS_AccessFlag_ShareWrite = (1<<4),
};

internal OS_FileIter *    os_file_iter_begin(Arena *arena, String8 path, OS_FileIterFlags flags);
internal B32              os_file_iter_next(Arena *arena, OS_FileIter *iter, OS_FileInfo *info_out);
internal void             os_file_iter_end(OS_FileIter *iter);
internal void             os_push_files_infos(Arena *arena, String8 path, OS_FileIterFlags flags, OS_FileInfoList *list);

#endif