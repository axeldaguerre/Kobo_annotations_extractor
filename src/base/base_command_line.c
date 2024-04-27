
internal U64
cmd_line_hash_from_string(String8 string)
{
 U64 result = 5381;
 for(U64 i = 0; i < string.size; i += 1)
 {
  result = ((result << 5) + result) + string.str[i];
 }
 return result;
}

internal CmdLine
cmd_line_from_string_list(Arena *arena, String8List strings)
{
 Temp scratch = temp_begin(arena);
 
 //- rjf: set up
 CmdLine cmdln = {0};
 cmdln.slots_count = 64;
 cmdln.slots = push_array(arena, CmdLineOptSlot, cmdln.slots_count);
 
 //- rjf: separate strings by whitespace & collapse quotes - emulate shell
 String8List separated_strings = {0};
 for(String8Node *n = strings.first; n != 0; n = n->next)
 {
  String8List strings_from_this_n = {0};
  U64 start_idx = 0;
  B32 quoted = 0;
  B32 seeking_non_ws = 0;
  for(U64 idx = 0; idx <= n->string.size; idx += 1)
  {
   if(seeking_non_ws && idx < n->string.size && !char_is_whitespace(n->string.str[idx]))
   {
    seeking_non_ws = 0;
    start_idx = idx;
   }
   if(!seeking_non_ws && (idx == n->string.size || n->string.str[idx] == ' ' || n->string.str[idx] == '"'))
   {
    String8 string = str8_substr(n->string, r1u64(start_idx, idx));
    str8_list_push(scratch.arena, &strings_from_this_n, string);
    start_idx = idx+1;
    if(n->string.str[idx] == ' ')
    {
     seeking_non_ws = 1;
    }
   }
   if(idx < n->string.size && n->string.str[idx] == '"')
   {
    quoted ^= 1;
   }
  }
  str8_list_concat_in_place(&separated_strings, &strings_from_this_n);
 }
 
 //- rjf: parse list of strings
 CmdLineOptNode *active_opt_node = 0;
 for(String8Node *n = separated_strings.first; n != 0; n = n->next)
 {
  String8 piece     = str8_skip_chop_whitespace(n->string);
  B32 double_dash   = str8_match(str8_prefix(piece, 2), str8_lit("--"), 0);
  B32 single_dash   = str8_match(str8_prefix(piece, 1), str8_lit("-"), 0);
  B32 value_for_opt = (active_opt_node != 0);
  
  //- rjf: dispatch to rule
  if(value_for_opt == 0 && (double_dash || single_dash))
  {
   goto new_option;
  }
  if(value_for_opt)
  {
   goto value_strings;
  }
  
  //- rjf: parse this string as a new option
  new_option:;
  {
   U64 dash_prefix_size = !!single_dash + !!double_dash;
   String8 opt_part = str8_skip(piece, dash_prefix_size);
   U64 colon_pos = str8_find_needle(opt_part, 0, str8_lit(":"), 0);
   U64 equal_pos = str8_find_needle(opt_part, 0, str8_lit("="), 0);
   U64 value_specifier_pos = min(colon_pos, equal_pos);
   String8 opt_name = str8_prefix(opt_part, value_specifier_pos);
   String8 first_part_of_opt_value = str8_skip(opt_part, value_specifier_pos+1);
   U64 hash = cmd_line_hash_from_string(opt_name);
   U64 slot_idx = hash%cmdln.slots_count;
   CmdLineOptSlot *slot = &cmdln.slots[slot_idx];
   CmdLineOptNode *node = push_array(arena, CmdLineOptNode, 1);
   SLLQueuePush(slot->first, slot->last, node);
   node->name = opt_name;
   if(first_part_of_opt_value.size != 0)
   {
    str8_list_push(arena, &node->values, first_part_of_opt_value);
   }
   if(value_specifier_pos < opt_part.size &&
      (first_part_of_opt_value.size == 0 ||
       str8_match(str8_suffix(first_part_of_opt_value, 1), str8_lit(","), 0)))
   {
    active_opt_node = node;
   }
  }
  goto end_node;
  
  //- rjf: parse this string as containing values
  value_strings:;
  {
   U8 splits[] = { ',' };
   String8List value_parts = str8_split(arena, piece, splits, ArrayCount(splits), 0);
   str8_list_concat_in_place(&active_opt_node->values, &value_parts);
   if(!str8_match(str8_suffix(piece, 1), str8_lit(","), 0))
   {
    active_opt_node = 0;
   }
  }
  goto end_node;
  
  //- rjf: interpret this string as an input string independent from options
  {
   str8_list_push(arena, &cmdln.inputs, piece);
  }
  goto end_node;
  
  end_node:;
 }
 
 //- rjf: produce flattened value strings
 {
  for(U64 slot_idx = 0; slot_idx < cmdln.slots_count; slot_idx += 1)
  {
   for(CmdLineOptNode *n = cmdln.slots[slot_idx].first; n != 0; n = n->next)
   {
    StringJoin join = {str8_lit(""), str8_lit(","), str8_lit("")};
    n->value = str8_list_join(arena, &n->values, &join);
   }
  }
 }
 
 temp_end(scratch);
 return cmdln;
}

