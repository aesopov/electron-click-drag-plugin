#include "drag.h"
#include "drag_utils.h"

#if !defined(_WIN32) && !defined(__APPLE__)
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <unistd.h>
#include <cstring>

Napi::Value StartDrag(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  if (info.Length() < 1) {
    DragUtils::ThrowError(env, "Expected window handle as first argument");
    return env.Null();
  }

  Display* display = XOpenDisplay(NULL);
  if (!display) {
    DragUtils::ThrowError(env, "Cannot open X display");
    return env.Null();
  }

  // Accept either Buffer (preferred, from getNativeWindowHandle) or Number (legacy)
  Window window = 0;
  if (info[0].IsBuffer()) {
    auto buf = info[0].As<Napi::Buffer<char>>();
    // Window is typedef of unsigned long. Safely copy up to sizeof(unsigned long).
    unsigned long wid = 0;
    size_t n = std::min(sizeof(unsigned long), (size_t)buf.Length());
    std::memcpy(&wid, buf.Data(), n);
    window = (Window)wid;
  } else if (info[0].IsNumber()) {
    window = (Window)info[0].As<Napi::Number>().Int64Value();
  } else {
    XCloseDisplay(display);
    DragUtils::ThrowError(env, "Expected Buffer or Number for window handle");
    return env.Null();
  }

  // Extract optional client coordinates from options object {x, y}
  auto coords = DragUtils::ExtractCoordinates(info, 1);
  bool haveClient = coords.hasCoordinates;
  int win_x = coords.hasCoordinates ? coords.x : 0;
  int win_y = coords.hasCoordinates ? coords.y : 0;

  Window root, child;
  int root_x, root_y;
  unsigned int mask;
  if (!haveClient) {
    XQueryPointer(display, window, &root, &child, &root_x, &root_y, &win_x, &win_y, &mask);
  } else {
    // Use provided client coordinates, convert to root coordinates

    // Convert client coordinates to root coordinates
    XWindowAttributes attrs;
    if (XGetWindowAttributes(display, window, &attrs) == 0) {
      XCloseDisplay(display);
      DragUtils::ThrowError(env, "Failed to get window attributes");
      return env.Null();
    }
    root_x = attrs.x + win_x;
    root_y = attrs.y + win_y;
  }

  Atom moveAtom = XInternAtom(display, "_NET_WM_MOVERESIZE", False);
  if (moveAtom == None) {
    XCloseDisplay(display);
    DragUtils::ThrowError(env, "Cannot find _NET_WM_MOVERESIZE atom");
    return env.Null();
  }

  XEvent xev = {0};
  xev.xclient.type = ClientMessage;
  xev.xclient.window = window;
  xev.xclient.message_type = moveAtom;
  xev.xclient.format = 32;
  xev.xclient.data.l[0] = root_x;
  xev.xclient.data.l[1] = root_y;
  xev.xclient.data.l[2] = 8; // _NET_WM_MOVERESIZE_MOVE
  xev.xclient.data.l[3] = 1; // button 1
  xev.xclient.data.l[4] = 0;

  XSendEvent(display, DefaultRootWindow(display), False,
             SubstructureRedirectMask | SubstructureNotifyMask, &xev);

  XFlush(display);
  XCloseDisplay(display);

  return env.Null();
}
#endif
