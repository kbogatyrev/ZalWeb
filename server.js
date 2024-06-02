//const { URL } = require('url');
const url = require("url");
const fs = require('fs');


Object.defineProperty(exports, "__esModule", { value: true });
var server = undefined;
var allow_origin = undefined;
var httpsOptions = undefined;
var protocol = undefined;

function init(config) {
    protocol = config.settings.protocol;
    if (undefined == protocol || ("http" != protocol && "https" != protocol)) {
        console.log('*** WARNING: unable to read protocol, assuming HTTP.');
        server = require("http");
    } else {
        server = require(protocol);
    }

    if ("https" == protocol) {
        const cert_path = config.certificates.cert_path;
        const ca_path = config.certificates.ca_path;
        const private_key_path = config.certificates.private_key_path;

        if (cert_path == undefined || ca_path == undefined || private_key_path == undefined) {
            console.log('*** ERROR: unable to read certs, exiting.');
            process.exit(2);
        }
        httpsOptions = {
            cert: fs.readFileSync(cert_path),
            ca: fs.readFileSync(ca_path),
            key: fs.readFileSync(private_key_path)
        };
    }

    if (config.misc.allow_origin != undefined) {
        allow_origin = config.misc.allow_origin;
    }
}       //  init()

function start(route, handle) {
    function onRequest(request, response) {
        const fullUrl = request.protocol + '://' + request.headers.host + request.url;
        console.log(fullUrl);
        const parsedUrl = new URL(fullUrl);
        console.log(parsedUrl);
        if (allow_origin != undefined) {
  	        response.setHeader("Access-Control-Allow-Origin", allow_origin);
  	        response.setHeader("Access-Control-Allow-Methods", "POST, GET, PUT");
  	        response.setHeader("Access-Control-Allow-Headers", "Content-Type");
        }
        route(handle, parsedUrl.pathname, parsedUrl.searchParams, response);
    }
    if (protocol == "https") {
        server.createServer(httpsOptions, onRequest).listen(8088);
    } else {
        server.createServer(onRequest).listen(8088);
    }
    console.log('Server has started.');
}

exports.init = init;
exports.start = start;
