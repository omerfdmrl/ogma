#include "./src/ogma.h"

void callbackWelcome(Request *request, Response *response) {
    (void) request;
    response_sendFile(response, "test.html");
}

void callbackWelcomePOST(Request *request, Response *response) {
    (void) request;
    HashTable *responseJSON = init_hash_table(10);
    insert_hash_table(responseJSON, "foo", "bar");
    insert_hash_table(responseJSON, "hey", "yo!");
    response_sendJson(response, json_hash_table(responseJSON));
    free_hash_table(responseJSON);
}

void middlewareDate(Request *request, Response *response, int *next) {
    (void) request;
    (void) response;
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    printf("now: %d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    *next = 1;
}

int main() {
    HTTP_Server *server = init_server(3837);
    Router *router = init_router(10);

    RouterNode *welcomeNode = init_router_node("/welcome", "GET", callbackWelcome);
    insert_middleware(welcomeNode, middlewareDate);
    insert_router(router, welcomeNode);


    RouterNode *welcomeNodePOST = init_router_node("/welcome", "POST", callbackWelcomePOST);
    insert_middleware(welcomeNodePOST, middlewareDate);
    insert_router(router, welcomeNodePOST);

    loop_server(server, router);

    free_router(router);
    free_server(server);

    return 0;
}

