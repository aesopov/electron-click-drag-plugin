const { app, BrowserWindow, ipcMain } = require('electron');

// Load the addon via package entry (handles platform paths)
const dragAddon = require('..');

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

  ipcMain.on('start-drag', (event, options) => {
    const hwndBuffer = win.getNativeWindowHandle();
    // Pass buffer directly on all platforms now
    if (options && typeof options.x === 'number' && typeof options.y === 'number') {
      // Use provided coordinates
      dragAddon.startDrag(hwndBuffer, options);
    } else {
      // Use current mouse position
      dragAddon.startDrag(hwndBuffer);
    }
  });
});
