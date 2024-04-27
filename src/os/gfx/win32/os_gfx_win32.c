internal void
os_graphical_message(Arena *arena, B32 error, String8 title, String8 message)
{
  String16 title16 = str16_from_str8(arena, title);
  String16 message16 = str16_from_str8(arena, message);
  MessageBoxW(0, (WCHAR *)message16.str, (WCHAR *)title16.str, MB_OK|(!!error*MB_ICONERROR));
}