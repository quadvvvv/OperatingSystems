// my-look: A utility to search for matching lines in a file or standard input
// Copyright 2021 Yunbo Ni

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAX_LENGTH 100  // Maximum length for each line in the dictionary or input

int main(int argc, char **argv) {
    char *fvalue = NULL;
    char c;
    char search_str[MAX_LENGTH];
    char dict_str[MAX_LENGTH];

    // Step 1 - Check and parse command-line arguments
    if (argc <= 1) {
        fprintf(stderr, "my-look: invalid command line\n");
        exit(1);
    }

    // Step 1.1 - Parse flags: -V, -h, and -f
    while ((c = getopt(argc, argv, "Vhf:")) != -1) {
        switch (c) {
            case 'V':
                printf("my-look from CS537 Summer 2021\n");
                exit(0);
            case 'h':
                printf("Usage: my-look [-V] [-h] [-f <filename>] <search_string>\n");
                printf("Searches for lines that start with <search_string>.\n");
                exit(0);
            case 'f':
                fvalue = optarg;
                break;
            case '?':
                fprintf(stderr, "my-look: invalid command line\n");
                exit(1);
            default:
                abort();
        }
    }

    // Step 2 - Validate search string
    if (optind != argc - 1) {
        fprintf(stderr, "my-look: missing search string\n");
        exit(1);
    }
    snprintf(search_str, strlen(argv[optind]) + 1, "%s", argv[optind]);

    // Step 3 - Search from file or standard input
    if (fvalue != NULL && optind == argc - 1) {  // Case 1: Search in specified file
        FILE *fp = fopen(fvalue, "r");
        if (fp == NULL) {
            fprintf(stderr, "my-look: cannot open file %s\n", fvalue);
            exit(1);
        }

        // Process each line in the file and print matches
        while (fgets(dict_str, MAX_LENGTH, fp) != NULL) {
            if (!strncasecmp(dict_str, search_str, strlen(search_str))) {
                printf("%s", dict_str);
            }
        }
        fclose(fp);
        exit(0);

    } else if (fvalue == NULL && optind == (argc - 1)) {  // Case 2: Search in standard input
        while (fgets(dict_str, MAX_LENGTH, stdin) != NULL) {
            if (!strncasecmp(dict_str, search_str, strlen(search_str))) {
                printf("%s", dict_str);
            }
        }
        exit(0);

    } else {
        fprintf(stderr, "my-look: invalid command line usage\n");
        exit(1);
    }
}
