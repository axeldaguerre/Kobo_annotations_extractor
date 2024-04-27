#include"core/os_core.c"

// #if OS_WINDOWS
# include"core/win32/os_core_win32.c"
# include "gfx/win32/os_gfx_win32.c"
// #elif OS_LINUX
// # include "core/linux/os_core_linux.c"
// # include "gfx/linux/os_gfx_linux.c"
// #endif