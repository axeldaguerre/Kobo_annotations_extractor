internal B32
sqlite_open_db(String8 db_path, SQLiteState *state)
{
  B32 result = 1;
  
  if(state->api.open_db((char*)db_path.str, &state->db) != SQLITE_OK)
  {
    result = 0;
    state->errors = DBError_Connexion;
  }
  return result;
}

internal B32
sqlite_close_db(SQLiteState *state)
{
  B32 result = 0;
  
  if(state->api.close_db(state->db) == SQLITE_OK)
  {
    state->db = 0;
    result = 1;
  }
  return result;
} 

internal B32
kobo_sqlite_init(Arena *arena, String8 lib_path, String8 db_path, SQLiteState *state)
{
  B32 result = 0;
  
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
  api.column_int64        = (sqlite_column_int64 *) os_library_load_proc(arena, state->lib, str8_lit("sqlite3_column_int64"));
  api.column_text         = (sqlite_column_text *)  os_library_load_proc(arena, state->lib, str8_lit("sqlite3_column_text"));
  api.column_blob         = (sqlite_column_blob *)  os_library_load_proc(arena, state->lib, str8_lit("sqlite3_column_blob"));
  api.column_text16       = (sqlite_column_text16 *)os_library_load_proc(arena, state->lib, str8_lit("sqlite3_column_text16"));
  api.column_double       = (sqlite_column_double *)os_library_load_proc(arena, state->lib, str8_lit("sqlite3_column_double"));
  api.column_bytes        = (sqlite_column_bytes *) os_library_load_proc(arena, state->lib, str8_lit("sqlite3_column_bytes"));
  api.column_blob         = (sqlite_column_blob *)  os_library_load_proc(arena, state->lib, str8_lit("sqlite3_column_bytes"));
  api.finalize_statement  = (sqlite_finalize *)     os_library_load_proc(arena, state->lib, str8_lit("sqlite3_finalize"));
  api.col_name            = (sqlite_col_name *)     os_library_load_proc(arena, state->lib, str8_lit("sqlite3_column_name"));
  api.errmsg              = (sqlite_errmsg  *)      os_library_load_proc(arena, state->lib, str8_lit("sqlite3_errmsg"));
  api.errcode             = (sqlite_errcode  *)     os_library_load_proc(arena, state->lib, str8_lit("sqlite3_errcode"));
  
  if(!api.close_db || !api.open_db || !api.exec_query || 
     !api.prepare_query || !api.step_query || !api.column_type ||
     !api.column_value || !api.bind_int || !api.column_count ||
     !api.column_int || !api.column_int64 || !api.column_text || !api.column_blob||
     !api.column_text16 || !api.column_double || !api.column_bytes || 
     !api.finalize_statement || !api.errmsg || !api.errcode)
  {
    state->errors = DBError_Library;
    printf("Error: sqlite library. Some exported methods was not found.");
    return 0;
  }
  state->api = api;
  if(sqlite_open_db(db_path, state))
  {
    result = 1;
  }
  return result;
} 

internal B32
sqlite_prepare_query(String8 query, SQLiteState *state)
{
  B32 result = 0;  
  
  if(state->api.prepare_query(state->db, (char *)query.str, -1, &state->statement, NULL) ==  SQLITE_OK) 
  {
    state->api.bind_int (state->statement, 1, 2);
    result = 1;
  }
  return result;
}

internal U8 
sqlite_column_count(SQLiteState *state)
{
  U8 result = 0;
  
  result = (U8)state->api.column_count(state->statement);
  return result;  
}

internal String8
sqlite_get_column_name(SQLiteState *state, int col_idx)
{
  String8 result = {0};
  
  const char * column_name = state->api.col_name(state->statement, col_idx);
  result = str8_cstring((char *)column_name);
  return result;
}

internal B32
sqlite_finalize_statement(SQLiteState *state)
{
  B32 result = 1;
  if(state->api.finalize_statement(state->statement) != SQLITE_OK)
  {
    result = 0;
  }
  return result;
}

internal SQLiteState*
kobo_db_init(Arena *arena, String8 lib_path, String8 db_path)
{
  SQLiteState *state = push_array(arena, SQLiteState, 1);
  state->is_initialized = kobo_sqlite_init(arena, lib_path, db_path, state);
  return state;
}

internal B32
kobo_db_close(SQLiteState *state)
{
  B32 result = 0;
  if(sqlite_close_db(state))
  {
    result = 1;
  }
  
  return result;
}

internal B32 
kobo_db_state_is_running(SQLiteState *state)
{
  B32 result = state->errors == DBError_Null && 
              state->step_code != SQLITE_ERROR &&
              state->step_code != SQLITE_DONE;
  return result;
}

