#include "drag.h"

#if !defined(_WIN32) && !defined(__APPLE__)
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <unistd.h>
#include <cstring>

Napi::Value StartDrag(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  if (info.Length() < 1) {
    Napi::TypeError::New(env, "Expected window handle as first argument").ThrowAsJavaScriptException();
    return env.Null();
  }

  Display* display = XOpenDisplay(NULL);
  if (!display) {
    Napi::TypeError::New(env, "Cannot open X display").ThrowAsJavaScriptException();
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
    Napi::TypeError::New(env, "Expected Buffer or Number for window handle").ThrowAsJavaScriptException();
    return env.Null();
  }

  // Optional client coordinates (x, y). If not provided, query current pointer.
  bool haveClient = (info.Length() >= 3 && info[1].IsNumber() && info[2].IsNumber());
  Window root, child;
  int root_x, root_y, win_x, win_y;
  unsigned int mask;
  if (!haveClient) {
    XQueryPointer(display, window, &root, &child, &root_x, &root_y, &win_x, &win_y, &mask);
  } else {
    // Use provided client coordinates, convert to root coordinates
    win_x = (int)info[1].As<Napi::Number>().Int64Value();
    win_y = (int)info[2].As<Napi::Number>().Int64Value();

    // Convert client coordinates to root coordinates
    XWindowAttributes attrs;
    if (XGetWindowAttributes(display, window, &attrs) == 0) {
      XCloseDisplay(display);
      Napi::TypeError::New(env, "Failed to get window attributes").ThrowAsJavaScriptException();
      return env.Null();
    }
    root_x = attrs.x + win_x;
    root_y = attrs.y + win_y;
  }

  Atom moveAtom = XInternAtom(display, "_NET_WM_MOVERESIZE", False);
  if (moveAtom == None) {
    XCloseDisplay(display);
    Napi::TypeError::New(env, "Cannot find _NET_WM_MOVERESIZE atom").ThrowAsJavaScriptException();
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
