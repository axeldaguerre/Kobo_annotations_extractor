function void EntryPoint(CmdLine *cmdln);

#if BUILD_CONSOLE_OUTPUT
int main(int argument_count, char **arguments)
{
 os_w32_hinstance = GetModuleHandle(0);
 BaseMainThreadEntry(EntryPoint, (U64)argument_count, arguments);
 return 0;
}
#else
int WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR lp_cmd_line, int n_show_cmd)
{
 os_w32_hinstance = instance;
 BaseMainThreadEntry(EntryPoint, (U64)__argc, __argv);
 return 0;
}
#endif
