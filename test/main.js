const { app, BrowserWindow, ipcMain } = require('electron');

// Load the compiled native addon
const dragAddon = require('../build/Release/drag.node');

app.whenReady().then(() => {
  const win = new BrowserWindow({
    width: 800,
    height: 800,
    frame: false, // Important: no OS window frame
    webPreferences: {
      nodeIntegration: true,
      contextIsolation: false, // only for test/dev
      devTools: true
    },
  });

  win.loadFile('renderer.html');

  win.webContents.openDevTools();

  ipcMain.on('start-drag', (event) => {
    const hwndBuffer = win.getNativeWindowHandle();
    // On Linux, extract the window ID from the buffer (first 4 bytes, little-endian)
    const windowId = process.platform === 'linux'
      ? hwndBuffer.readUInt32LE(0)
      : hwndBuffer;
    dragAddon.startDrag(windowId);
  });
});
