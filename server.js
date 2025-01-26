const { log } = require("console");
const express = require("express");
const fs = require("fs");
const http = require("http");
const https = require("https");

const app = express();
let server;

//Object.defineProperty(exports, "__esModule", { value: true });
//var server = undefined;
let allow_origin = undefined;
let httpsOptions = undefined;
let protocol = undefined;
let port = undefined;
let db_path = undefined;
//var config = undefined;

function init(config) {
  config = config;
  protocol = config.settings.protocol;
  db_path = config.settings.db_path;
  if (undefined == protocol || ("http" != protocol && "https" != protocol)) {
    console.log("*** WARNING: unable to read protocol, assuming HTTP.");
  } else {
    console.log("Protocol: " + protocol + ".");
  }

  if ("https" == protocol) {
    const cert_path = config.certificates.cert_path;
    const ca_path = config.certificates.ca_path;
    const private_key_path = config.certificates.private_key_path;

    if (
      cert_path == undefined ||
      ca_path == undefined ||
      private_key_path == undefined
    ) {
      console.log("*** ERROR: unable to read certs, exiting.");
      process.exit(2);
    }
    httpsOptions = {
      cert: fs.readFileSync(cert_path),
      ca: fs.readFileSync(ca_path),
      key: fs.readFileSync(private_key_path),
    };
  }

  if (config.misc.allow_origin != undefined) {
    allow_origin = config.misc.allow_origin;
  }

  port = Number(config.settings.port);

  if (typeof port != "number" || isNaN(port)) {
    console.log("*** ERROR: unable to read certs, exiting.");
    process.exit(3);
  }
} //  init()

function start(route, handle) {
  if (protocol == "https") {
    console.log(httpsOptions);
    server = https.createServer(httpsOptions, app);
  } else {
    server = http.createServer(app);
  }

  server.listen(port);

  const requestHandler = require("./requestHandlers");
  requestHandler.setDbPath(db_path);

  console.log("Listening on port " + port + ".");

  app.use(express.json());

  app.get("/query", (req, res) => {
    res.set({
      "Access-Control-Allow-Origin": "*",
      "Access-Control-Allow-Methods": "GET",
      "Access-Control-Allow-Headers": "Content-Type, Authorization",
    });
    const fullUrl = req.protocol + "://" + req.headers.host + req.url;
    console.log("Full URL: " + fullUrl);
    const parsedUrl = new URL(fullUrl);
    console.log("Parsed URL: " + parsedUrl);
    console.log("Method: " + req.method);
    route(handle, parsedUrl.pathname, parsedUrl.searchParams, "", res);
  });

  app.get("/forms", (req, res) => {
    res.set({
      "Access-Control-Allow-Origin": "*",
      "Access-Control-Allow-Methods": "GET",
      "Access-Control-Allow-Headers": "Content-Type, Authorization",
    });
    const fullUrl = req.protocol + "://" + req.headers.host + req.url;
    console.log("Full URL: " + fullUrl);
    const parsedUrl = new URL(fullUrl);
    console.log("Parsed URL: " + parsedUrl);
    console.log("Method: " + req.method);
    route(handle, parsedUrl.pathname, parsedUrl.searchParams, "", res);
  });

  app.get("/parse", (req, res) => {
    res.set({
      "Access-Control-Allow-Origin": "*",
      "Access-Control-Allow-Methods": "GET",
      "Access-Control-Allow-Headers": "Content-Type, Authorization",
    });
    const fullUrl = req.protocol + "://" + req.headers.host + req.url;
    console.log("Full URL: " + fullUrl);
    const parsedUrl = new URL(fullUrl);
    console.log("Parsed URL: " + parsedUrl);
    console.log("Method: " + req.method);
    route(handle, parsedUrl.pathname, parsedUrl.searchParams, "", res);
  });

  app.get("/text", (req, res) => {
    res.set({
      "Access-Control-Allow-Origin": "*",
      "Access-Control-Allow-Methods": "GET",
      "Access-Control-Allow-Headers": "Content-Type, Authorization",
    });
    const fullUrl = req.protocol + "://" + req.headers.host + req.url;
    console.log("Full URL: " + fullUrl);
    const parsedUrl = new URL(fullUrl);
    console.log("Parsed URL: " + parsedUrl);
    console.log("Method: " + req.method);
    route(handle, parsedUrl.pathname, parsedUrl.searchParams, "", res);
  });

  app.options("/insertPropNoun", (req, res) => {
    res.set({
      "Access-Control-Allow-Origin": "*",
      "Access-Control-Allow-Methods": "POST, OPTIONS",
      "Access-Control-Allow-Headers": "Content-Type, Authorization",
    });
    res.status(204).send({});
    console.log("Endpoint: /insertPropNoun, method=" + req.method);
  });

  app.post("/insertPropNoun", (req, res) => {
    console.log("Endpoint: /insertPropNoun, method = " + req.method);
    console.log("Body = " + JSON.stringify(req.body));
  });
}

exports.init = init;
exports.start = start;