internal String8List
cmd_line_opt_strings(CmdLine *cmdln, String8 name)
{
 String8List result = {0};
 {
  U64 hash = cmd_line_hash_from_string(name);
  U64 slot_idx = hash%cmdln->slots_count;
  CmdLineOptSlot *slot = &cmdln->slots[slot_idx];
  CmdLineOptNode *node = 0;
  for(CmdLineOptNode *n = slot->first; n != 0; n = n->next)
  {
   if(str8_match(n->name, name, 0))
   {
    node = n;
    break;
   }
  }
  if(node != 0)
  {
   result = node->values;
  }
 }
 return result;
}

internal String8
cmd_line_opt_string(CmdLine *cmdln, String8 name)
{
 String8 result = {0};
 {
  U64 hash = cmd_line_hash_from_string(name);
  U64 slot_idx = hash%cmdln->slots_count;
  CmdLineOptSlot *slot = &cmdln->slots[slot_idx];
  CmdLineOptNode *node = 0;
  for(CmdLineOptNode *n = slot->first; n != 0; n = n->next)
  {
   if(str8_match(n->name, name, 0))
   {
    node = n;
    break;
   }
  }
  if(node != 0)
  {
   result = node->value;
  }
 }
 return result;
}

internal B32
cmd_line_opt_b32(CmdLine *cmdln, String8 name)
{
 B32 result = 0;
 {
  U64 hash = cmd_line_hash_from_string(name);
  U64 slot_idx = hash%cmdln->slots_count;
  CmdLineOptSlot *slot = &cmdln->slots[slot_idx];
  CmdLineOptNode *node = 0;
  for(CmdLineOptNode *n = slot->first; n != 0; n = n->next)
  {
   if(str8_match(n->name, name, 0))
   {
    node = n;
    break;
   }
  }
  if(node != 0)
  {
   result = (node->value.size == 0 ||
             str8_match(node->value, str8_lit("true"), StringMatchFlag_CaseInsensitive) ||
             str8_match(node->value, str8_lit("1"), StringMatchFlag_CaseInsensitive));
  }
 }
 return result;
}

internal F64
cmd_line_opt_f64(CmdLine *cmdln, String8 name)
{
 F64 result = 0;
 {
  String8 string = cmd_line_opt_string(cmdln, name);
  result = f64_from_str8(string);
 }
 return result;
}

internal S64
cmd_line_opt_s64(CmdLine *cmdln, String8 name)
{
 S64 result = 0;
 {
  String8 string = cmd_line_opt_string(cmdln, name);
  result = s64_from_str8(string, 10);
 }
 return result;
}
