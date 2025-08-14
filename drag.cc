#include <napi.h>
#include <windows.h>
#include <fstream>

Napi::Value StartDrag(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  if (!info[0].IsBuffer()) {
    Napi::TypeError::New(env, "Expected buffer as first argument").ThrowAsJavaScriptException();
    return env.Null();
  }

  HWND hwnd = *reinterpret_cast<HWND*>(info[0].As<Napi::Buffer<char>>().Data());

  POINT startPos;
  GetCursorPos(&startPos);
  int startX = startPos.x;
  int startY = startPos.y;

  ScreenToClient(hwnd, &startPos);
  LPARAM lParam = MAKELPARAM(startPos.x, startPos.y);

  ReleaseCapture();
  SendMessage(hwnd, WM_SYSCOMMAND, SC_MOVE | HTCAPTION, 0);

  POINT endPos;
  GetCursorPos(&endPos);
  int endX = endPos.x;
  int endY = endPos.y;

  if (endX == startX && endY == startY) {
    SendMessage(hwnd, WM_LBUTTONUP, 0, lParam);
  }

  return env.Null();
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set("startDrag", Napi::Function::New(env, StartDrag));
  return exports;
}

NODE_API_MODULE(drag, Init)