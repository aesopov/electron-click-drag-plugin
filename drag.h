#pragma once
#include <napi.h>

// Starts an OS-level window drag for the given native handle/identifier.
// Windows: expects a Buffer containing HWND bytes from getNativeWindowHandle().
// Linux (X11): accepts a Buffer from getNativeWindowHandle() (preferred) or legacy Number window id.
// macOS: expects a Buffer with NSWindow* from getNativeWindowHandle().
// Optionally accepts an options object with {x, y} as client coordinates where the mousedown occurred.
Napi::Value StartDrag(const Napi::CallbackInfo& info);

// Module initialization common to all platforms
Napi::Object Init(Napi::Env env, Napi::Object exports);
