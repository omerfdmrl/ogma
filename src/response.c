#ifndef OGME_RESPONSE_H

#define OGME_RESPONSE_H

#include "ogma.h"

void *response_sendFile(Response response, char * fileName) {
	FILE* file = fopen(fileName, "r");

	if (file == NULL) {
		return NULL;
	}

	fseek(file, 0, SEEK_END);
	long fsize = ftell(file);
	fseek(file, 0, SEEK_SET);

	char* response_data = malloc(sizeof(char) * (fsize+1));
	char ch;
	int i = 0;
	while((ch = fgetc(file)) != EOF) {
		response_data[i] = ch;
		i++;
	}
	fclose(file);
	char http_header[4096] = "HTTP/1.1 200 OK\r\n\r\n";

	strcat(http_header, response_data);
	strcat(http_header, "\r\n\r\n");
	send(response.socket, http_header, sizeof(http_header), 0);

	return NULL;
}

#endif // !OGME_RESPONSE_H