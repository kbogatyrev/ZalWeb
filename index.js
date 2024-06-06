


const toml = require('toml');
const fs = require('fs');
var express = require('express');
var cors = require('cors');
var app = express();

app.use(cors({ origin: true, credentials: true }));

const requestHandlers = require('./requestHandlers');
const server = require('./server');
const router = require('./router');

const settings_path = process.argv[2]
if (settings_path == undefined) {
    console.log("*** ERROR: missing config file");
    process.exit(1);
}

const config = toml.parse(fs.readFileSync(settings_path, 'utf-8'));
server.init(config);

var handle = {};
handle["/query"] = requestHandlers.wordQuery;
handle["/forms"] = requestHandlers.paradigmQuery;
handle["/parse"] = requestHandlers.wordParse;
handle["/text"] = requestHandlers.textQuery;
console.log(handle);
server.start(router.route, handle);
