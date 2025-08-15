const path = require('path');
const os = require('os');

const platform = os.platform();
const arch = os.arch();

let binaryPath;

if (platform === 'win32' && arch === 'x64') {
  binaryPath = path.join(__dirname, 'build/Release/win32-x64/drag.node');
} else if (platform === 'linux' && arch === 'x64') {
  binaryPath = path.join(__dirname, 'build/Release/linux-x64/drag.node');
} else {
  throw new Error(`Unsupported platform: ${platform}-${arch}`);
}

module.exports = require(binaryPath);
