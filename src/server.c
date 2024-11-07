#ifndef OGMA_HTTP_SERVER_H
#define OGMA_HTTP_SERVER_H

#include "ogma.h"

HTTP_Server *init_server(int port) {
    HTTP_Server *http_server = (HTTP_Server*)OGMA_MALLOC(sizeof(HTTP_Server));
    http_server->port = port;
    int server_socket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    assert(server_socket < 0, Error, "Socket could not be created");

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = INADDR_ANY;

    int opt = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    assert(bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address)) < 0, Error, "Socket could not bind");
    assert(listen(server_socket, 10) < 0, Error, "Socket could not listen");

    http_server->socket = server_socket;
    logger("HTTP Server initialized, PORT: %d", port);
    return http_server;
}

typedef struct {
    int client_socket;
    Router *router;
} Task;

typedef struct {
    Task tasks[MAX_EVENTS];
    int task_count;
    pthread_mutex_t lock;
    pthread_cond_t cond;
} ThreadPool;

ThreadPool thread_pool;

void *handle_client() {
    while (1) {
        Task task;
        
        pthread_mutex_lock(&thread_pool.lock);
        while (thread_pool.task_count == 0) {
            pthread_cond_wait(&thread_pool.cond, &thread_pool.lock);
        }
        
        task = thread_pool.tasks[--thread_pool.task_count];
        pthread_mutex_unlock(&thread_pool.lock);

        char client_msg[4096] = "";
        ssize_t bytes_read = read(task.client_socket, client_msg, sizeof(client_msg) - 1);
        if (bytes_read <= 0) {
            close(task.client_socket);
            continue;
        }
        
        client_msg[bytes_read] = '\0';
        Request *request = init_request(client_msg);
        Response *response = init_response();
        response->socket = task.client_socket;

        RouterNode *route = search_router(task.router, request->url, request->method);
        if (!route) {
            close(task.client_socket);
            continue;
        }
        
        route->callback(request, response);
        
        free_response(response);
        free_request(request);
        close(task.client_socket);
    }
    return NULL;
}

void init_thread_pool(Router *router) {
    thread_pool.task_count = 0;
    pthread_mutex_init(&thread_pool.lock, NULL);
    pthread_cond_init(&thread_pool.cond, NULL);

    for (int i = 0; i < THREAD_POOL_SIZE; i++) {
        pthread_t thread;
        pthread_create(&thread, NULL, handle_client, router);
        pthread_detach(thread);
    }
}

void add_task(int client_socket, Router *router) {
    pthread_mutex_lock(&thread_pool.lock);

    if (thread_pool.task_count < MAX_EVENTS) {
        Task task;
        task.client_socket = client_socket;
        task.router = router;
        thread_pool.tasks[thread_pool.task_count++] = task;
        pthread_cond_signal(&thread_pool.cond);
    }

    pthread_mutex_unlock(&thread_pool.lock);
}

void loop_server(HTTP_Server *server, Router *router) {
    int epoll_fd = epoll_create1(0);
    struct epoll_event event, events[MAX_EVENTS];

    event.events = EPOLLIN;
    event.data.fd = server->socket;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server->socket, &event);

    init_thread_pool(router);

    while (1) {
        int n = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);

        for (int i = 0; i < n; i++) {
            if (events[i].data.fd == server->socket) {
                int client_socket;
                while ((client_socket = accept(server->socket, NULL, NULL)) != -1) {
                    int flags = fcntl(client_socket, F_GETFL, 0);
                    fcntl(client_socket, F_SETFL, flags | O_NONBLOCK);

                    event.events = EPOLLIN | EPOLLET;
                    event.data.fd = client_socket;
                    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_socket, &event);
                }
            } else {
                int client_socket = events[i].data.fd;
                epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_socket, NULL);
                add_task(client_socket, router);
            }
        }
    }
    close(epoll_fd);
}

void free_server(HTTP_Server *http_server) {
    if (http_server == NULL) return;
    if (http_server->socket != -1) close(http_server->socket);
    free(http_server);
    logger("HTTP Server successfully closed.");
}

#endif // !OGMA_HTTP_SERVER_H
