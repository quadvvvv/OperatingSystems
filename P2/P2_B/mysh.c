// Copyright 2021 Yunbo Ni

#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

// Constants for shell configuration
#define MAX_LENGTH 512        // Maximum length of command line input
#define MAX_TOKENS 100        // Maximum number of tokens in a command
#define DELIMITER " \t\r\n\a"  // Delimiters for tokenizing the command
#define EXIT_CMD "exit"       // Command to exit the shell
#define CTRL_D "Ctrl-D"       // Alternative way to exit the shell

// Function prototypes
int check_redirection(int len, char **tokens);
void interactive_loop();
void batch_loop(FILE *batch);

/**
 * Main function to run the shell
 * Handles both interactive and batch modes
 */
int main(int argc, char **argv) {
    // Interactive mode
    if (argc == 1) {
        interactive_loop();
        exit(0);
    // Batch mode
    } else if (argc == 2) {
        FILE *fp = fopen(argv[1], "r");
        if (fp == NULL) {
            // Error handling for file open failure
            char err_msg2[MAX_LENGTH];
            strcpy(err_msg2, "Error: Cannot open file ");
            strcat(err_msg2, argv[1]);
            strcat(err_msg2, ".\n");
            int msg2_len = strlen(err_msg2);
            write(2, err_msg2, msg2_len);
            exit(1);
        }
        batch_loop(fp);
        fclose(fp);
    } else {
        // Error handling for incorrect number of arguments
        char *err_msg1 = "Usage: mysh [batch-file]\n";
        write(2, err_msg1, strlen(err_msg1));
        exit(1);
    }
}

/**
 * Function to handle the interactive shell loop
 * Continuously prompts for user input and executes commands
 */
void interactive_loop() {
    char *prompt = "mysh> ";
    char *err_msg1 = "error reading cmd line\n";
    char *err_msg2 = "cmd line exceeds max length\n";
    char *err_msg3 = "too many args for cmd\n";
    char *err_msg5 = "fork() error\n";
    char *err_msg6 = "Redirection misformatted.\n";
    char *err_msg7 = "Cannot write to file foo.txt.\n";
    char cmd_line[MAX_LENGTH];

    while (1) {
        // Prompt and read command line
        write(1, prompt, strlen(prompt));
        if (fgets(cmd_line, MAX_LENGTH, stdin) != NULL) {
            // Handle case where input exceeds max length
            if (strlen(cmd_line) == MAX_LENGTH &&
                cmd_line[MAX_LENGTH - 2] != '\n') {
                write(2, err_msg2, strlen(err_msg2));
                // Consume remaining input
                int ch;
                while ((ch = fgetc(stdin)) != '\n' && ch != EOF) {}
                continue;
            }
        } else {
            // Handle EOF (Ctrl-D)
            if (feof(stdin)) {
                exit(0);
            } else {
                perror(err_msg1);
                continue;
            }
        }

        // Parse command line
        int i = 0;
        char *tokens[MAX_TOKENS];
        char *token;

        // Check for redirection and handle '>' character
        int redir_index = -1;
        int redir_counter = 0;
        char modified_cmd_line[MAX_LENGTH];
        for (int i = 0; i < strlen(cmd_line); i++) {
            if (cmd_line[i] == '>') {
                redir_index = i;
                redir_counter++;
            }
        }

        // Modify command line if redirection is present
        if (redir_index != -1 && redir_counter != 0) {
            if (redir_counter > 1 || redir_index == 0 ||
                redir_index >= strlen(cmd_line) - 1) {
                write(2, err_msg6, strlen(err_msg6));
                continue;
            }
            memset(modified_cmd_line, '\0', sizeof(modified_cmd_line));
            strncpy(modified_cmd_line, cmd_line, redir_index);
            strcat(modified_cmd_line, " > ");
            char ending_cmd_line[MAX_LENGTH];
            memset(ending_cmd_line, '\0', sizeof(ending_cmd_line));
            strcpy(ending_cmd_line, &cmd_line[redir_index + 1]);
            strcat(modified_cmd_line, ending_cmd_line);
        }

        // Variables for handling redirection
        int redir_tindex = -1;
        int redir = 0;
        char output_file[MAX_LENGTH];

        // Use modified command line if redirection is present
        if (redir_counter == 1) {
            strcpy(cmd_line, modified_cmd_line);
        }

        // Tokenize the command line
        token = strtok(cmd_line, DELIMITER);

        if (token == NULL) continue;  // Skip empty lines

        while (token != NULL) {
            if (!strcmp(token, ">")) {
                redir_tindex = i;
                token = strtok(NULL, DELIMITER);
                continue;  // Skip '>' token
            }

            tokens[i] = token;
            i++;
            token = strtok(NULL, DELIMITER);

            if (i == MAX_TOKENS - 1) break;  // Avoid buffer overflow
        }

        // Handle case where too many arguments are provided
        if (token != NULL) {
            write(2, err_msg3, strlen(err_msg2));
            continue;
        }

        tokens[i] = NULL;  // Null-terminate the token array

        // Handle redirection
        if (redir_tindex != -1) {
            if (redir_tindex == 0 || redir_tindex != (i - 1)) {
                write(2, err_msg6, strlen(err_msg6));
                continue;
            } else {
                redir = 1;
                strcpy(output_file, tokens[i - 1]);
                tokens[i - 1] = NULL;
            }
        }

        // Handle exit command
        if (i == 1 && (!strcmp(tokens[0], EXIT_CMD) || !strcmp(tokens[0], CTRL_D)))
            exit(0);

        // Fork and execute the command
        pid_t pid;
        pid = fork();
        int status;
        if (pid == 0) {  // Child process
            if (redir) {
                // Handle output redirection
                int fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd == -1) {
                    write(2, err_msg7, strlen(err_msg7));
                }
                dup2(fd, fileno(stdout));
                close(fd);
            }
            // Execute the command
            execv(tokens[0], tokens);
            // If execv returns, it must have failed
            char err_msg4[MAX_LENGTH];
            strcpy(err_msg4, tokens[0]);
            strcat(err_msg4, ": Command not found.\n");
            write(2, err_msg4, strlen(err_msg4));
            _exit(1);
        }
        if (pid < 0) {
            // Handle fork error
            write(2, err_msg5, strlen(err_msg5));
            continue;
        }
        // Parent process waits for child to complete
        waitpid(pid, &status, 0);
    }
}

