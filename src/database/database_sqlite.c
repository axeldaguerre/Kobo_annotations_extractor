
/*
  TODO: should not be here, but because of C/C++ compiler ...
        Solution: pulling entry iteration from sqlite into base should fix it
*/

internal ColumnDataDBNode*
database_entry_list_push(Arena *arena, ColumnDataDBList *list, ColumnDataDB *entry)
{ 
  ColumnDataDBNode *node = push_array(arena, ColumnDataDBNode, 1);
  node->entry.col_name = push_str8_copy(arena, entry->col_name);
  SLLQueuePush(list->first, list->last, node);
  list->node_count += 1;  
  return node;
}

internal B32
sqlite_open_db(String8 db_path, StateDB *state)
{
  B32 result = 1;
  SQLiteState *sqlite_state = (SQLiteState *)state->memory;
  if(sqlite_state->api.open_db((char*)db_path.str, &sqlite_state->db) != SQLITE_OK)
  {
    result = 0;
    state->errors = DBError_Connexion;
  }
  return result;
}

internal B32
sqlite_close_db(StateDB *state)
{
  B32 result = 0;
  SQLiteState *sqlite_state = (SQLiteState *)state->memory;
  if(sqlite_state->api.close_db(sqlite_state->db) == SQLITE_OK)
  {
    sqlite_state->db = 0;
    result = 1;
  }
  return result;
} 

internal B32
sqlite_init(Arena *arena, String8 lib_path, String8 db_path, StateDB *state)
{
  B32 result = 0;
  SQLiteState *sqlite_state = (SQLiteState *)state->memory;
  state->lib = os_library_open(arena, lib_path);
  if(os_handle_match(state->lib, os_handle_zero()))
  {
    state->errors = DBError_Library;
    printf("Couldn't find the SQLite3 dll");
    return 0;
  }
  SQLiteAPI api = {0};

  api.open_db             = (sqlite_open *)         os_library_load_proc(arena, state->lib, str8_lit("sqlite3_open"));
  api.close_db            = (sqlite_close *)        os_library_load_proc(arena, state->lib, str8_lit("sqlite3_close"));
  api.exec_query          = (sqlite_exec *)         os_library_load_proc(arena, state->lib, str8_lit("sqlite3_exec"));
  api.prepare_query       = (sqlite_prepare *)      os_library_load_proc(arena, state->lib, str8_lit("sqlite3_prepare"));
  api.step_query          = (sqlite_step *)         os_library_load_proc(arena, state->lib, str8_lit("sqlite3_step"));
  api.column_type         = (sqlite_column_type *)  os_library_load_proc(arena, state->lib, str8_lit("sqlite3_column_type"));
  api.column_value        = (sqlite_col_value *)    os_library_load_proc(arena, state->lib, str8_lit("sqlite3_column_value"));
  api.bind_int            = (sqlite_bind_int *)     os_library_load_proc(arena, state->lib, str8_lit("sqlite3_bind_int"));
  api.column_count        = (sqlite_col_count *)    os_library_load_proc(arena, state->lib, str8_lit("sqlite3_column_count"));  
  api.column_int          = (sqlite_column_int *)   os_library_load_proc(arena, state->lib, str8_lit("sqlite3_column_int"));
  api.column_text         = (sqlite_column_text *)  os_library_load_proc(arena, state->lib, str8_lit("sqlite3_column_text"));
  api.column_blob         = (sqlite_column_blob *)  os_library_load_proc(arena, state->lib, str8_lit("sqlite3_column_blob"));
  api.column_text16       = (sqlite_column_text16 *)os_library_load_proc(arena, state->lib, str8_lit("sqlite3_column_text16"));
  api.column_double       = (sqlite_column_double *)os_library_load_proc(arena, state->lib, str8_lit("sqlite3_column_double"));
  api.column_bytes        = (sqlite_column_bytes *) os_library_load_proc(arena, state->lib, str8_lit("sqlite3_column_bytes"));
  api.column_blob         = (sqlite_column_blob *) os_library_load_proc(arena, state->lib, str8_lit("sqlite3_column_bytes"));
  api.finalize_statement  = (sqlite_finalize *)     os_library_load_proc(arena, state->lib, str8_lit("sqlite3_finalize"));
  api.col_name            = (sqlite_col_name *)     os_library_load_proc(arena, state->lib, str8_lit("sqlite3_column_name"));
  
  if(!api.close_db || !api.open_db || !api.exec_query || 
     !api.prepare_query || !api.step_query || !api.column_type ||
     !api.column_value || !api.bind_int || !api.column_count ||
     !api.column_int || !api.column_text || !api.column_blob||
     !api.column_text16 || !api.column_double || !api.column_bytes || 
     !api.finalize_statement)
  {
    state->errors = DBError_Library;
    printf("Error: sqlite library. Some exported methods was not found.");
    return 0;
  }
  sqlite_state->api = api;
  if(sqlite_open_db(db_path, state))
  {
    result = 1;
  }
  return result;
} 

