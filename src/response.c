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


#endif // !OGME_RESPONSE_H