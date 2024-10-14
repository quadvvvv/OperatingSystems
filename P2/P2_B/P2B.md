# Custom Shell Implementation (mysh)

## Project Overview
This project involves the implementation of a custom command-line interpreter (shell) in C, designed to run on Linux systems. The shell, named `mysh`, provides basic functionality similar to common Unix shells but with a simplified feature set.

## Key Features

1. **Dual-Mode Operation**
   - Interactive Mode: Prompts user for commands
   - Batch Mode: Executes commands from a specified file

2. **Command Execution**
   - Executes external commands using `fork()`, `execv()`, and `waitpid()`
   - Handles command-line arguments

3. **Output Redirection**
   - Supports redirecting command output to files using '>'

4. **Built-in Commands**
   - `exit`: Terminates the shell
   - `alias`: Creates command aliases (optional feature)
   - `unalias`: Removes command aliases (optional feature)

5. **Error Handling**
   - Robust error checking and reporting for various scenarios

## Technical Highlights

- Written in C for Linux environments
- Utilizes system calls for process management and file operations
- Implements parsing of command-line inputs
- Handles file I/O for batch mode and output redirection

## Learning Outcomes

- Deep understanding of process creation and management in Unix-like systems
- Experience with system calls and low-level I/O operations
- Practice in parsing and tokenizing input strings
- Exposure to shell design and functionality

## Usage

```
./mysh [batch-file]
```

- Run without arguments for interactive mode
- Specify a batch file for batch mode execution

This project demonstrates proficiency in C programming, understanding of Unix process management, and ability to implement core shell functionalities.