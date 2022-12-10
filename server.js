const { URL } = require('url');

Object.defineProperty(exports, "__esModule", { value: true });
const http = require("http");
const url = require("url");
//const port = process.env.port || 1337
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
//# sourceMappingURL=server.js.map