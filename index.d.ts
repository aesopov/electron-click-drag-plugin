/**
 * Options for the startDrag function.
 */
export interface DragOptions {
  /** Client X coordinate of mousedown within the window. */
  x: number;
  /** Client Y coordinate of mousedown within the window. */
  y: number;
}

/**
 * Starts window drag.
 * @param windowHandle A platform-specific window handle (Buffer from getNativeWindowHandle()).
 * @param options Optional coordinates object {x, y} specifying where the mousedown occurred.
 */
export function startDrag(windowHandle: Buffer | number, options?: DragOptions): void;