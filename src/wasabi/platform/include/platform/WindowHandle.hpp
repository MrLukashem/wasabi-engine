
#pragma once


#ifdef _WIN32
#include <windows.h>

using WindowHandle = HWND;
#endif // _WIN32

#ifdef  __linux__
struct wl_display;

struct WindowHandle {
    wl_display* wlDisplay;
};
#endif

#ifdef __APPLE__
using CAMetalLayer = void;

struct WindowHandle {
    CAMetalLayer* caMetalLayer;
};
#endif
