//    https://www.zalizniak.com/query?word=баба

function route(handle, pathname, params, response) {
  console.log("About to route request for " + pathname);

  if (typeof handle[pathname] !== "function") {
    console.log("No request handler found for " + pathname);
    response.writeHead(404, { "Content-Type": "text/plain" });
    response.write("404 Not found");
    response.end();
    return;
  }

  const searchParams = new URLSearchParams(params);

  switch (pathname) {
    case "/query":
      //      try {
      const word = searchParams.get("word");
      console.log("Word search: " + word);
      handle[pathname](word, response);
      //      } catch (err) {
      //console.log("**************", err);
      //        response.writeHead(404, { "Content-Type": "text/json; charset=utf-8" });
      //        var json = "{}";
      //        response.write(json);
      //        response.end();
      //      }
      break;
    case "/forms":
      const inflectionId = parseInt(searchParams.get("inflection-id"), 10);
      handle[pathname](inflectionId, response);
      break;
    case "/parse":
      break;
    case "/text":
      const start_idx = parseInt(searchParams.get("start-idx"), 10);
      const size = parseInt(searchParams.get("size"), 10);
      handle[pathname](start_idx, size, response);
      break;
    default:
      console.log("No request handler found for " + pathname);
      response.writeHead(404, { "Content-Type": "text/plain" });
      response.write("404 Not found");
      response.end();
  }
}
exports.route = route;
