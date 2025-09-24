/**
 * Starts window drag.
 * @param windowHandle A platform-specific window handle (Buffer from getNativeWindowHandle()).
 * @param x Optional client X coordinate of mousedown within the window.
 * @param y Optional client Y coordinate of mousedown within the window.
 */
export function startDrag(windowHandle: Buffer | number, x?: number, y?: number): void;