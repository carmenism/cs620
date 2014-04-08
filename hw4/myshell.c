/**
 * CS 620, Fall 2012
 * Assignment 4
 * 
 * myshell.c
 *
 * Simulates a shell that accepts only external commands.  The shell does not
 * recognize pipes, redirection, background execution, or wild card expansion.
 * The shell uses fork(), execvp(), and wait() to execute the command from
 * user input.  Exits with the "exit" command.
 *
 * @author Carmen St. Jean (crr8)
 * @date October 3, 2012
 */
#include "myshell.h"

int main(void) {
    char line[BUFFER_SIZE];
    char lineCopy[BUFFER_SIZE];
    char * command[BUFFER_SIZE];
    
    while (1) { 
        printf("myshell %% ");
        gets(line);
        
        strcpy(lineCopy, line);
        
        if (is_exit(line)) {
            exit(0);
        } else if (!is_blank(line)) {
            tokenize(lineCopy, command);        
            
            if (contains_only_valid_chars(line) && is_valid_command(command[0])) {
                execute_command(command);
            } else {
                printf("Command not found: %s\n", line);
            }
        }   
    }
    
    return 0;
}

/**
 * Executes a command by forking a process, calling execvp, and waiting.
 *
 * @param command The tokenized command from the user input.
 */
void execute_command(char ** command) {
    pid_t child_id;
    int child_status;
    
    // Fork a child process.
    if ((child_id = fork()) < 0) {
        // Forking a child process failed.
        exit(1);
    }
    
    if (child_id == 0) { // In child process.
        // Execute the command.
        execvp(*command, command);
    } else { // in parent process
        // Wait for the child process to complete.
        wait(&child_status);
    }
}

/**
 * Determines if a string equals the exit command ("exit").
 *
 * @param str The string to compare with the exit command.
 * @return True if the string is the empty command.
 */
bool is_exit(char * str) {
    return (strings_equal(str, "exit"));
}

/**
 * Determines if a string equals the empty string ("").
 *
 * @param str The string to compare with the empty string.
 * @return True if the string is an empty string.
 */
bool is_blank(char * str) {
    return (strings_equal(str, ""));
}

/**
 * Determines if the command is one of the recognized commands - e.g., "ls",
 * "dir", "mkdir", etc.
 *
 * @param command The tokenized command from the user.
 * @param True if the command is valid.
 */
bool is_valid_command(char * command) {
    FILE *fp;
    char path[1035];
    char pathFull[1035];

    char typeCommand[1035];
    strcpy(typeCommand, "type ");
    strcat(typeCommand, command);

    char internalMessage[1035];
    strcpy(internalMessage, command);
    strcat(internalMessage, " is a shell builtin");

    // Open the command for reading.
    fp = popen(typeCommand, "r");
    if (fp == NULL) {
        printf("Failed to run command\n" );
        return false;
    }

    // Read only the first output line from the file.
    if (fgets(path, sizeof(internalMessage) - 1, fp) != NULL) {
        strcpy(pathFull, path);
    } else {
        return false;
    }
    
    // Replace the new line with a null character.
    char * firstNewLine = strchr(pathFull, '\n');
    *firstNewLine = '\0';
    
    // Close the file.
    pclose(fp);

    return !strings_equal(path, internalMessage);
}

/**
 * Determines if a command contains invalid characters - i.e., '|', '&', '>',
 * or '<'.
 *
 * @param command The command to check for invalid characters.
 * @return True if the command contains no invalid characters.
 */
bool contains_only_valid_chars(char * command) {
    int i;
    
    for (i = 0; i < strlen(invalid_chars); i++) {
        if (string_contains(command, invalid_chars[i])) {
            return false;
        }
    }

    return true;
}

/**
 * Tokenizes the command line according to the space (' ') and tab ('\t')
 * delimiters.  NOTE: This destroys the original line string.
 *
 * @param line The command line as typed by the user.
 * @param tokens The array of strings where the tokens of the command line
 *  should be stored.
 */
void tokenize(char * line, char ** tokens) {
    char * token = strtok(line, delims);
    int i = 0;
    
    while (token != NULL) {
        tokens[i] = token;        
        token = strtok(NULL, delims);
        i++;
    }
    
    tokens[i] = '\0';
}

/**
 * Determines if the given character is contained in the specified string.
 *
 * @param str The string to search for the character in.
 * @param ch The character to search for in the string.
 * @return True if the string contains the character.
 */
bool string_contains(char * str, char ch) {
    return (strchr(str, ch) != NULL);
}

/**
 * Determines if two strings are equal to each other or not.
 *
 * @param stringA The first string to compare.
 * @param stringB The second string to compare.
 * @return True if the two strings are equal.
 */
bool strings_equal(char * stringA, char * stringB) {
    return (strcmp(stringA, stringB) == 0);
}
