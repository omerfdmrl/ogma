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
#include <sys/select.h>
#include <sys/epoll.h>
#include <pthread.h>
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

typedef struct HashNode {
    char *key;
    char *value;
    struct HashNode *next;
} HashNode;

typedef struct HashTable {
    unsigned int size;
    HashNode **table;
} HashTable;

typedef struct {
    char *url;
    char *method;
    HashTable *query;
    HashTable *body;
    HashTable *header;
} Request;

typedef struct {
    int socket;
} Response;

typedef struct RouterNode {
    char *path;
    char *method;
    void (*callback)(Request *request, Response *response);
    void (**middlewares)(Request *request, Response *response, int *next);
    unsigned int middleware_count;
} RouterNode;

typedef struct {
    unsigned int size;
    RouterNode **nodes;
} Router;

typedef struct {
    int socket;
    int port;
} HTTP_Server;


#ifndef MAX_EVENTS
#define MAX_EVENTS 1
#endif // !MAX_EVENTS

#ifndef THREAD_POOL_SIZE
#define THREAD_POOL_SIZE 1
#endif // !THREAD_POOL_SIZE

typedef struct {
    mongoc_database_t *database;
} Model;

void logger(const char *message, ...);
void assert(int condition, assert_level level, const char *message, ...);

HashTable *init_hash_table(unsigned int size);
void insert_hash_table(HashTable *hash_table, const char *key, const char *value);
char *search_hash_table(HashTable *hash_table, const char *key);
void free_hash_table(HashTable *hash_table);
void print_hash_table(HashTable *hash_table);
char *json_hash_table(HashTable *hash_table);

Request *init_request(char *message);
void free_request(Request *request);
void print_request(Request *request);

Response *init_response();
void free_response(Response *response);
void *response_sendFile(Response *response, char * fileName);
void response_sendJson(Response *response, char *jsonData);
void response_sendText(Response *response, char *text);

Router *init_router(unsigned int size);
RouterNode *init_router_node(char *path, char *method, void (*callback)(Request *request, Response *response));
void insert_router(Router *router, RouterNode *node);
void insert_middleware(RouterNode *node, void (*middleware)(Request *request, Response *response, int *next));
RouterNode *search_router(Router *router, char *path, char *method);
void delete_router(Router *router, char *path, char *method);
void free_router(Router *router);
void print_router(Router *router);

HTTP_Server *init_server(int port);
void loop_server(HTTP_Server *server, Router *router);
void free_server(HTTP_Server *http_server);

mongoc_database_t *init_db(const char *url, const char *dbName);
void free_db();

#endif // !OGMA_H