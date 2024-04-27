/* date = June 2nd 2023 8:24 am */

#ifndef BASE_COMMAND_LINE_H
#define BASE_COMMAND_LINE_H

typedef struct CmdLineOptNode CmdLineOptNode;
struct CmdLineOptNode
{
 CmdLineOptNode *next;
 String8 name;
 String8List values;
 String8 value;
};

typedef struct CmdLineOptSlot CmdLineOptSlot;
struct CmdLineOptSlot
{
 CmdLineOptNode *first;
 CmdLineOptNode *last;
};

typedef struct CmdLine CmdLine;
struct CmdLine
{
 U64 slots_count;
 CmdLineOptSlot *slots;
 String8List inputs;
};

internal U64         cmd_line_hash_from_string(String8 string);
internal CmdLine     cmd_line_from_string_list(Arena *arena, String8List strings);
internal String8List cmd_line_opt_strings(CmdLine *cmdln, String8 name);
internal String8     cmd_line_opt_string(CmdLine *cmdln, String8 name);
internal B32         cmd_line_opt_b32(CmdLine *cmdln, String8 name);
internal F64         cmd_line_opt_f64(CmdLine *cmdln, String8 name);
internal S64         cmd_line_opt_s64(CmdLine *cmdln, String8 name);

#endif // BASE_COMMAND_LINE_H
