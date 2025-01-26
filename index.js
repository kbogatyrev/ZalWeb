const toml = require("toml");
const fs = require("fs");
const cors = require("cors");
const favicon = require("serve-favicon");
const path = require("path");

//app.use(cors({ origin: true, credentials: true }));
//app.use("/favicon.ico", express.static("public/Zal.ico"));

const requestHandlers = require("./requestHandlers");
const server = require("./server");
const router = require("./router");

const settings_path = process.argv[2];
if (settings_path == undefined) {
  console.log("*** ERROR: missing config file");
  process.exit(1);
}

const config = toml.parse(fs.readFileSync(settings_path, "utf-8"));
server.init(config);

let handle = {};
handle["/query"] = requestHandlers.wordQuery;
handle["/forms"] = requestHandlers.paradigmQuery;
handle["/parse"] = requestHandlers.wordParse;
handle["/insertPropNoun"] = requestHandlers.insertPropNoun;
handle["/text"] = requestHandlers.textQuery;

console.log(handle);
server.start(router.route, handle);
