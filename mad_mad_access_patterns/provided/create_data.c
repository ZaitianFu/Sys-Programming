/**
* Mad Mad Access Patterns Lab
* CS 241 - Fall 2018
*/

#include "treebuild.h"
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DELIM " ,\r\t\n"

/*
  Data is from http://norvig.com/ngrams/
  wget http://norvig.com/ngrams/count_1w.txt
*/

void printHelp();
float randomPrice(const char *word);

int main(int argc, char **argv) {
    BinaryTree tree;
    BinaryTree_init(&tree);

    if (argc != 3)
        printHelp();

    FILE *inf = fopen(argv[1], "r");
    if (!inf) {
        fprintf(stderr, "Cannot read %s\n", argv[1]);
        return 1;
    }

    FILE *outf = fopen(argv[2], "w");
    if (!outf) {
        fprintf(stderr, "Cannot write %s\n", argv[2]);
        return 1;
    }

    char *line = NULL;
    size_t line_len = 0;
    int line_no = 0;

    while (getline(&line, &line_len, inf) != -1) {
        line_no++;

        if (line_no % 100 == 0) {
            printf("\rReading line %d", line_no);
            fflush(stdout);
        }

        char *word = strtok(line, DELIM);
        char *count_str = strtok(NULL, DELIM);
        long count;
        // printf("'%s' -> '%s'\n", word, count_str);

        if (1 != sscanf(count_str, "%ld", &count)) {
            fprintf(stderr, "Invalid count: \"%s\"\n", count_str);
        } else {
            if (count > UINT_MAX)
                count = UINT_MAX;
            float price = randomPrice(word);
            // printf("price of %s = %.02f\n", word, price);
            BinaryTree_add(&tree, word, count, price);
        }
    }

    printf("\rReading line %d", line_no);

    BinaryTree_write(&tree, outf);

    printf("\nWrote %s\n", argv[2]);

    BinaryTree_destroy(&tree);
    free(line);
    return 0;
}

void printHelp() {
    fprintf(stderr, "\n  create_data <input_file> <output_file>\n\n");
    exit(1);
}

unsigned permuteString(unsigned r, const char *word) {
    while (*word) {
        r = r ^ *word++;
        r = r * 16777619;
    }
    return r;
}

/* Returns a 'random' price between 0.01 (10^-2) and 100 (10^2)
   that is a function of the word. */
float randomPrice(const char *word) {
    unsigned r = 2166136261u;
    r = permuteString(r, "init");
    r = permuteString(r, word);

    double price = (int)pow(10.0, (double)r / UINT_MAX * 4) / 100.0;

    return (float)price;
}
