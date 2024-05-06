/*
  TODO: get rid of them
*/
internal B32       sqlite_init(Arena *arena, String8 lib_path, String8 db_path, StateDB *state);
internal B32       sqlite_close_db(StateDB *state);
internal B32       sqlite_prepare_query(String8 query, StateDB *state);
internal U8        sqlite_column_count(StateDB *state);
internal StepFlags sqlite_step_query(StateDB *state);
internal RawData*  sqlite_column_value(Arena *arena, int column_idx, StateDB *state,  ColumRawMeaningTable *meaning_table, DBRawDataTransformers *transformers);
internal B32       sqlite_finalize_statement(StateDB *state);

internal StateDB*
database_init(Arena *arena, TypeDB type, String8 lib_path, String8 db_path)
{
  StateDB *state = push_array(arena, StateDB, 1);
  state->db_type = type;
  
  B32 is_init = 0;
  if(state->db_type == TypeDB_SQLITE)
  {
     is_init = sqlite_init(arena, lib_path, db_path, state);
  }
  state->is_initialized = is_init;
  return state;
}

internal B32
database_close(StateDB *state)
{
  B32 result = 0;
  if(state->db_type == TypeDB_SQLITE)
  {
    if(sqlite_close_db(state))
    {
      result = 1;
    }
  }
  return result;
}

internal void
database_exec_push_raw_list(Arena *arena, String8 query, StateDB *state, 
                            RawDataList *out, ColumRawMeaningTable *col_meanings,
                            DBRawDataTransformers *transformers)
{
  if(state->db_type == TypeDB_SQLITE)
  {
    if(!sqlite_prepare_query(query, state))
    {
      state->errors = DBError_Query;
      return;
    };
    int column_count = sqlite_column_count(state);
        
    for(int step_flag = StepFlag_Null;
        step_flag != StepFlag_Done;
        step_flag = sqlite_step_query(state)) 
    {
      
      if(step_flag == StepFlag_Error) 
      {
        state->errors = DBError_Query;
         break;
      }
      else if(step_flag != StepFlag_Row)
      {
        continue;
      }
      
      RawData raw = {0};
      RawData *first = {0};
      RawData *last = {0};
      for(U8 column_idx = 0; column_idx <= column_count-1; ++column_idx)
      {
        RawData *raw_chunk = sqlite_column_value(arena, column_idx, state, col_meanings, transformers);
        AppendLast(first, last, last->next, raw_chunk);
      }
      
      raw_data_list_push(arena, out, *first);
    }
    
    if(!sqlite_finalize_statement(state))
    {
      state->errors = DBError_Query;
    }
  }
}

internal void
database_print_error(StateDB *state)
{
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
}

internal RawMeaning
database_raw_meaning_from_col_name(String8 col_name, ColumRawMeaningTable *table)
{
  RawMeaning raw_meaning = {0};
  
  for(U64 idx = 0; table->count; ++idx)
  {
    ColumRawMeaning *col_raw_meaning = &table->col_meanings[idx];
    if(str8_match(col_name, col_raw_meaning->col_name, StringMatchFlag_CaseInsensitive))
    {
      raw_meaning = col_raw_meaning->meaning;
    }
  }
  
  return raw_meaning;
}
