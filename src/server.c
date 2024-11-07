#ifndef OGMA_HTTP_SERVER_H

#define OGMA_HTTP_SERVER_H

#include "ogma.h"

HTTP_Server *init_server(int port) {
    HTTP_Server *http_server = (HTTP_Server*)OGMA_MALLOC(sizeof(HTTP_Server));
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
    return http_server;
}

struct arg_struct {
    HTTP_Server *server;
    Router *router;
};

void *run_server(void *arguments) {
    struct arg_struct *args = arguments;
    HTTP_Server *http_server = args->server;
    Router *router = args->router;
    char client_msg[4096] = "";
    int client_socket = accept(http_server->socket, NULL, NULL);
    
    if (client_socket == -1) {
        perror("accept failed");
        return NULL;
    }

    ssize_t bytes_read = read(client_socket, client_msg, sizeof(client_msg) - 1);
    if (bytes_read == -1) {
        perror("read failed");
        close(client_socket);
        return NULL;
    }

    if (bytes_read == 0) {
        printf("Client closed the connection.\n");
        close(client_socket);
        return NULL;
    }

    client_msg[bytes_read] = '\0';

    Request *request = init_request(client_msg);
    print_request(request);

    Response *response = init_response();
    response->socket = client_socket;

    RouterNode *route = search_router(router, request->url, request->method);
    if (!route) {
        logger("Not in routes: %s", request->url);
        close(client_socket);
        return NULL;
    }

    route->callback(request, response);

    free_response(response);
    free_request(request);
    return NULL;
}

void loop_server(HTTP_Server *server, Router *router) {
    fd_set readfds;
    int max_fd = server->socket;
    
    while (1) {
        FD_ZERO(&readfds);
        FD_SET(0, &readfds);
        FD_SET(server->socket, &readfds);

        int activity = select(max_fd + 1, &readfds, NULL, NULL, NULL);
        if (activity < 0) {
            perror("select error");
            break;
        }

        if (FD_ISSET(0, &readfds)) {
            char input = getchar();
            if (input == 'q') {
                break;
            }
        }

        if (FD_ISSET(server->socket, &readfds)) {
            pthread_t thread_id;
            struct arg_struct args;
            args.router = router;
            args.server = server;
            pthread_create(&thread_id, NULL, run_server, (void *)&args);
            pthread_join(thread_id, NULL);
        }
    }
}

void free_server(HTTP_Server *http_server) {
    if (http_server == NULL)
        return;

    if (http_server->socket != -1) {
        close(http_server->socket);
    }

    free(http_server);
    logger("HTTP Server successfully closed.");
}
#endif // !OGMA_HTTP_SERVER_H