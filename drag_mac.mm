#include "drag.h"
#include "drag_utils.h"

#ifdef __APPLE__
#import <Cocoa/Cocoa.h>

// Electron's getNativeWindowHandle returns a Buffer containing the NSWindow*.
// We can call -performWindowDragWithEvent: if we can synthesize/obtain an NSEvent.
// Since we are invoked from a mousedown handler on the renderer side typically,
// the actual NSEvent already occurred. We can trigger drag based on current mouse
// position by creating a leftMouseDragged event and calling performWindowDragWithEvent.

static NSEvent* CreateDragEvent(NSWindow* window, bool hasPoint, NSPoint provided) {
  // Use provided window-local mouse location if given, otherwise current mouse location
  NSPoint local = hasPoint ? provided : [window mouseLocationOutsideOfEventStream];
  NSTimeInterval now = [NSDate timeIntervalSinceReferenceDate];
  NSEvent* event = [NSEvent mouseEventWithType:NSEventTypeLeftMouseDown
                                       location:local
                                  modifierFlags:0
                                      timestamp:now
                                   windowNumber:window.windowNumber
                                        context:nil
                                    eventNumber:0
                                     clickCount:1
                                       pressure:1.0];
  return event;
}

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

  // Electron may return either NSWindow* or NSView* depending on version.
  // Read as Objective-C id and resolve window accordingly.
  id obj = *reinterpret_cast<id*>(buf.Data());
  if (!obj) {
    DragUtils::ThrowError(env, "Null native pointer");
    return env.Null();
  }
  NSWindow* window = nil;
  if ([obj isKindOfClass:[NSWindow class]]) {
    window = (NSWindow*)obj;
  } else if ([obj isKindOfClass:[NSView class]]) {
    window = [(NSView*)obj window];
  }
  if (!window) {
    DragUtils::ThrowError(env, "Could not resolve NSWindow from handle");
    return env.Null();
  }

  // Extract optional client coordinates from options object {x, y}
  auto coords = DragUtils::ExtractCoordinates(info, 1);
  bool hasPoint = coords.hasCoordinates;
  NSPoint provided = NSZeroPoint;
  if (hasPoint) {
    provided.x = (CGFloat)coords.x;
    provided.y = (CGFloat)coords.y;
    // Convert from top-left origin to bottom-left origin (macOS coordinate system)
    NSView* cv = window.contentView;
    CGFloat height = cv ? cv.bounds.size.height : window.frame.size.height;
    provided.y = height - provided.y;
  }

  dispatch_async(dispatch_get_main_queue(), ^{
    @autoreleasepool {
      if ([window respondsToSelector:@selector(performWindowDragWithEvent:)]) {
        NSEvent* ev = CreateDragEvent(window, hasPoint, provided);
        [window performWindowDragWithEvent:ev];
      } else {
        // Fallback: begin a standard window drag via performDrag
        // Not typically needed on modern macOS.
      }
    }
  });

  return env.Null();
}
#endif
