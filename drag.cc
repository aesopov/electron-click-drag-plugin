#include <napi.h>
#include <windows.h>

Napi::Value StartDrag(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  if (!info[0].IsBuffer()) {
    Napi::TypeError::New(env, "Expected buffer as first argument").ThrowAsJavaScriptException();
    return env.Null();
  }

  Napi::Buffer<char> hwndBuffer = info[0].As<Napi::Buffer<char>>();
  HWND hwnd = *reinterpret_cast<HWND*>(hwndBuffer.Data());

  ReleaseCapture();
  SendMessage(hwnd, WM_SYSCOMMAND, SC_MOVE | HTCAPTION, 0);

  return env.Null();
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set("startDrag", Napi::Function::New(env, StartDrag));
  return exports;
}

NODE_API_MODULE(drag, Init)
