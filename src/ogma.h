#ifndef OGMA_H

#define OGMA_H

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <mongoc/mongoc.h>

#ifndef OGMA_MALLOC
#define OGMA_MALLOC malloc
#endif // !OGMA_MALLOC

#ifndef OGMA_REALLOC
#define OGMA_REALLOC realloc
#endif // !OGMA_REALLOC

#ifndef OGMA_FREE
#define OGMA_FREE free
#endif // !OGMA_FREE

typedef enum {
    Warning,
    Error
} assert_level;



typedef struct {
    char *userAgent;
} Header;

typedef struct {
    char *url;
    char *method;
    Header header;
} Request;

typedef struct {
    int socket;
} Response;

#define DEFAULT_ROUTER_SIZE 43 // 'a' - 'z'

typedef struct Router Router;

struct Router {
    Router* childs[DEFAULT_ROUTER_SIZE];
    void (*callback)(Request request, Response response);
    bool end;
};

typedef struct {
    int socket;
    int port;
} HTTP_Server;

typedef struct {
    mongoc_database_t *database;
} Model;

void logger(const char *message, ...);
void assert(int condition, assert_level level, const char *message, ...);

Request init_request(char message[4096]);
void print_request(Request request);

void *response_sendFile(Response response, char * fileName);

Router *alloc_router();
Router* init_router(Router *router);
void router_add(Router *router, const char *key, void (*callback)());
Router *router_search(Router *router, const char *key);
void close_router(Router *router);

HTTP_Server *alloc_server();
void init_server(HTTP_Server *http_server, int port);
void *run_server(HTTP_Server *http_server, Router *router);
void close_server(HTTP_Server *http_server);

mongoc_database_t *init_db(const char *url, const char *dbName);
void close_db();

#endif // !OGMA_H