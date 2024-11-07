#ifndef OGMA_ROUTER_H

#define OGMA_ROUTER_H

#include "ogma.h"

unsigned int hash_router(Router *router, char *path, char *method) {
    unsigned int hash_value = 0;

    for (size_t i = 0; i < strlen(path); i++) {
        hash_value += path[i];
    }
    for (size_t i = 0; i < strlen(method); i++) {
        hash_value += method[i];
    }
    return hash_value % router->size;
}

Router *init_router(unsigned int size) {
    Router *router = (Router *)OGMA_MALLOC(sizeof(Router));
    router->size = size;
    router->nodes = OGMA_MALLOC(size * sizeof(RouterNode *));
    for (size_t i = 0; i < size; i++) {
        router->nodes[i] = NULL;
    }
    return router;
}

RouterNode *init_router_node(char *path, char *method, void (*callback)(Request *request, Response *response)) {
    RouterNode *node = (RouterNode *)OGMA_MALLOC(sizeof(RouterNode));
    node->path = (char *)OGMA_MALLOC((strlen(path) + 1) * sizeof(char));
    node->method = (char *)OGMA_MALLOC((strlen(method) + 1) * sizeof(char));
    strcpy(node->path, path);
    strcpy(node->method, method);
    node->callback = callback;
    return node;
}

void insert_router(Router *router, char *path, char *method, void (*callback)(Request *request, Response *response)) {
    RouterNode *node = init_router_node(path, method, callback);
    unsigned int index = hash_router(router, node->path, node->method);
    if (router->nodes[index] == NULL) {
        router->nodes[index] = node;
    } else {
        OGMA_FREE(router->nodes[index]->path);
        OGMA_FREE(router->nodes[index]->method);
        OGMA_FREE(router->nodes[index]);
        router->nodes[index] = node;
    }
}

RouterNode *search_router(Router *router, char *path, char *method) {
    unsigned int index = hash_router(router, path, method);
    if (router->nodes[index] != NULL && strcmp(router->nodes[index]->path, path) == 0 && strcmp(router->nodes[index]->method, method) == 0) {
        return router->nodes[index];
    }
    return NULL;
}

void delete_router(Router *router, char *path, char *method) {
    unsigned int index = hash_router(router, path, method);
    if (router->nodes[index] != NULL && strcmp(router->nodes[index]->path, path) == 0 && strcmp(router->nodes[index]->method, method) == 0) {
        OGMA_FREE(router->nodes[index]->path);
        OGMA_FREE(router->nodes[index]->method);
        OGMA_FREE(router->nodes[index]);
        router->nodes[index] = NULL;
    }
}

void free_router(Router *router) {
    for (size_t i = 0; i < router->size; i++) {
        if (router->nodes[i] != NULL) {
            OGMA_FREE(router->nodes[i]->path);
            OGMA_FREE(router->nodes[i]->method);
            OGMA_FREE(router->nodes[i]);
        }
    }
    OGMA_FREE(router->nodes);
    OGMA_FREE(router);
}

void print_router(Router *router) {
    for (size_t i = 0; i < router->size; i++) {
        if (router->nodes[i] == NULL) {
            printf("\t%li\t----\n", i);
        } else {
            printf("\t%li\tMethod: %s, Path: %s\n", i, router->nodes[i]->method, router->nodes[i]->path);
        }
    }
}

#endif // OGMA_ROUTER_H
