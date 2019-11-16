/**
* Name: Sandra Shtabnaya
* Avatar: Meta Genie
* Course: CPSC 405_01
* Assignment: Lab 4 - Parallel Log Likelihood
*/

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <sys/mman.h>
#include <sys/sysinfo.h>
#include <sys/stat.h>
#include "threads.h"
#include "hashtable.h"
#include "rb_tree.c"

struct File {
    char* file;
    char name[MAX_WORD];
    int fp;
    int fd;
    int size;
    long int word_count;
    int id;
    pthread_mutex_t mutex;
};

double log_likelihood(struct Word* word);
double entropy();
void sort();
void print_table();
int** make_contingency_table(struct Word* word);
void init_file(struct File* text, char* name, int id);
void parse_word(struct File* file);
void* parse_text(void* file);
int compare_by_likelihood(struct rb_tree* self, struct rb_node* w1, struct rb_node* w2);

struct File text1;
struct File text2;
pthread_rwlock_t write_lock;
int THREAD_COUNT;
int CUTOFF = 35;

int main(int argc, char** argv) {
    if (!argv[1] || !argv[2]) {
        printf("Please enter two text files.\n");
        exit(0);
    }

    init_file(&text1, argv[1], TEXT_1);
    init_file(&text2, argv[2], TEXT_2);
    Pthread_rwlock_init(&write_lock, NULL);

    THREAD_COUNT = (get_nprocs_conf() / 2 == 0) ? 50 : get_nprocs_conf() / 2;
    pthread_t text1_threads[THREAD_COUNT];
    pthread_t text2_threads[THREAD_COUNT];

    for (int thread = 0; thread < THREAD_COUNT; thread++) {
        Pthread_create(&text1_threads[thread], NULL, parse_text, (void*) &text1);
        Pthread_create(&text2_threads[thread], NULL, parse_text, (void*) &text2);
    }

    for (int thread = 0; thread < THREAD_COUNT; thread++) {
        Pthread_join(text1_threads[thread], NULL);
        Pthread_join(text2_threads[thread], NULL);
    }

    print_table();
    close(text1.fd);
    close(text2.fd);
    return 0;
}

void init_file(struct File* text, char* name, int id) {
    strcpy(text->name, name);
    int fd = open(text->name, O_RDONLY);
    text->fd = fd;
    text->fp = 0;
    struct stat sb;

    if (fstat(text->fd, &sb) == -1) {
        perror("File size error.\n");
    }
    // read only, start at beginning of file, make file private to process
    char* mem = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, text->fd, text->fp);
    text->file = mem;
    text->size = sb.st_size;
    text->word_count = 0;
    text->id = id;
    Pthread_mutex_init(&text->mutex, NULL);
}

void* parse_text(void* text) {
    while (1) {
        Pthread_mutex_lock(&((struct File*) text)->mutex);

        if (((struct File*) text)->fp >= ((struct File*) text)->size) {
            Pthread_mutex_unlock(&((struct File*) text)->mutex);
            break;
        }

        parse_word((struct File*) text);
        Pthread_mutex_unlock(&((struct File*) text)->mutex);
    }
    return NULL;
}

void parse_word(struct File* text) {

    char word[MAX_WORD];
    word[0] = '\0';
    char letter;
    letter = text->file[text->fp];
    text->fp += 1;

    while (!isspace(letter) && !ispunct(letter)) {

        // checks for special UTF-8 characters
        if (((letter & 0x000000ff) >> 5) == 0b111 
            && (text->file[text->fp] & 0x000000ff) == 0x80) {

            text->fp += 2;
            break;
        }
        else{
            strncat(word, &letter, 1);
            letter = text->file[text->fp];
            text->fp += 1;
        }
    }
    if (word[0] != '\0') {
        Pthread_rwlock_wrlock(&write_lock);
        if (text->id == TEXT_1) {
            add_count(TEXT_1, word);
        }
        else {
            add_count(TEXT_2, word);
        }
        text->word_count += 1;
        Pthread_rwlock_unlock(&write_lock);
    }
}

void print_table() {
    // prints header
    printf("Word,%s count,%s count,%s freq,%s freq,Log Likelihood\n", 
        text1.name, text2.name, text1.name, text2.name);
    
    struct Word* w;
    struct rb_tree* sorted = rb_tree_create(compare_by_likelihood);

    // computes log likelihood
    for (w = words; w != NULL; w = w->hh.next) {
        if (text1.word_count > CUTOFF && text2.word_count > CUTOFF && 
            (w->count1 > CUTOFF || w->count2 > CUTOFF)) {
            double g2 = log_likelihood(w);
            w->ll = g2;
            rb_tree_insert(sorted, w);
        }
    }

    //sort(words);
    struct rb_iter* iterator = rb_iter_create();
    for (w = rb_iter_first(iterator, sorted); w != NULL; w = rb_iter_next(iterator)) {
        printf("%s,", w->key);
        printf("%d,", w->count1);
        printf("%d,", w->count2);
        printf("%0.5f,", (double) w->count1 / text1.word_count);
        printf("%0.5f,", (double) w->count2 / text2.word_count);
        printf("%0.5f\n", w->ll);
    }

    rb_iter_dealloc(iterator);
    rb_tree_dealloc(sorted, NULL);
}

int compare_by_likelihood(struct rb_tree* self, struct rb_node* w1, struct rb_node* w2) {
    struct Word* this = (struct Word*) w1->value;
    struct Word* other = (struct Word*) w2->value;

    return (this->ll < other->ll) - (this->ll > other->ll);
}

/*
* Takes an array that contains the frequency of a word occurring and
* not occurring in two texts.
*/
double entropy(int** contingency_table) {
    double entropy = 0;
    int sum = 0;

    for (int row = 0; row < 2; row++) {
        int row_total = 0;
        int col_total = 0;
        
        for (int col = 0; col < 2; col++) {
            int frequency = contingency_table[row][col];
            if (frequency > 0) {
                entropy += frequency * log(frequency);
            }
            sum += frequency;
            row_total += frequency;
            col_total += contingency_table[col][row];
        }
        
        entropy -= row_total * log(row_total);
        entropy -= col_total * log(col_total);
    }

    entropy += sum * log(sum);
    return entropy;
}


double log_likelihood(struct Word* word) {
    int** table = make_contingency_table(word);
    return 2 * entropy(table);
}

int** make_contingency_table(struct Word* word) {
    int **contingency_table = (int**) malloc(2 * sizeof(int *)); 
    for (int i = 0; i < 2; i++) 
         contingency_table[i] = (int*) malloc(2 * sizeof(int)); 

    int count1 = word->count1 + 1;
    int count2 = word->count2 + 1;
    int not_word1 = text1.word_count - count1;
    int not_word2 = text2.word_count - count2;

    contingency_table[0][0] = count1;
    contingency_table[0][1] = count2;
    contingency_table[1][0] = not_word1;
    contingency_table[1][1] = not_word2;

    return contingency_table;
}