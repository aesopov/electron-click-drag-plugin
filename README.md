# electron-click-drag-plugin

A native Node.js addon that enables elements to be **both draggable and clickable** in Electron / Overwolf Electron.  
Useful when `-webkit-app-region: drag` disables click events, but you still want OS-level window dragging.

## Installation

```bash
npm install node-addon-api electron --save
```

## Build

```bash
npx node-gyp configure 
npx node-gyp build
```


This will compile the native C/C++ addon into ./build/Release/<addon_name>.node.

Usage
In your Electron preload or main process:

```js
const { app, BrowserWindow, ipcMain } = require('electron');

// Load the compiled native addon
const dragAddon = require('./build/Release/drag.node');

app.whenReady().then(() => {
  const win = new BrowserWindow({
    width: 800,
    height: 800,
    frame: false, // Important: no OS window frame
    webPreferences: {
      nodeIntegration: false,
      contextIsolation: false, // only for test/dev
      devTools: true
    },
  });

  win.loadFile('index.html');

  win.webContents.openDevTools();

  ipcMain.on('start-drag', (event) => {
    try {
      const hwndBuffer = win.getNativeWindowHandle();
      dragAddon.startDrag(hwndBuffer);
    } catch (error) {
      console.log(error)
    }
  });
});
```

## Tested On
✅ Windows 10 / 11 (Overwolf Electron)

✅ Standard Electron (>= v22)