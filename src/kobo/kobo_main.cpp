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

#include "os/os_inc.c"
#include "base/base_inc.c"
#include "html/html_inc.c"
#include "database/database_inc.c"


static ColumRawMeaning column_meanings[] = {
    COL_TEXT("VolumeID",            {RawStrenght_Highest, RawHLMeaning_Contextual}),
    COL_TEXT("StartContainerPath",  {RawStrenght_Lowest, RawHLMeaning_Contextual}),
    COL_TEXT("EndContainerPath",    {RawStrenght_Lowest, RawHLMeaning_Contextual}),
    COL_TEXT("Text",                {RawStrenght_Lowest, RawHLMeaning_Contextual}),
    COL_TEXT("Annotation",          {RawStrenght_Lowest, RawHLMeaning_Contextual}),
    COL_TEXT("DateCreated",         {RawStrenght_Lowest, RawHLMeaning_Time}),
    COL_TEXT("ChapterProgress",     {RawStrenght_Lowest, RawHLMeaning_Contextual}),
    COL_TEXT("Type",                {RawStrenght_Lowest, RawHLMeaning_Contextual}),
    COL_TEXT("BookmarkID",          {RawStrenght_Lowest, RawHLMeaning_Contextual}),
};


// 
// internal void
// kobo_format_column_value(Arena *arena, EntryDataDBList *list)
// {
//   for(EntryDataDBNode *node = list->first;
//       node != 0; 
//       node = node->next)
//   {
//       for(ColumnData *column = &node->entry.data;
//           column != 0;
//           column = column->next_sibbling)
//       {        
//         if(str8_match(column->name, str8_lit("VolumeID"), 0))
//         {
//           column->value = str8_from_last_slash(column->value);
//           column->value = str8_chop_last_dot(column->value);
//           // column->value = push_str8_cat(arena, str8_lit("VolumeID: "), column->value);
//         }
//         else if(str8_match(column->name,str8_lit("StartContainerPath"), 0))
//         {
//             // column->value = push_str8_cat(arena, str8_lit("StartContainerPath: "), column->value);
//         }
//         else if(str8_match(column->name, str8_lit("EndContainerPath"), 0))
//         {
//           // column->value = push_str8_cat(arena, str8_lit("EndContainerPath: "), column->value);
//         }
//         else if(str8_match(column->name, str8_lit("Text"), 0))
//         {
//           // column->value = push_str8_cat(arena, str8_lit("Text: "), column->value);
//         }
//         else if(str8_match(column->name, str8_lit("Annotation"), 0))
//         {
//           // column->value = push_str8_cat(arena, str8_lit("Annotation: "), column->value);
//         }
//         else if(str8_match(column->name, str8_lit("DateCreated"), 0))
//         {
//           // column->value = push_str8_cat(arena, str8_lit("DateCreated: "), column->value);
//         }
//         else if(str8_match(column->name, str8_lit("ChapterInProgess"), 0))
//         {
//           // column->value = push_str8_cat(arena, str8_lit("ChapterInProgess: "), column->value);
//         }
//         else if(str8_match(column->name, str8_lit("Type"), 0))
//         {
//           // column->value = push_str8_cat(arena, str8_lit("Type: "), column->value);
//         }
//         else if(str8_match(column->name, str8_lit("BookmarkID"), 0))
//         {
//           // column->value = push_str8_cat(arena, str8_lit("BookmarkID: "), column->value);
//         }
//       }    
//   }
// }

#if OS_WINDOWS
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
  Arena *perm_arena = arena_allocate__sized(GB(2), MB(128));
  char **argv = __argv;
  int argc = __argc;
  if(argc < 3) {printf("[path_to_sqlite_database] [path_output_file]"); return 0;}
  String8 root_path = os_current_directory(perm_arena);
  String8List arguments = os_string_list_from_argcv(perm_arena, (int)argc, argv);
  String8 out_path = arguments.first->next->next->string;
  String8 db_path = arguments.first->next->string;
  StateDB *state = database_init(perm_arena, TypeDB_SQLITE, str8_lit("sqlite3.dll"), db_path);
  
  if(!state->is_initialized) {printf("Error: could connect to database %s", argv[1]); return 0;}

  String8 query = str8_lit("SELECT * FROM Bookmark");
  ColumnData by = {0};
  by.name = str8_lit("VolumeID");
    
  DBRawDataTransformers db_transformers = {0};
  db_transformers.transformer_int = raw_default_integer_transformer;
  db_transformers.transformer_float =  raw_default_float_transformer;
// db_transformers.transformer_blob = raw_default_blob_transformer;
  db_transformers.transformer_string = raw_default_string_transformer;
  
  
  
  RawMeaningTable col_meaning_table = database_get_col_raw_meaning_table(perm_arena, column_meanings, ArrayCount(column_meanings));
  RawDataList *raws = push_array(perm_arena, RawDataList, 1);
  
  database_exec_push_raw_list(perm_arena, query, state, cols, col_meaning_table, default_raw_data_transformers);
    
  for(ColumnDataDB *col = cols.first;
      col != 0;
      col = col->next_sibbling;)
  {
    raw_data_list_push(perm_arena, raws, col->raw);
  }
  printf("DB: retrieving entries, grouping by %s", by.name.str);
  
  // kobo_format_column_value(perm_arena, raws);
  
  /*
    NOTE: EntryDataDBList -> Textual
  */
 
  TextualList *textual_list = push_array(perm_arena, TextualList, 1);
  for(RawDataNode *raw_node = raws->first;
      raw_node != 0; 
      raw_node = raw_node->next)
  {
    Textual textual = textual_from_raw(raw_node->raw);
    textual_list_push(perm_arena, textual_list, textual);
  }
  
  /*
    NOTE: Textual -> HTML
  */  
  HTMLElementList *el_list = push_array(perm_arena, HTMLElementList, 1);
  
  for(TextualNode *node = textual_list->first;
      node != 0;
      node = node->next)
  {
    String8 title_text = textual_get_title_content(&node->textual);
    if(title_text.size == 0)
    {
      printf("Couldn't found the title book, we still process without it ...\n");
    }
      
    HTMLElement *root = html_get_root_doc(perm_arena, title_text);
    HTMLElement *el = html_element_from_textual(perm_arena, &node->textual);
    html_append_into(perm_arena, el, root);
    html_element_list_push(perm_arena, el_list, *root);
  } 
  /*
    NOTE: HTML -> String + write to file
  */    
  for(HTMLElementNode *node = el_list->first;
      node != 0; 
      node = node->next)
  {
    String8 output = html_str8_from_element(perm_arena, &node->element, 1);
    String8 file_path = push_str8_cat(perm_arena, root_path, str8_lit("/"));
    String8 title_content = html_get_title_content(&node->element);
    
    if(title_content.size)
    {
      file_path = push_str8_cat(perm_arena, file_path, title_content);
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
  
  if(!database_close(state))
  {
    printf("Error: unable to close the db connexion\n");
  }
  os_library_close(state->lib);
  database_print_error(state);
  return 0;  
}

#elif OS_LINUX

int main(int argument_count, char **arguments)
{}

#endif
