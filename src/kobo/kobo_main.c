#include <stdio.h>
#if defined(_WIN32)
#  define OS_WINDOWS 1
# elif defined(__gnu_linux__) || defined(__linux__)
#  define OS_LINUX 1
#endif

#include "base/base_inc.h"
#include "os/os_inc.h"
#include "html/html_inc.h"
#include "database/database_inc.h"
#include "third_party/stb/stb_sprintf.h"

#include "os/os_inc.c"
#include "base/base_inc.c"
#include "html/html_inc.c"
#include "database/database_inc.c"

static Textual column_meanings[] = {
    COL_TEXT("VolumeID",            TextType_Title),
    COL_TEXT("StartContainerPath",  TextType_Paragraph),
    COL_TEXT("EndContainerPath",    TextType_Paragraph),
    COL_TEXT("Text",                TextType_Paragraph),
    COL_TEXT("Annotation",          TextType_Heading1),
    COL_TEXT("DateCreated",         TextType_Date),
    COL_TEXT("ChapterProgress",     TextType_Code),    
    COL_TEXT("Type",                TextType_Code),
    COL_TEXT("BookmarkID",          TextType_Code),
};

internal void
kobo_format_column_value(Arena *arena, EntryDataDBList *list)
{
  for(EntryDataDBNode *node = list->first;
      node != 0; 
      node = node->next)
  {
      for(ColumnData *column = &node->entry.data;
          column != 0;
          column = column->next_sibbling)
      {        
        if(str8_match(column->name, str8_lit("VolumeID"), 0))
        {
          column->value = str8_from_last_slash(column->value);
          column->value = str8_chop_last_dot(column->value);
          // column->value = push_str8_cat(arena, str8_lit("VolumeID: "), column->value);
        }
        else if(str8_match(column->name,str8_lit("StartContainerPath"), 0))
        {
            // column->value = push_str8_cat(arena, str8_lit("StartContainerPath: "), column->value);
        }
        else if(str8_match(column->name, str8_lit("EndContainerPath"), 0))
        {
          // column->value = push_str8_cat(arena, str8_lit("EndContainerPath: "), column->value);
        }
        else if(str8_match(column->name, str8_lit("Text"), 0))
        {
          // column->value = push_str8_cat(arena, str8_lit("Text: "), column->value);
        }
        else if(str8_match(column->name, str8_lit("Annotation"), 0))
        {
          // column->value = push_str8_cat(arena, str8_lit("Annotation: "), column->value);
        }
        else if(str8_match(column->name, str8_lit("DateCreated"), 0))
        {
          // column->value = push_str8_cat(arena, str8_lit("DateCreated: "), column->value);
        }
        else if(str8_match(column->name, str8_lit("ChapterInProgess"), 0))
        {
          // column->value = push_str8_cat(arena, str8_lit("ChapterInProgess: "), column->value);
        }
        else if(str8_match(column->name, str8_lit("Type"), 0))
        {
          // column->value = push_str8_cat(arena, str8_lit("Type: "), column->value);
        }
        else if(str8_match(column->name, str8_lit("BookmarkID"), 0))
        {
          // column->value = push_str8_cat(arena, str8_lit("BookmarkID: "), column->value);
        }
      }    
  }
}

#if OS_WINDOWS
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
  Arena *perm_arena = arena_allocate__sized(GB(2), MB(128));
  char **argv = __argv;
  int argc = __argc;
  if(argc < 3) {printf("[path_to_sqlite_database] [path_output_file]"); return 0;}
  String8 root_path = os_current_directory(perm_arena);
  String8List arguments = os_string_list_from_argcv(perm_arena, (int)argc, argv);
  // TODO: make it work
  // CmdLine cmdline = cmd_line_from_string_list(perm_arena, arguments);  
  String8 out_path = arguments.first->next->next->string;
  String8 db_path = arguments.first->next->string;
  StateDB *state = database_init(perm_arena, TypeDB_SQLITE, str8_lit("sqlite3.dll"), db_path);
  
  if(!state->is_initialized) {printf("Error: could connect to database %s", argv[1]); return 0;}

  TextualTable *column_meanings_table = textual_get_table(perm_arena, column_meanings, ArrayCount(column_meanings));
  String8 query = str8_lit("SELECT * FROM Bookmark");
  ColumnData by = {0};
  by.name = str8_lit("VolumeID");
  
  EntryDataDBList *result = push_array(perm_arena, EntryDataDBList, 1);
  printf("DB: retrieving entries, grouping by %s", by.name.str);
  database_exec_push_list(perm_arena, query, state, column_meanings_table, result);
  if(!database_close(state))
  {
    printf("Error: unable to close the db connexion\n");
  }
  kobo_format_column_value(perm_arena, result);
  result = database_entries_group_by(perm_arena, result, by);
  
  TextualList *textual_list = push_array(perm_arena, TextualList, 1);
  for(EntryDataDBNode *entry_node = result->first; 
      entry_node != 0; 
      entry_node = entry_node->next)
  {
    Textual *textual = database_entries_to_textual(perm_arena, &entry_node->entry);
    textual_list_push(perm_arena, textual_list, textual);
  }
  
  HTMLElementList *el_list = push_array(perm_arena, HTMLElementList, 1);
  HTMLElement *root = html_get_root_doc(perm_arena); 
  
  for(TextualNode *node = textual_list->first;
      node != 0;
      node = node->next)
  {
    HTMLElement *el = html_element_from_textual(perm_arena, &node->textual);
    html_append_into(perm_arena, el, root);
    html_element_list_push(perm_arena, el_list, *root); 
  }
  
  for(HTMLElementNode *node = el_list->first;
      node != 0; 
      node = node->next)
  {
    HTMLParser parser = {0};
    parser.space_by_indent = 1;
    String8 output = html_str8_from_element(perm_arena, &node->element, &parser);
    String8 file_path = push_str8_cat(perm_arena, root_path, str8_lit("/"));
    HTMLElement *title = html_try_get_first_element_from_tag_type(&node->element, HTMLTag_h1);
    if(title)
    {
      file_path = push_str8_cat(perm_arena, file_path, title->content);
    }
    else
    {
      file_path = push_str8_cat(perm_arena, file_path, str8_lit("unknown_title"));
    }
    file_path = push_str8_cat(perm_arena, file_path, str8_lit("_annotations.txt"));
    if(!os_write_data_to_file_path(perm_arena, file_path, output))
    {
      printf("failed to write in file\n");
      break;
    }
  }
  os_library_close(state->lib);
  database_print_error(state);
  return 0;  
}

#elif OS_LINUX

int main(int argument_count, char **arguments)
{}

#endif
