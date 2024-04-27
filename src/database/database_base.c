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
database_exec_push_list(Arena *arena, String8 query, StateDB *state, 
                        TextualTable *table, EntryDataDBList *out)
{
  if(state->db_type == TypeDB_SQLITE)
  {
    if(!sqlite_prepare_query(query, state))
    {
      state->errors = DBError_Query;
      return;
    };
    int column_count = sqlite_column_count(state);
    
    // TODO: better looping, a while or do while. 
    //       currently first step_flags == StepFlag_Null
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
      
      EntryDataDB *entry     = push_array(arena, EntryDataDB, 1);
      ColumnData  *first_col = {0};
      ColumnData  *last_col  = {0};
      for(U8 column_idx = 0; column_idx <= column_count-1; ++column_idx)
      {
        ColumnData *column = sqlite_column_value(arena, column_idx, state, table);
        if(column->textual_type != TextType_Null)
        {
          // TODO: MACRO or procedure (search to see others)
          if(last_col)
          {
            last_col->next_sibbling = column;
          }
          else
          {
            first_col = column;
          }
          last_col = column;
        }        
      }
      entry->data = *first_col;
      database_entry_list_push(arena, out, entry);
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
  if(!state->errors & DBError_Null)
  {
    printf("DB Error(s): \n");
  } 
  if((!state->errors & DBError_Query))
  {
    printf("Query failed\n" );
  }
  if((!state->errors & DBError_Connexion))
  {
    printf("Connexion failed\n" );
  }
  if((!state->errors & DBError_Library))
  {
    printf("Can't find Database DLL \n");
  }
}

internal void
database_append_node(Arena *arena, TextualList *list, TextualNode *in)
{
  B32 match = 0;
  // AssertAlways(in->textual.type == TextType_Title);
  for(TextualNode *node = list->first; 
      node != 0; 
      node = node->next)
  {
    // AssertAlways(node->textual.type == TextType_Title);
    if(str8_match(node->textual.text, in->textual.text, 0))
    {
      node->next = in;
      match = 1;
      break;
    }
  }
  if(!match)
  {
    list->last = in;
  }
}


internal EntryDataDBNode *
database_try_retrieve_entry_node(EntryDataDBList *list, ColumnData data)
{
  EntryDataDBNode *result = {0};
  for(EntryDataDBNode *node = list->first; 
      node != 0; 
      node = node->next)
  { 
    for(ColumnData *col = &node->entry.data;
        col != 0; 
        col = col->next_sibbling)
    {
      if(str8_match(col->name, data.name, StringMatchFlag_CaseInsensitive) &&
        str8_match(col->value, data.value, StringMatchFlag_CaseInsensitive))
      {
        result = node;        
        break;
      }
    }
    if(result)
    {
      break;
    }
  }
  return result;
}
  
internal EntryDataDBList *
database_entries_group_by(Arena *arena, EntryDataDBList *list, ColumnData by)
{
  EntryDataDBList *result = push_array(arena, EntryDataDBList, 1);
  
  for(EntryDataDBNode *node = list->first; 
      node != 0; 
      node = node->next)
  {
    for(ColumnData *data = &node->entry.data;
        data != 0; 
        data = data->next_sibbling)
    {
      if(str8_match(data->name, by.name, StringMatchFlag_CaseInsensitive))
      {
        EntryDataDBNode *existing_node = database_try_retrieve_entry_node(result, *data);
        if(existing_node)
        {
          for(EntryDataDB *entry = &existing_node->entry; 
              entry !=0; 
              entry = entry->next_sibbling)
          {
            if(!entry->next_sibbling)
            {
              entry->next_sibbling = &node->entry;
              break;
            }
          }
        }
        else if(!existing_node && str8_match(by.name, data->name, StringMatchFlag_CaseInsensitive))
        {
          database_entry_list_push(arena, result, &node->entry);
        }
      }
    }
  }
  return result;
}
internal Textual *
database_entries_to_textual(Arena *arena, EntryDataDB *entries)
{
  Textual *result = push_array(arena, Textual, 1);  
  /*
    TODO: pointer is not null when there is no data left, we seems to set it somewhere in the 
          code even when the entry is null
  */
  if(entries->data.type != ColumnType_Null)
  { 
    Textual *first = {0};  
    Textual *last = {0};
    
    for(ColumnData *data = &entries->data; 
        data != 0; 
        data = data->next_sibbling)
    {      
      Textual *textual = push_array(arena, Textual, 1);  
      textual->text = push_str8_copy(arena, data->value);
      textual->type = data->textual_type;
      
      // TODO: MACRO or procedure (search to see others)      
      if(last)
      {
        last->first_sub_textual = textual;
      }
      else
      {
        first = textual; 
      }
      last = textual;
    }
    
    if(entries->next_sibbling)
    {
      first->next_sibbling = database_entries_to_textual(arena, entries->next_sibbling);
    }
    result = first;
  }  
  
  return result;
}