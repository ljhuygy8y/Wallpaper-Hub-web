const fs = require('fs');
const path = require('path');
const p = path.join(__dirname, '..', 'app.js');
let c = fs.readFileSync(p, 'utf8');
const start = c.indexOf('  bindWallpapers() {');
const end = c.indexOf('  bindModal() {');
if (start < 0 || end < 0) {
  console.error('markers not found', start, end);
  process.exit(1);
}

const insert = fs.readFileSync(path.join(__dirname, 'wallpapers-methods.js'), 'utf8');
c = c.slice(0, start) + insert + c.slice(end);
c = c.replace(/await this\.loadWallpapers\(\)/g, 'await this.loadCollection()');
fs.writeFileSync(p, c);
console.log('patched app.js');
