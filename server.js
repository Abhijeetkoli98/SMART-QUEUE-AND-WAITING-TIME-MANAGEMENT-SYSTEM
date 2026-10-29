const http = require('http');
const fs = require('fs');
const path = require('path');

const PORT = 3000;
const PUBLIC_DIR = path.join(__dirname, 'web_gui');

const MIME_TYPES = {
  '.html': 'text/html; charset=utf-8',
  '.css': 'text/css',
  '.js': 'text/javascript',
  '.png': 'image/png',
  '.jpg': 'image/jpeg',
  '.svg': 'image/svg+xml',
  '.json': 'application/json',
  '.ico': 'image/x-icon'
};

const server = http.createServer((req, res) => {
  let reqUrl = req.url.split('?')[0];

  // API endpoint to bridge C++ data files to the Web UI
  if (reqUrl === '/api/data') {
    const dataDir = path.join(__dirname, 'data');
    let nextTokenId = 101;
    let avgServiceTime = 5;
    let currentlyServing = null;
    let waitingQueue = [];
    let completedRecords = [];

    // 1. Read token_config.txt
    try {
      const cfg = fs.readFileSync(path.join(dataDir, 'token_config.txt'), 'utf8').trim().split('\n');
      if (cfg[0]) nextTokenId = parseInt(cfg[0], 10) || 101;
      if (cfg[1]) avgServiceTime = parseInt(cfg[1], 10) || 5;
    } catch (e) {}

    // 2. Read active_queue.txt
    try {
      const queueLines = fs.readFileSync(path.join(dataDir, 'active_queue.txt'), 'utf8').split('\n');
      queueLines.forEach(line => {
        line = line.trim();
        if (!line) return;
        const parts = line.split('|');
        if (parts[0] === 'SERVING' && parts.length >= 5) {
          currentlyServing = {
            id: parseInt(parts[1], 10),
            name: parts[2],
            service: parts[3],
            arrival: parts[4]
          };
        } else if (parts[0] === 'WAITING' && parts.length >= 5) {
          waitingQueue.push({
            id: parseInt(parts[1], 10),
            name: parts[2],
            service: parts[3],
            arrival: parts[4]
          });
        }
      });
    } catch (e) {}

    // 3. Read completed_records.csv
    try {
      const csvLines = fs.readFileSync(path.join(dataDir, 'completed_records.csv'), 'utf8').split('\n');
      for (let i = 1; i < csvLines.length; i++) {
        const line = csvLines[i].trim();
        if (!line) continue;
        const parts = line.split(',');
        if (parts.length >= 5) {
          const rawId = parts[0].replace('T-', '').trim();
          const name = parts[1].replace(/"/g, '').trim();
          const service = parts[2].replace(/"/g, '').trim();
          const arrival = parts[3].trim();
          const completion = parts[4].trim();
          completedRecords.push({
            id: parseInt(rawId, 10),
            name: name,
            service: service,
            arrival: arrival,
            completion: completion
          });
        }
      }
    } catch (e) {}

    res.writeHead(200, {
      'Content-Type': 'application/json',
      'Access-Control-Allow-Origin': '*'
    });
    res.end(JSON.stringify({
      nextTokenId,
      avgServiceTime,
      currentlyServing,
      waitingQueue,
      completedRecords
    }));
    return;
  }

  let safePath = path.normalize(reqUrl === '/' ? '/index.html' : reqUrl);
  let filePath = path.join(PUBLIC_DIR, safePath);

  // Prevent directory traversal attacks
  if (!filePath.startsWith(PUBLIC_DIR)) {
    res.writeHead(403, { 'Content-Type': 'text/plain' });
    res.end('403 Forbidden');
    return;
  }

  fs.stat(filePath, (err, stats) => {
    if (err || !stats.isFile()) {
      res.writeHead(404, { 'Content-Type': 'text/plain' });
      res.end('404 Not Found');
      return;
    }

    const ext = path.extname(filePath).toLowerCase();
    const contentType = MIME_TYPES[ext] || 'application/octet-stream';

    res.writeHead(200, {
      'Content-Type': contentType,
      'Cache-Control': 'no-cache',
      'Access-Control-Allow-Origin': '*'
    });

    const stream = fs.createReadStream(filePath);
    stream.pipe(res);
  });
});

server.listen(PORT, () => {
  console.log(`[Smart Queue Web Server] Hosting on http://localhost:${PORT}/`);
});
