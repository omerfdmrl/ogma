#ifndef OGMA_LOGGER_H

#define OGMA_LOGGER_H

#include "ogma.h"

void logger(const char *message, ...) {
    va_list args;
    va_start(args, message);
    fprintf(stdout, "[Log] ");
    vfprintf(stdout, message, args);
    fprintf(stdout, "\n");
    va_end(args);
};

#endif // !OGMA_LOGGER_H