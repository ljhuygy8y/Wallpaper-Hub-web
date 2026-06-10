const fs = require('fs');
const p = require('path').join(__dirname, '..', 'app.js');
let c = fs.readFileSync(p, 'utf8');
c = c.replace(/<motion class="pin-overlay">/g, '<div class="pin-overlay">');
c = c.replace(/\s*`\.replace\(\/<motion class="pin-overlay">[\s\S]*?<\/div>'\);/g, '\n    `;');
fs.writeFileSync(p, c);
console.log('fixed');
