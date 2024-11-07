#ifndef OGMA_UTILS_H

#define OGMA_UTILS_H

#include "ogma.h"

unsigned int hash_hash_table(HashTable *table, const char *key) {
    unsigned long hash_value = 0;
    for (size_t i = 0; i < strlen(key); i++) {
        hash_value += key[i];
    }
    return hash_value % table->size;
}

HashTable *init_hash_table(unsigned int size) {
    HashTable *hash_table = (HashTable *)malloc(sizeof(HashTable));
    hash_table->size = size;
    hash_table->table = (HashNode **)malloc(size * sizeof(HashNode *));
    for (size_t i = 0; i < size; i++) {
        hash_table->table[i] = NULL;
    }
    return hash_table;
}

void insert_hash_table(HashTable *hash_table, const char *key, const char *value) {
    unsigned int index = hash_hash_table(hash_table ,key);
    HashNode *new_node = (HashNode *)malloc(sizeof(HashNode));
    new_node->key = strdup(key);
    new_node->value = strdup(value);
    new_node->next = hash_table->table[index];
    hash_table->table[index] = new_node;
}

char *search_hash_table(HashTable *hash_table, const char *key) {
    unsigned int index = hash_hash_table(hash_table, key);
    HashNode *node = hash_table->table[index];
    while (node != NULL) {
        if (strcmp(node->key, key) == 0) {
            return node->value;
        }
        node = node->next;
    }
    return NULL;
}

void print_hash_table(HashTable *hash_table) {
    printf("{ ");
    int first = 1;

    for (size_t i = 0; i < hash_table->size; i++) {
        HashNode *node = hash_table->table[i];
        while (node != NULL) {
            if (!first) printf(", ");
            printf("\"%s\": \"%s\"", node->key, node->value);
            first = 0;
            node = node->next;
        }
    }
    printf(" }");
}

void free_hash_table(HashTable *hash_table) {
    for (size_t i = 0; i < hash_table->size; i++) {
        HashNode *node = hash_table->table[i];
        while (node != NULL) {
            HashNode *temp = node;
            node = node->next;
            free(temp->key);
            free(temp->value);
            free(temp);
        }
    }
    free(hash_table->table);
    free(hash_table);
}

#endif // !OGMA_UTILS_H