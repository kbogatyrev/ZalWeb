//    https://www.zalizniak.com/query?l=баба

function route(handle, pathname, params, response) {
    console.log("About to route request for " + pathname);

    const searchParams = new URLSearchParams(params);

    const search = searchParams.get('l');
    console.log(searchParams.get('l'));

    if (typeof handle[pathname] === 'function') {
        handle[pathname](search, response);
    }
    else {
        console.log("No request handler found for " + pathname);
        response.writeHead(404, { "Content-Type": "text/plain" });
        response.write("404 Not found");
        response.end();
    }
}
exports.route = route;
