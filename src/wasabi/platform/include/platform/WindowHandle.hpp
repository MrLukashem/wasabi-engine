
#pragma once

#ifdef _WIN32
#include <windows.h>

using WindowHandle = HWND;
#endif // _WIN32

#ifdef  __linux__
struct wl_display;
struct wl_surface;

struct WindowHandle {
    wl_display* wlDisplay;
    wl_surface* wlSurface;
};
#endif

#ifdef __APPLE__

#if !defined(__OBJC__)
using CAMetalLayer = void;
struct NSWindow;

struct WindowHandle {
    CAMetalLayer* caMetalLayer;
};
#endif

#endif
