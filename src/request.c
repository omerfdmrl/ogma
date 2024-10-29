#ifndef OGME_REQUEST_H

#define OGME_REQUEST_H

#include "ogma.h"

Request init_request(char message[4096]) {
    Request request;

    char *client_http_header = strtok(message, "\n");
    char *header_token = strtok(client_http_header, " ");
    
    int header_parse_counter = 0;

    while (header_token != NULL) {

        switch (header_parse_counter) {
            case 0:
                request.method = header_token;
                break;
            case 1:
                request.url = header_token;
                break;
        }
        header_token = strtok(NULL, " ");
        header_parse_counter++;
    }
    return request;
}
void print_request(Request request) {
    logger("Method: %s, Route: %s", request.method, request.url);
}

#endif // !OGME_REQUEST_H