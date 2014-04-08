/**
 * CS 620, Fall 2012
 * Assignment 4
 * 
 * myshell.h
 *
 * Simulates a shell that accepts only external commands.  The shell does not
 * recognize pipes, redirection, background execution, or wild card expansion.
 * The shell uses fork(), execvp(), and wait() to execute the command from
 * user input.  Exits with the "exit" command.
 *
 * @author Carmen St. Jean (crr8)
 * @date October 3, 2012
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define BUFFER_SIZE 500

typedef enum { false, true } bool;

char * delims = " \t";

char * invalid_chars = "|><*&";

/**
 * Executes a command by forking a process, calling execvp, and waiting.
 *
 * @param command The tokenized command from the user input.
 */
void execute_command(char ** command);

/**
 * Determines if a string equals the exit command ("exit").
 *
 * @param str The string to compare with the exit command.
 * @return True if the string is the empty command.
 */
bool is_exit(char * str);

/**
 * Determines if a string equals the empty string ("").
 *
 * @param str The string to compare with the empty string.
 * @return True if the string is an empty string.
 */
bool is_blank(char * str);

/**
 * Determines if the command is one of the recognized commands - e.g., "ls",
 * "dir", "mkdir", etc.
 *
 * @param command The tokenized command from the user.
 * @param True if the command is valid.
 */
bool is_valid_command(char * command);

/**
 * Determines if a command contains invalid characters - i.e., '|', '&', '>',
 * or '<'.
 *
 * @param command The command to check for invalid characters.
 * @return True if the command contains no invalid characters.
 */
bool contains_only_valid_chars(char * command);

/**
 * Tokenizes the command line according to the space (' ') and tab ('\t')
 * delimiters.  NOTE: This destroys the original line string.
 *
 * @param line The command line as typed by the user.
 * @param tokens The array of strings where the tokens of the command line
 *  should be stored.
 */
void tokenize(char * line, char ** tokens);

/**
 * Determines if the given character is contained in the specified string.
 *
 * @param str The string to search for the character in.
 * @param ch The character to search for in the string.
 * @return True if the string contains the character.
 */
bool string_contains(char * str, char ch);

/**
 * Determines if two strings are equal to each other or not.
 *
 * @param stringA The first string to compare.
 * @param stringB The second string to compare.
 * @return True if the two strings are equal.
 */
bool strings_equal(char * stringA, char * stringB);
