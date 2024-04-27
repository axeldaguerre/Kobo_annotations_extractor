internal OS_Handle
os_handle_zero()
{
  OS_Handle handle = {0};
  return handle;
}

internal B32
os_handle_match(OS_Handle a, OS_Handle b)
{
  return a.u64[0] == b.u64[0];
}

internal OS_FileInfoNode*
os_file_info_list_push(Arena *arena, OS_FileInfoList *list, OS_FileInfo *info)
{  
  OS_FileInfoNode *node = push_array(arena, OS_FileInfoNode, 1);
  node->info = *info;
  SLLPush(list->first, list->last, node);
  list->count += 1;
  return node;
}


internal OS_FileInfoArray
os_file_info_array_from_list(Arena *arena, OS_FileInfoList *list)
{
  OS_FileInfoArray array;
  array.count = list->count;  
  array.infos = push_array_no_zero(arena, OS_FileInfo, array.count);
  U64 i = 0;
  for(OS_FileInfoNode *n = list->first; n != 0; n = n->next, ++i) {
    array.infos[i] = n->info;
  }  
  return array;
}

internal OS_HandleNode*
os_handle_list_push(Arena *arena, OS_HandleList *list, OS_Handle *handle)
{
  OS_HandleNode *node = push_array_no_zero(arena, OS_HandleNode, list->count);
  SLLPush(list->first, list->last, node);
  list->count += 1;
  return node;
}

internal OS_HandleArray
os_handle_array_from_list(Arena *arena, OS_HandleList *list)
{
  OS_HandleArray array;
  array.count = list->count;
  array.handles = push_array_no_zero(arena, OS_Handle, array.count);  
  U64 i = 0;
  for(OS_HandleNode *n = list->first; n != 0; n = n->next, ++i) {
    array.handles[i] = n->handle;
  }
  return array;
}

internal OS_FileInfoList *
os_file_infos_join_list(Arena *arena, OS_FileInfoList *a, OS_FileInfoList *b)
{
  OS_FileInfoList *list = push_array(arena, OS_FileInfoList, 1);
  for(OS_FileInfoNode *node = a->first; node != 0; node = node->next)
  {
    SLLPush(list->first, list->last, node);
  }
  for(OS_FileInfoNode *node = b->first; node != 0; node = node->next)
  {
    SLLPush(list->first, list->last, node);
  }
  return list;
}

internal void
os_push_files_infos(Arena *arena, String8 query, OS_FileIterFlags flags, OS_FileInfoList *list)
{
  OS_FileIter *iter = os_file_iter_begin(arena, query, flags);
  String8 root = query;
  if(str8_chop_last_dot(query).size != query.size || str8_ends_with(root, str8_lit("*"), 0))
  {
     root = str8_chop_last_slash(root);
  }
  if(iter)
  {
    do 
    {
      OS_FileInfo *info = push_array(arena, OS_FileInfo, 1);
      if(os_file_iter_next(arena, iter, info))
      {
        info->root_path = root;
        os_file_info_list_push(arena, list, info);
        if(info->props.flags & FilePropertyFlag_IsFolder)
        {
          String8 folder_path = push_str8_cat(arena, root, info->name);
          String8 folder_path_wildcard = push_str8_cat(arena, folder_path, str8_lit("\\*"));
          os_push_files_infos(arena, folder_path_wildcard, flags, list);
        } 
      }
    } while(!(iter->flags & OS_FileIterFlag_Done));
    os_file_iter_end(iter);
  }
}

internal B32
os_write_data_to_file_path(Arena *arena, String8 path, String8 data)
{
  B32 good = 0;
  OS_Handle file = os_file_open(arena, path, OS_AccessFlag_Write);
  if(!os_handle_match(file, os_handle_zero()))
  {
    good = 1;
    os_file_write(file, r1u64(0, data.size), data.str);
    os_file_close(file);
  }
  return good;
}

internal void
os_file_close(OS_Handle file)
{
  if(os_handle_match(file, os_handle_zero())) { return; }
  HANDLE handle = (HANDLE)file.u64[0];
  CloseHandle(handle);
}

internal String8List
os_string_list_from_argcv(Arena *arena, int argc, char **argv)
{
  String8List result = {0};
  for(int i = 0; i < argc; i += 1)
  {
    String8 str = str8_cstring(argv[i]);
    str8_list_push(arena, &result, str);
  }
  return result;
}
