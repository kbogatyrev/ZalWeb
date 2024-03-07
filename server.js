const { URL } = require('url');

Object.defineProperty(exports, "__esModule", { value: true });

const http = require("https");
const fs = require("fs");

const httpsOptions = {
    cert: fs.readFileSync("/etc/letsencrypt/live/bogatyrev.org/cert.pem"),
    ca: fs.readFileSync("/etc/letsencrypt/live/bogatyrev.org/fullchain.pem"),
    key: fs.readFileSync("/etc/letsencrypt/live/bogatyrev.org/privkey.pem")
};

const url = require("url");

function start(route, handle) {
    function onRequest(request, response) {
        const fullUrl = request.protocol + '://' + request.headers.host + request.url;
        console.log(fullUrl);
        const parsedUrl = new URL(fullUrl);
        console.log(parsedUrl);
        route(handle, parsedUrl.pathname, parsedUrl.searchParams, response);
    }
    http.createServer(onRequest).listen(8088);
    console.log('Server has started.');
}
exports.start = start;
