#ifndef OGMA_ASSERT_H

#define OGMA_ASSERT_H

#include "ogma.h" 

const char* assert_names[] = {
    [Warning] = "Warning",
    [Error] = "Error"
};

void assert(int condition, assert_level level, const char *message, ...) {
    va_list args;
    va_start(args, message);
    if(condition) {
        fprintf(stderr, "[%s] ", assert_names[level]);
        fprintf(stderr, message, args);
        fprintf(stderr, "\n");
        exit(EXIT_FAILURE);
    }
    va_end(args);
}

#endif // !OGMA_ASSERT_H