/*
    NOTE: When you fetch the db you will have node not related, because entries are sorted by
          book titles, when the book title is different than the previous we can push the node 
          on a list and be certain we won't have annotation coming from a book title node we pushed previously
  */ 
internal RawDataNode *
kobo_db_execute_create_raw(Arena *arena, String8 query, SQLiteState *state)
{  
  B32 print_col_names = 1;
  RawDataNode *first_book_n = {0};
  RawDataNode *last_book_n = {0};
  while(kobo_db_state_is_running(state))
  {
    state->step_code = state->api.step_query(state->statement);
    if(state->step_code == SQLITE_ROW)
    { 
      // NOTE: We don't know the order in which the columns will be fetched, we need to keep track of the nodes for later link each together
      RawDataNode *annotation_n = {0}; 
      RawDataNode *first_details_n = {0};
      RawDataNode *last_details_n = {0};
      for(U8 col_idx = 0; 
          col_idx <= state->col_count; 
          ++col_idx)
      {
        int colType = state->api.column_type(state->statement, col_idx);
        String8 col_name = sqlite_get_column_name(state, col_idx);
        RawDataNode *col_n = push_array(arena, RawDataNode, 1);
        if(print_col_names)
        {
          col_n->raw.data = push_str8_cat(arena, col_name, str8_lit(": "));
        }
        
        switch(colType) 
        {
          case SQLITE_INTEGER:
          case SQLITE_FLOAT:
          {
            S64 value = state->api.column_int64(state->statement, col_idx);
            col_n->raw.data = push_str8_cat(arena, col_n->raw.data, str8_from_s64(arena, value, 10, 3, '.'));
          } break;    
          case SQLITE_TEXT:
          {
            U8 *data = (U8*)state->api.column_text(state->statement, col_idx);
            col_n->raw.data = push_str8_cat(arena, col_n->raw.data, str8(data, cstr8_length(data)));
          } break; 
          
          case SQLITE_NULL:
          {} break;
          case SQLITE_BLOB:
          {} break;
        }
        
        if(str8_match(col_name, str8_lit("Annotation"), 0))
        {
          // NOTE: Annotation's title column (written by Kobo user)
          col_n->raw.meaning.strenght = RawStrenght_High;
          col_n->raw.meaning.semantic_flags = RawSemantic_Summary;
          annotation_n = col_n;
        }
        else if(str8_match(col_name, str8_lit("VolumeID"), 0))
        {
          // NOTE: Book title column
          col_n->raw.meaning.strenght = RawStrenght_Highest;
          col_n->raw.meaning.semantic_flags = RawSemantic_Summary;
          if(!first_book_n || !str8_match(last_book_n->raw.data, col_n->raw.data, 0))
          {
            SLLQueuePush(first_book_n, last_book_n, col_n);
          }
        }
        else
        {
          // NOTE: Details columns
          col_n->raw.meaning.semantic_flags = RawSemantic_Details;
          col_n->raw.meaning.strenght = RawStrenght_AboveMedium;
          col_n->parent = annotation_n;
          SLLQueuePush(first_details_n, last_details_n, col_n);
        }
      }
      
      if(!last_book_n || !annotation_n || !first_details_n)
      {
        state->errors |= DBError_Query;
        return 0;
      }
      // NOTE: annotation column can be fetch before first details node or book title node
      annotation_n->first = first_details_n; 
      annotation_n->parent = last_book_n;
      
      DLLPushBack_NPZ(&raw_node_g_nil, last_book_n->first, last_book_n->last, annotation_n, next, prev);
    }
    else if(state->step_code == SQLITE_ERROR) 
    {
      state->errors |= DBError_Query;
      break;
    }
    else if(state->step_code == SQLITE_DONE) 
    {
      break;
    }
  }
  
  if(!sqlite_finalize_statement(state))
  {
    state->errors = DBError_Query;
  }
  
  return first_book_n;
}

internal void
kobo_db_print_error(SQLiteState *state)
{
  if(state->errors == DBError_Null) return;
  printf("Errors: \n");
  if(!(state->errors & DBError_Null))
  {
    printf("DB Error(s): \n");
  } 
  if(!(state->errors & DBError_Query))
  {
    printf("Query failed\n" );
  }
  if(!(state->errors & DBError_Connexion))
  {
    printf("Connexion failed\n" );
  }
  if(!(state->errors & DBError_Library))
  {
    printf("Can't find Database DLL \n");
  }
  if(!(state->errors & DBError_Library))
  {
    printf("Can't find Database DLL \n");
  }
}
