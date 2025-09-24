#include "drag_utils.h"

namespace DragUtils {

ClientCoordinates ExtractCoordinates(const Napi::CallbackInfo& info, size_t optionsIndex) {
  if (info.Length() <= optionsIndex || !info[optionsIndex].IsObject()) {
    return ClientCoordinates(); // No coordinates provided
  }

  auto options = info[optionsIndex].As<Napi::Object>();
  if (!options.Has("x") || !options.Has("y")) {
    return ClientCoordinates(); // Missing x or y property
  }

  auto xVal = options.Get("x");
  auto yVal = options.Get("y");
  if (!xVal.IsNumber() || !yVal.IsNumber()) {
    return ClientCoordinates(); // x or y is not a number
  }

  int x = (int)xVal.As<Napi::Number>().Int64Value();
  int y = (int)yVal.As<Napi::Number>().Int64Value();

  return ClientCoordinates(x, y);
}

bool ValidateBufferSize(Napi::Env env, const Napi::Buffer<char>& buffer, size_t minSize, const char* errorMessage) {
  if (buffer.Length() < minSize) {
    Napi::TypeError::New(env, errorMessage).ThrowAsJavaScriptException();
    return false;
  }
  return true;
}

void ThrowError(Napi::Env env, const char* message) {
  Napi::TypeError::New(env, message).ThrowAsJavaScriptException();
}

}