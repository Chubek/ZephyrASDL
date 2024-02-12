#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "absyn.h"

#define MAX_KEY_LENGTH 255
#define MAX_VALUE_LENGTH 255
#define MAX_TABLE_SIZE 66536
#define MAX_STACK_SIZE 1024

extern int line_num;
extern int col_num;

typedef struct {
    char key[MAX_KEY_LENGTH];
    char value[MAX_VALUE_LENGTH];
} MemoEntry;

MemoEntry memo_table[MAX_TABLE_SIZE] = {0};

void memo_table_insert(const char *key, const char *value) {
    if (key == NULL || value == NULL) {
        return;
    }

    size_t key_length = strlen(key);
    if (key_length >= MAX_KEY_LENGTH) {
        key_length = MAX_KEY_LENGTH - 1;
    }

    size_t value_length = strlen(value);
    if (value_length >= MAX_VALUE_LENGTH) {
        value_length = MAX_VALUE_LENGTH - 1;
    }

    unsigned int hash = 0;
    for (size_t i = 0; i < key_length; ++i) {
        hash = (hash * 31) + key[i];
    }
    unsigned int index = hash % MAX_TABLE_SIZE;

    strncpy(memo_table[index].key, key, key_length);
    memo_table[index].key[key_length] = '\0'
    strncpy(memo_table[index].value, value, value_length);
    memo_table[index].value[value_length] = '\0';
}


const char* memo_table_retrieve(const char *key) {
    if (key == NULL) {        
        return NULL;
    }
    
    unsigned int hash = 0;
    size_t key_length = strlen(key);
    for (size_t i = 0; i < key_length; ++i) {
        hash = (hash * 31) + key[i];
    }
    unsigned int index = hash % MAX_TABLE_SIZE;

    
    for (size_t i = index; i < MAX_TABLE_SIZE; ++i) {
        if (memo_table[i].valid && strcmp(memo_table[i].key, key) == 0) {            
            return memo_table[i].value;
        }
    } 

    return NULL;
}

char* string_concat(const char* str1, const char* str2) {
    
    size_t len1 = strlen(str1);
    size_t len2 = strlen(str2);
    
    char* result = (char*)malloc((len1 + len2 + 1) * sizeof(char));
    if (result == NULL) {        
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    
    strcpy(result, str1);    
    strcat(result, str2);

    return result;
}


