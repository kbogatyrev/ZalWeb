//    https://www.zalizniak.com/query?word=баба

function route(handle, pathname, params, response) {
    console.log("About to route request for " + pathname);

    if (typeof handle[pathname] !== 'function') {
        console.log("No request handler found for " + pathname);
        response.writeHead(404, { "Content-Type": "text/plain" });
        response.write("404 Not found");
        response.end();
        return;
    }

    const searchParams = new URLSearchParams(params);

    switch(pathname) {
        case '/query':
            const word = searchParams.get('word');
            console.log('Word search: ' + word);
            handle[pathname](word, response);
            break;
        case '/forms':
            const inflectionId = searchParams.get('inflection-id');
            handle[pathname](inflectionId, response);
            break;
        case '/parse':
            break;
        case '/text':
            const start_idx = searchParams.get('start-idx');
            const size = searchParams.get('size');
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
