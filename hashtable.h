#ifndef htable_H
#define htable_H

#include "uthash.h"

#define TEXT_1 1
#define TEXT_2 2
#define MAX_WORD 50

struct Word {
    char key[MAX_WORD];
    int count1;
    int count2;
    double ll; // the log likelihood
    UT_hash_handle hh;
};

struct Word* words = NULL;

void add_word(int table_num, char* word, int count) {
    struct Word* w;
    w = (struct Word*) malloc(sizeof *w);
    strcpy(w->key, word);
    if (table_num == TEXT_1) {
        HASH_ADD_STR(words, key, w);
        w->count1 = count;
        w->count2 = 0;
    }
    else {
        HASH_ADD_STR(words, key, w);
        w->count2 = count;
        w->count1 = 0;
    }
    w->ll = 0;
}

struct Word* get_count(char* word) {
    struct Word* entry;
    HASH_FIND_STR(words, word, entry);
    return entry;
}

void add_count(int table_num, char* word) {
    struct Word* w;
    w = get_count(word);

    // if the word does not exist in the hashtable
    if (w == NULL) {
        w = (struct Word*) malloc(sizeof *w);
        add_word(table_num, word, 1);
    }
    else {
        // increment current value by one
        if (table_num == TEXT_1) {
            w->count1 += 1;
        }
        else {
            w->count2 += 1;
        }
    }
}

int sort_by_likelihood(struct Word* w1, struct Word* w2) {
    int w1_log = w1->ll;
    int w2_log = w2->ll;

    if (w1_log < w2_log) {
        return 1;
    }
    return -1;
}

void sort() {
    HASH_SORT(words, sort_by_likelihood);
}

#endif /* htable_H */