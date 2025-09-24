#include "drag.h"

#ifdef _WIN32
#include <windows.h>

Napi::Value StartDrag(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  if (info.Length() < 1 || !info[0].IsBuffer()) {
    Napi::TypeError::New(env, "Expected buffer as first argument").ThrowAsJavaScriptException();
    return env.Null();
  }

  HWND hwnd = *reinterpret_cast<HWND*>(info[0].As<Napi::Buffer<char>>().Data());

  // Optional client coordinates (x, y) in window client space
  int clientX = -1, clientY = -1;
  if (info.Length() >= 3 && info[1].IsNumber() && info[2].IsNumber()) {
    clientX = (int)info[1].As<Napi::Number>().Int64Value();
    clientY = (int)info[2].As<Napi::Number>().Int64Value();
  }

  int screenStartX = 0, screenStartY = 0;
  POINT tmp;
  if (clientX >= 0 && clientY >= 0) {
    // Convert provided client coords to screen for comparison
    tmp.x = clientX;
    tmp.y = clientY;
    ClientToScreen(hwnd, &tmp);
    screenStartX = tmp.x;
    screenStartY = tmp.y;
  } else {
    // Capture current cursor screen position and derive client coords for lParam
    GetCursorPos(&tmp);
    screenStartX = tmp.x;
    screenStartY = tmp.y;
    ScreenToClient(hwnd, &tmp);
    clientX = tmp.x;
    clientY = tmp.y;
  }
  LPARAM lParam = MAKELPARAM(clientX, clientY);

  ReleaseCapture();
  SendMessage(hwnd, WM_SYSCOMMAND, SC_MOVE | HTCAPTION, 0);

  POINT endPos;
  GetCursorPos(&endPos);
  int endX = endPos.x;
  int endY = endPos.y;

  if (endX == screenStartX && endY == screenStartY) {
    SendMessage(hwnd, WM_LBUTTONUP, 0, lParam);
  }

  return env.Null();
}
#endif
