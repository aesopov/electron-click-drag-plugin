#pragma once
#include <napi.h>

namespace DragUtils {
  /**
   * Represents optional client coordinates extracted from options object
   */
  struct ClientCoordinates {
    bool hasCoordinates;
    int x;
    int y;

    ClientCoordinates() : hasCoordinates(false), x(-1), y(-1) {}
    ClientCoordinates(int x, int y) : hasCoordinates(true), x(x), y(y) {}
  };

  /**
   * Extracts client coordinates from options object {x, y}
   * @param info N-API callback info
   * @param optionsIndex Index of the options parameter (usually 1)
   * @return ClientCoordinates struct with coordinate data
   */
  ClientCoordinates ExtractCoordinates(const Napi::CallbackInfo& info, size_t optionsIndex = 1);

  /**
   * Validates that a buffer has minimum required size
   * @param env N-API environment
   * @param buffer The buffer to validate
   * @param minSize Minimum required size in bytes
   * @param errorMessage Error message to throw if validation fails
   * @return true if valid, false if error was thrown
   */
  bool ValidateBufferSize(Napi::Env env, const Napi::Buffer<char>& buffer, size_t minSize, const char* errorMessage);

  /**
   * Throws a standardized N-API error
   * @param env N-API environment
   * @param message Error message
   */
  void ThrowError(Napi::Env env, const char* message);
}