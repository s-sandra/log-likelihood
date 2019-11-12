#ifndef htable_H
#define htable_H

#include "uthash.h"

#define TEXT_1 1
#define TEXT_2 2
#define MAX_WORD 50

struct Word {
    char key[MAX_WORD];
    int count;
    UT_hash_handle hh;
};

struct Word* words1 = NULL;
struct Word* words2 = NULL;

void add_word(int table_num, char* word, int count) {
    struct Word* w;
    w = (struct Word*) malloc(sizeof *w);
    strcpy(w->key, word);
    w->count = count;
    if (table_num == TEXT_1) {
        HASH_ADD_STR(words1, key, w);
    }
    else {
        HASH_ADD_STR(words2, key, w);
    }
}

struct Word* get_count(int table_num, char* word) {
    struct Word* entry;
    if (table_num == TEXT_1) {
        HASH_FIND_STR(words1, word, entry);
    }
    else {
        HASH_FIND_STR(words2, word, entry);
    }
    return entry;
}

void add_count(int table_num, char* word) {
    struct Word* w;
    w = get_count(table_num, word);

    // if the word does not exist in the hashtable
    if (w == NULL) {
        w = (struct Word*) malloc(sizeof *w);
        add_word(table_num, word, 1);
    }
    else {
        w->count += 1; // increment current value by one
    }
}

int sort_by_count(struct Word* w1, struct Word* w2) {
    int w1_count = w1->count;
    int w2_count = w2->count;

    if (w1_count < w2_count) {
        return -1;
    }
    return 1;
}

void sort(int table_num) {
    if (table_num == TEXT_1) {
        HASH_SORT(words1, sort_by_count);
    }
    else {
        HASH_SORT(words2, sort_by_count);
    }
}

#endif /* htable_H */