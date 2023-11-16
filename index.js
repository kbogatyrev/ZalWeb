var server = require('./server');
var router = require('./router');
var requestHandlers = require("./requestHandlers");
var handle = {};
handle["/query"] = requestHandlers.wordQuery;
handle["/forms"] = requestHandlers.paradigmQuery;
handle["/parse"] = requestHandlers.wordParse;
server.start(router.route, handle);
