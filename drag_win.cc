#include "drag.h"
#include "drag_utils.h"

#ifdef _WIN32
#include <windows.h>

Napi::Value StartDrag(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  if (info.Length() < 1 || !info[0].IsBuffer()) {
    DragUtils::ThrowError(env, "Expected buffer as first argument");
    return env.Null();
  }

  auto buf = info[0].As<Napi::Buffer<char>>();
  if (!DragUtils::ValidateBufferSize(env, buf, sizeof(void*), "Invalid native handle buffer")) {
    return env.Null();
  }

  HWND hwnd = *reinterpret_cast<HWND*>(buf.Data());

  // Extract optional client coordinates from options object {x, y}
  auto coords = DragUtils::ExtractCoordinates(info, 1);
  int clientX = coords.hasCoordinates ? coords.x : -1;
  int clientY = coords.hasCoordinates ? coords.y : -1;

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
