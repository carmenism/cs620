#include <stdio.h>
#include <unistd.h>
int main() {
    /* fork a child process */
    fork(); % a
    wait(&status);
    printf("a process id = %d", getpid());
    /* fork another child process */
    fork(); % b
    wait(&status);
    printf("b process id = %d", getpid());
    /* and fork another */
    fork(); % c
    wait (&status);
    printf("c process id = %d", getpid());
    return 0;
}
