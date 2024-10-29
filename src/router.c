#ifndef OGMA_ROUTER_H

#define OGMA_ROUTER_H

#include "ogma.h"

Router *alloc_router() {
    Router *router = OGMA_MALLOC(sizeof(Router));
    return router;
}
Router* init_router(Router *router) {
    router->end = false;
    for (size_t i = 0; i < DEFAULT_ROUTER_SIZE; i++)
        router->childs[i] = NULL;
    return router;
}
void router_add(Router *router, const char *key, void (*callback)()) {
    Router *current = router;
    size_t length = strlen(key);
    for (size_t level = 0; level < length; level++) {
        size_t index = key[level] - 'a';
        if (!current->childs[index]) {
            current->childs[index] = malloc(sizeof(Router));
            init_router(current->childs[index]);
        }
        current = current->childs[index];
    }
    current->end = true;
    current->callback = callback;
}

Router *router_search(Router *router, const char *key) {
    Router *current = router;
    size_t length = strlen(key);
    for (size_t level = 0; level < length; level++) {
        size_t index = key[level] - 'a';
        if(!current->childs[index])
            return NULL;
        current = current->childs[index];
    }
    if (current->callback != NULL && current->end)
        return current;
    else
        return NULL;
}
void close_router(Router *router) {
    if (router == NULL)
        return;

    for (size_t i = 0; i < DEFAULT_ROUTER_SIZE; i++) {
        if (router->childs[i] != NULL) {
            close_router(router->childs[i]);
            OGMA_FREE(router->childs[i]);
            OGMA_FREE(router->callback);
        }
    }
    OGMA_FREE(router);
    logger("Router successfully closed.");
}

#endif // OGMA_ROUTER_H
