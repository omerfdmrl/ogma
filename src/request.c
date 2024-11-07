#ifndef OGME_REQUEST_H

#define OGME_REQUEST_H

#include "ogma.h"

Request *init_request(char *message) {
    Request *request = OGMA_MALLOC(sizeof(Request));
    request->query = init_hash_table(10);
    request->body = init_hash_table(10);
    request->header = init_hash_table(10);

    char *line = message;
    char *next_line;
    int line_counter = 0;

    while ((next_line = strchr(line, '\n')) != NULL) {
        *next_line = '\0';

        if (line_counter == 0) {
            char *method = strtok(line, " ");
            char *url_with_query = strtok(NULL, " ");
            
            request->method = method;
            char *url = strtok(url_with_query, "?");
            request->url = url;

            char *query_token = strtok(NULL, "&");
            while (query_token != NULL) {
                char *key = strtok(query_token, "=");
                char *value = strtok(NULL, "");

                if (key && value) {
                    insert_hash_table(request->query, key, value);
                }

                query_token = strtok(NULL, "&");
            }
        } else {
            char *header_key = strtok(line, ": ");
            char *header_value = strtok(NULL, "");

            if (header_key && header_value) {
                insert_hash_table(request->header, header_key, header_value);
            }
        }

        line = next_line + 1;
        line_counter++;
    }

    return request;
}

void free_request(Request *request) {
    free_hash_table(request->body);
    free_hash_table(request->query);
    free_hash_table(request->header);
    OGMA_FREE(request);
}
void print_request(Request *request) {
    logger("Method: %s, Route: %s", request->method, request->url);
}

#endif // !OGME_REQUEST_H