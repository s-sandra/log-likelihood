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

struct File {
    char* file;
    char name[MAX_WORD];
    int fp;
    int fd;
    int size;
    int id;
};

double log_likelihood();
double entropy();
void sort();
void print_table();
int** make_contingency_table(int word);
void init_file(struct File* text, char* name, int id);
void parse_word(struct File* file);
void parse_text(struct File* file);

int text1_counts[1] = {1047};
int text1_total = 109224;
int text2_total = 115692;
int text2_counts[1] = {0};
int total_words = 1;

struct File text1;
struct File text2;

int main(int argc, char** argv) {
    if (!argv[1] || !argv[2]) {
        printf("Please enter two text files.\n");
        exit(0);
    }

    init_file(&text1, argv[1], TEXT_1);
    parse_text(&text1);

    // char* name = "Buddha";
    // add_word(TEXT_2, name, 1417);
    // struct Word* w;
    // w = get_count(TEXT_2, name);
    // if (w == NULL) {
    //     printf("Ha1");
    // }
    // printf("Count is: %d\n", w->count);
    // number of processors available get_nprocs();
    // number of processors get_nprocs_conf();
    print_table();
    close(text1.fd);
    return 0;
}

void init_file(struct File* text, char* name, int id) {
    strcpy(text->name, name);
    int fd = open(text->name, O_RDONLY);
    text->fd = fd;
    text->fp = 0;
    struct stat sb;

    if (fstat(text->fd, &sb) == -1) {
        perror("File 1 size error");
    }
    // read only, start at beginning of file, make file private to process
    char* mem = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, text->fd, text->fp);
    text->file = mem;
    text->size = sb.st_size;
    text->id = id;
}

void parse_text(struct File* text) {
    while (text->fp < text->size) {
        parse_word(text);
    }
}

void parse_word(struct File* text) {
    char word[MAX_WORD];
    word[0] = '\0';
    char letter;
    letter = text->file[text->fp];
    text->fp += 1;

    while (!isspace(letter) && !ispunct(letter)) {
        strncat(word, &letter, 1);
        letter = text->file[text->fp];
        text->fp += 1;
    }
    if (word[0] != '\0') {
        if (text->id == TEXT_1) {
            add_count(TEXT_1, word);
        }
        else {
            add_count(TEXT_2, word);
        }
    }
}

void print_table() {
    // print header
    printf("Word,%s count,%s count,%s freq,%s freq,Log Likelihood\n", 
        text1.name, text2.name, text1.name, text2.name);

    int total_words = max()
    // computes log likelihood for top ten most frequent words
    for (int word = 0; word < total_words; word++) {
        printf("%d,", text1_counts[word]);
        printf("%d,", text2_counts[word]);
        printf("%0.5f,", (double) text1_counts[word] / text1_total);
        printf("%0.5f,", (double) text2_counts[word] / text2_total);
        double g2 = log_likelihood(word);
        printf("%0.5f\n", g2);
    }
}

/*
* Takes an array that contains the frequency of a word occurring and
* not occurring in two corpora.
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


double log_likelihood(int word) {
    int** table = make_contingency_table(word);
    return 2 * entropy(table);
}

int** make_contingency_table(int word) {
    int **contingency_table = (int**) malloc(2 * sizeof(int *)); 
    for (int i = 0; i < 2; i++) 
         contingency_table[i] = (int*) malloc(2 * sizeof(int)); 

    int count1 = text1_counts[word] + 1;
    int count2 = text2_counts[word] + 1;
    int not_word1 = text1_total - count1;
    int not_word2 = text2_total - count2;

    contingency_table[0][0] = count1;
    contingency_table[0][1] = count2;
    contingency_table[1][0] = not_word1;
    contingency_table[1][1] = not_word2;

    return contingency_table;
}