// my-rev: A utility to reverse lines from a file or standard input
// Copyright 2021 Yunbo Ni

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAX_LENGTH 100  // Maximum length for each line to be reversed

int main(int argc, char **argv) {
    char *fvalue = NULL;
    char c;
    char rev_str[MAX_LENGTH];
    char og_str[MAX_LENGTH];

    // Step 1 - Parse command-line arguments
    while ((c = getopt(argc, argv, "Vhf:")) != -1) {
        switch (c) {
            case 'V':
                printf("my-rev from CS537 Summer 2021\n");
                exit(0);
            case 'h':
                printf("Usage: my-rev [-V] [-h] [-f <filename>]\n");
                printf("Reverses each line of the specified file or standard input.\n");
                exit(0);
            case 'f':
                fvalue = optarg;
                break;
            case '?':
                fprintf(stderr, "my-rev: invalid command line option\n");
                exit(1);
            default:
                abort();
        }
    }

    // Step 2 - Reverse lines from file or standard input
    if (fvalue != NULL && optind == argc) {  // Case 1: Reverse lines from specified file
        FILE *fp = fopen(fvalue, "r");
        if (fp == NULL) {
            fprintf(stderr, "my-rev: cannot open file %s\n", fvalue);
            exit(1);
        }

        // Process each line in the file
        while (fgets(og_str, MAX_LENGTH, fp) != NULL) {
            int len = strlen(og_str);
            int nflag = (og_str[len - 1] == '\n') ? 1 : 0;

            // If line length is 1 or less, skip reversal
            if (len <= 1) {
                printf("%s", og_str);
                continue;
            }

            // Reverse line
            snprintf(rev_str, len + 1, "%s", og_str);
            for (int i = 0; i < len - nflag; i++) {
                rev_str[i] = og_str[len - nflag - 1 - i];
            }
            printf("%s", rev_str);
        }
        fclose(fp);
        exit(0);

    } else if (fvalue == NULL && optind == 1 && argc == 1) {  // Case 2: Reverse lines from standard input
        while (fgets(og_str, MAX_LENGTH, stdin) != NULL) {
            int len = strlen(og_str);
            int nflag = (og_str[len - 1] == '\n') ? 1 : 0;

            // If line length is 1 or less, skip reversal
            if (len <= 1) {
                printf("%s", og_str);
                continue;
            }

            // Reverse line
            snprintf(rev_str, len + 1, "%s", og_str);
            for (int i = 0; i < len - nflag; i++) {
                rev_str[i] = og_str[len - nflag - 1 - i];
            }
            printf("%s", rev_str);
        }
        exit(0);

    } else {  // Invalid command line usage
        fprintf(stderr, "my-rev: invalid command line usage\n");
        exit(1);
    }
}
