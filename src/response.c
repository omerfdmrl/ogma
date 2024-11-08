#ifndef OGME_RESPONSE_H

#define OGME_RESPONSE_H

#include "ogma.h"

Response *init_response() {
	Response *response = OGMA_MALLOC(sizeof(Response));
	return response;
}

void free_response(Response *response) {
	int closed = close(response->socket);
    if (closed < 0) {
        perror("close");
    }
	OGMA_FREE(response);
}

void *response_sendFile(Response *response, char *fileName) {
    FILE *file = fopen(fileName, "rb");

    if (file == NULL) {
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long fsize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *response_data = OGMA_MALLOC(fsize);

    fread(response_data, 1, fsize, file);
    fclose(file);

    char http_header[1024];
    snprintf(http_header, sizeof(http_header), "HTTP/1.1 200 OK\r\nContent-Length: %ld\r\n\r\n", fsize);

    char *final_response = OGMA_MALLOC(strlen(http_header) + fsize);
    strcpy(final_response, http_header);
    memcpy(final_response + strlen(http_header), response_data, fsize);

    send(response->socket, final_response, strlen(http_header) + fsize, 0);

    OGMA_FREE(response_data);
    OGMA_FREE(final_response);

    return NULL;
}

void response_sendJson(Response *response, char *jsonData) {
    size_t jsonSize = strlen(jsonData);

    char http_header[1024];
    snprintf(http_header, sizeof(http_header), "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: %zu\r\n\r\n", jsonSize);

    char *final_response = OGMA_MALLOC(strlen(http_header) + jsonSize);
    strcpy(final_response, http_header);
    strcpy(final_response + strlen(http_header), jsonData);

    send(response->socket, final_response, strlen(http_header) + jsonSize, 0);

    OGMA_FREE(final_response);
}

void response_sendText(Response *response, char *text) {
    size_t textSize = strlen(text);

    char http_header[1024];
    snprintf(http_header, sizeof(http_header), "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: %zu\r\n\r\n", textSize);

    char *final_response = OGMA_MALLOC(strlen(http_header) + textSize);
    strcpy(final_response, http_header);
    strcpy(final_response + strlen(http_header), text);

    send(response->socket, final_response, strlen(http_header) + textSize, 0);

    OGMA_FREE(final_response);
}

#endif // !OGME_RESPONSE_H