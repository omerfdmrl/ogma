#ifndef OGMA_HTTP_SERVER_H

#define OGMA_HTTP_SERVER_H

#include "ogma.h"

HTTP_Server *alloc_server() {
    HTTP_Server *http_server = (HTTP_Server*)OGMA_MALLOC(sizeof(HTTP_Server));
    return http_server;
}
void init_server(HTTP_Server *http_server, int port) {
    http_server->port = port;
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    assert(server_socket < 0, Error, "Socket could not created");

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = INADDR_ANY;

    int opt = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    assert(bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address)) < 0, Error, "Socket could not binding");

    assert(listen(server_socket, 10) < 0, Error, "Socket could not listening");
    http_server->socket = server_socket;
    logger("HTTP Server initialized, PORT: %d", port);
}
void *run_server(HTTP_Server *http_server, Router *router) {
    char client_msg[4096] = "";
    int client_socket = accept(http_server->socket, NULL, NULL);
    if(client_socket == -1) return NULL;

    read(client_socket, client_msg, 4095);

    Request request = init_request(client_msg);
    print_request(request);

    Router *route = router_search(router, request.url);
    if(!route) {
        logger("Not in routes: %s", request.url);
        close(client_socket);
        return NULL;
    }
    Response response;
    response.socket = client_socket;

    route->callback(request, response);
    close(client_socket);

    return NULL;
}
void close_server(HTTP_Server *http_server) {
    if (http_server == NULL)
        return;

    if (http_server->socket != -1) {
        close(http_server->socket);
    }

    free(http_server);
    logger("HTTP Server successfully closed.");
}
#endif // !OGMA_HTTP_SERVER_H