/**
 * Function to handle batch mode
 * Reads commands from a file and executes them
 */
void batch_loop(FILE *batch) {
    char cmd_line[MAX_LENGTH];
    char *err_msg2 = "cmd line exceeds max length\n";
    char *err_msg3 = "too many args for cmd\n";
    char *err_msg5 = "fork() error\n";
    char *err_msg6 = "Redirection misformatted.\n";
    char *err_msg7 = "Cannot write to file foo.txt.\n";

    while (fgets(cmd_line, MAX_LENGTH, batch) != NULL) {
        // Handle case where command exceeds max length
        if (strlen(cmd_line) == MAX_LENGTH && cmd_line[MAX_LENGTH - 2] != '\n') {
            write(2, err_msg2, strlen(err_msg2));
            // Consume the rest of the line
            int ch;
            while ((ch = fgetc(batch)) != '\n' && ch != EOF) {}
            continue;
        }

        // Echo each command
        write(1, cmd_line, strlen(cmd_line));

        // Parse command line (similar to interactive mode)
        int i = 0;
        char *tokens[MAX_TOKENS];
        char *token;

        // Check for redirection
        int redir_index = -1;
        int redir_counter = 0;
        char modified_cmd_line[MAX_LENGTH];
        for (int i = 0; i < strlen(cmd_line); i++) {
            if (cmd_line[i] == '>') {
                redir_index = i;
                redir_counter++;
            }
        }

        // Handle redirection formatting
        if (redir_index != -1 && redir_counter != 0) {
            if (redir_counter > 1 || redir_index == 0 || redir_index >= strlen(cmd_line) - 1) {
                write(2, err_msg6, strlen(err_msg6));
                continue;
            }
            // Modify command line for redirection
            memset(modified_cmd_line, '\0', sizeof(modified_cmd_line));
            strncpy(modified_cmd_line, cmd_line, redir_index);
            strcat(modified_cmd_line, " > ");
            char ending_cmd_line[MAX_LENGTH];
            memset(ending_cmd_line, '\0', sizeof(ending_cmd_line));
            strcpy(ending_cmd_line, &cmd_line[redir_index + 1]);
            strcat(modified_cmd_line, ending_cmd_line);
        }

        // Variables for handling redirection
        int redir_tindex = -1;
        int redir = 0;
        char output_file[MAX_LENGTH];

        if (redir_counter == 1) {
            strcpy(cmd_line, modified_cmd_line);
        }

        // Tokenize the command
        token = strtok(cmd_line, DELIMITER);

        if (token == NULL) continue;  // Skip empty lines

        while (token != NULL) {
            if (!strcmp(token, ">")) {
                redir_tindex = i;
                token = strtok(NULL, DELIMITER);
                continue;  // Skip '>' token
            }

            tokens[i] = token;
            i++;
            token = strtok(NULL, DELIMITER);

            if (i == MAX_TOKENS - 1) break;  // Avoid buffer overflow
        }

        // Handle case where too many arguments are provided
        if (token != NULL) {
            write(2, err_msg3, strlen(err_msg2));
            continue;
        }

        tokens[i] = NULL;  // Null-terminate the token array

        // Handle redirection
        if (redir_tindex != -1) {
            if (redir_tindex == 0 || redir_tindex != (i - 1)) {
                write(2, err_msg6, strlen(err_msg6));
                continue;
            } else {
                redir = 1;
                strcpy(output_file, tokens[i - 1]);
                tokens[i - 1] = NULL;
            }
        }

        // Handle exit command
        if (i == 1 && (!strcmp(tokens[0], EXIT_CMD) || !strcmp(tokens[0], CTRL_D)))
            exit(0);

        // Fork and execute the command
        pid_t pid;
        pid = fork();
        int status;
        if (pid == 0) {  // Child process
            if (redir) {
                // Handle output redirection
                int fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd == -1) {
                    write(2, err_msg7, strlen(err_msg7));
                }
                dup2(fd, fileno(stdout));
                close(fd);
            }
            // Execute the command
            execv(tokens[0], tokens);
            // If execv returns, it must have failed
            char err_msg4[MAX_LENGTH];
            strcpy(err_msg4, tokens[0]);
            strcat(err_msg4, ": Command not found.\n");
            write(2, err_msg4, strlen(err_msg4));
            _exit(1);
        }
        if (pid < 0) {
            // Handle fork error
            write(2, err_msg5, strlen(err_msg5));
            continue;
        }
        // Parent process waits for child to complete
        waitpid(pid, &status, 0);
    }
    fclose(batch);
    exit(0);
}