internal B32
sqlite_prepare_query(String8 query, StateDB *state)
{
  B32 result = 0;  
  SQLiteState *sqlite_state = (SQLiteState *)state->memory;
  if(sqlite_state->api.prepare_query(sqlite_state->db, (char *)query.str, -1, &sqlite_state->statement, NULL) ==  SQLITE_OK) 
  {
    sqlite_state->api.bind_int (sqlite_state->statement, 1, 2);
    result = 1;
  }
  return result;
}

internal U8 
sqlite_column_count(StateDB *state)
{
  U8 result = 0;
  SQLiteState *sqlite_state = (SQLiteState *)state->memory;
  result = (U8)sqlite_state->api.column_count(sqlite_state->statement);
  return result;  
}

internal String8
sqlite_get_column_name(StateDB *state, int col_idx)
{
  String8 result = {0};
  SQLiteState *sqlite_state = (SQLiteState *)state->memory;
  const char * column_name = sqlite_state->api.col_name(sqlite_state->statement, col_idx);
  result = str8_cstring((char *)column_name);
  return result;
}

internal StepFlags
sqlite_step_query(StateDB *state)
{
  StepFlags result = StepFlag_Null;
  SQLiteState *sqlite_state = (SQLiteState *)state->memory;
  int step_flag = sqlite_state->api.step_query(sqlite_state->statement);
  if(step_flag == SQLITE_DONE)
  {
    result = StepFlag_Done;
  }
  else if(step_flag == SQLITE_ROW)
  {
    result = StepFlag_Row;
  }
  else if(step_flag == SQLITE_ERROR)
  {
    result = StepFlag_Error;
  }
  return result;
}

internal RawData *
sqlite_column_value(Arena *arena, int column_idx, StateDB *state, 
                    ColumRawMeaningTable *meaning_table, 
                    DBRawDataTransformers *transformers)
{
  RawData *result = push_array(arena, RawData, 1);
  SQLiteState *sqlite_state = (SQLiteState *)state->memory;
  
  ColumnDataDB col = {0};
  int colType      = sqlite_state->api.column_type(sqlite_state->statement, column_idx);
  col.col_name     = push_str8_copy(arena, sqlite_get_column_name(state, column_idx));
  col.raw.meaning  = database_raw_meaning_from_col_name(col.col_name, meaning_table);
  
  switch(colType) 
  {
    case SQLITE_INTEGER:
    {
      col.type = ColumnType_Text;
      sqlite_state->api.column_int(sqlite_state->statement, column_idx);
      col.raw.buffer.data = data;
      col.raw.buffer.size = 32/8;
      col.raw.type = RawDataType_int;
      col.raw.transformer = transformers->transformer_int;
    } break;

    case SQLITE_FLOAT:
    {
      col.type = ColumnType_Float;
      F64 data = sqlite_state->api.column_int(sqlite_state->statement, column_idx);
      col.raw.buffer.data = (F64*)&data;
      col.raw.buffer.size = 64/8;
      col.raw.type = RawDataType_float;
      col.raw.transformer = transformers->transformer_float;
    } break;

    case SQLITE_TEXT:
    {
      col.type = ColumnType_Text;
      U8 *data = (U8*)sqlite_state->api.column_text(sqlite_state->statement, column_idx);
      
      col.raw.buffer.data = (void*)&str8(data, cstr8_length(data));
      col.raw.buffer.size = sqlite_state->api.sqlite_column_bytes(sqlite_state->statement, column_idx);
      col.raw.type = RawDataType_string;
      // col.raw.transformer = &transformers->raw_default_string_transformer;
    } break;  
    
    case SQLITE_NULL:
    {
      col.type = TextType_Unmeaningfull;
    } break;
    
    case SQLITE_BLOB:
    {
      col.type = ColumnType_Blob;
      col.raw.buffer.data = sqlite_state->api.column_blob(sqlite_state->statement, column_idx);
      col.raw.buffer.size = sqlite_state->api.sqlite_column_bytes(sqlite_state->statement, column_idx);
      //TODO: transformer
    } break;
  }
  
  return result;
}

internal B32
sqlite_finalize_statement(StateDB *state)
{
  B32 result = 1;
  SQLiteState *sqlite_state = (SQLiteState *)state->memory;
  if(sqlite_state->api.finalize_statement(sqlite_state->statement) != SQLITE_OK)
  {
    result = 0;
  }
  return result;
}