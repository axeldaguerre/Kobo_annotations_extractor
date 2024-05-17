#include <stdio.h>
#if defined(_WIN32)
#  define OS_WINDOWS 1
# elif defined(__gnu_linux__) || defined(__linux__)
#  define OS_LINUX 1
#endif

#include "base/base_inc.h"
#include "os/os_inc.h"
#include "html/html_inc.h"
#include "kobo/kobo_database.h"

#include "os/os_inc.c"
#include "base/base_inc.c"
#include "html/html_inc.c"
#include "kobo/kobo_database.c"

internal String8
full_col_names[] = 
{
  str8_lit("Annotation"),
  str8_lit("VolumeID"),
  str8_lit("Text"),
  str8_lit("DateCreated"),
  str8_lit("ChapterProgress"),
  str8_lit("Type"),
  str8_lit("ContentID"),
};

internal String8
light_col_names[] = 
{
  str8_lit("Annotation"),
  str8_lit("Text"),
  str8_lit("VolumeID"),
};


#if OS_WINDOWS
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
  Arena *perm_arena = arena_allocate__sized(GB(2), MB(128));
  char **argv = __argv;
  int argc = __argc;
  if(argc < 4) {printf("[path_to_sqlite_database] [path_output_file] [full/brief output]"); return 0;}
  
  String8 root_path     = os_current_directory(perm_arena);
  String8List arguments = os_string_list_from_argcv(perm_arena, (int)argc, argv);
  String8 db_path       = arguments.first->next->string;
  String8 out_path      = arguments.first->next->next->string;
  String8 full_output   = arguments.first->next->next->next->string;
  SQLiteState *state    = kobo_db_init(perm_arena, str8_lit("sqlite3.dll"), db_path);
  
  if(!state->is_initialized) 
  {
    printf("Error: could connect to database %s", argv[1]); return 0;
  }
  KoboColumnNameTable col_name_table = {0};
  
  if(str8_match(full_output, str8_lit("full"), 0))
  {
    col_name_table.names = full_col_names;
    col_name_table.count = ArrayCount(full_col_names);
  }
  else
  {
    col_name_table.names = light_col_names;
    col_name_table.count = ArrayCount(light_col_names);
  }  
  
  String8 query = str8_lit("SELECT ");
  String8List *cols = push_array(perm_arena, String8List, 1);
  StringJoin join = {0, 0, str8_lit(", ")};
  for(U64 col_idx = 0; col_idx < col_name_table.count; ++col_idx)
  {
    str8_list_push(perm_arena, cols, col_name_table.names[col_idx]);
  }
  query = push_str8_cat(perm_arena, query, str8_list_join(perm_arena, cols, &join));
  query = push_str8_cat(perm_arena, query, str8_lit(" FROM Bookmark ORDER BY VolumeID COLLATE NOCASE ASC;"));
  
  if(sqlite_prepare_query(query, state))
  {
    state->col_count = sqlite_column_count(state)-1;
  }
  else
  {
    state->errors = DBError_Query;
    return 0 ;
  }
  /* 
    NOTE: All book titles are treated as a a tree, in which the root node has the book title nodes
          then inside each book title node you have annotation nodes having the annotation details node in them
          root_node (dumb node having the entire nodes)
              |
          book_title_node (The book from which the bookmark was saved)
              |
          annotation_node (annotation title = Kobo user text while saving the bookmark)
              |
          annotation's_details_node (text, date created, bookmark position, etc... = automatically created)
              
  */
  RawDataNode *book_title_nodes = kobo_db_execute_create_raw(perm_arena, query, state);
  String8 file_path_root = push_str8_cat(perm_arena, root_path, str8_lit("/"));
  
  for(RawDataNode *n = book_title_nodes;
     !raw_is_nil(n);
      n = n->next)
  {
    Temp scratch = temp_begin(perm_arena);
    String8 file_path = {0};
    HTMLParserOutput parser_output = {0};
    parser_output.indent_str = str8_lit(" ");
    parser_output.max_text_width = 150;
    parser_output.space_by_indent = 1;
     
    HTMLParser *parser = html_init_parser(scratch.arena, {0}, &parser_output);
    String8 title = n->raw.data;    
    // HTMLElementNode *book_title_el = html_create_element_from_tag_type(scratch.arena, HTMLTag_DIV);
    HTMLElementNode *doc_content = html_create_root_doc(scratch.arena, title);
    html_el_node_from_raw_node(scratch.arena, n, doc_content);
    // HTMLElementNode *doc = html_append_to_default_doc(scratch.arena, title, book_title_el);
    
    String8 output = html_str8_from_element_no_check(scratch.arena, parser, doc_content->root->first);
    if(title.size == 0)
    {
        state->errors |= DBError_Query;
        break;
    }
    
    String8 start_after = str8_lit("mnt/onboard/");
    U64 start = str8_find_needle(title, 0, start_after, StringMatchFlag_CaseInsensitive);
    title.str += (start + start_after.size);
    String8List title_path_list = str8_split_by_string_chars(scratch.arena, title, str8_lit("<>:\"/\\|?*"), StringSplitFlag_KeepEmpties);
    join = {0, 0, str8_lit(" ")};
    String8 file_path_windows = str8_list_join(scratch.arena, &title_path_list, &join);
    file_path_windows = str8_chop_last_dot(file_path_windows);
    file_path = push_str8_cat(scratch.arena, file_path_root, file_path_windows);
    
    
    file_path = push_str8_cat(perm_arena, file_path, str8_lit(".html"));
    
    if(!os_write_data_to_file_path(perm_arena, file_path, output))
    {
      printf("failed to write in file\n");
      break;
    }
    temp_end(scratch);
    
  }
    
  if(!kobo_db_close(state))
  {
    state->errors |= DBError_Query;
  }
  os_library_close(state->lib);
  kobo_db_print_error(state);
  return 0;  
}

#elif OS_LINUX

int main(int argument_count, char **arguments)
{}

#endif
