#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/resource.h>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fputs("Usage: please supply a command!\n", stderr);
        exit(1);
    }

    while (1) {
        char input[1000]; 
        char *args[100];

        printf("Enter a command: ");

        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }

        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "quit") == 0) {
            break;
        }
        
        char *token = strtok(input, " ");
        int i = 0;
        while (token != NULL && i < 15) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        
        args[i] = NULL;

        pid_t pid = fork();
        
        if (pid == 0) {
        
            execvp(args[0], args); 
            perror("Exec failed!"); 
            exit(1);
            
        } else if (pid > 0) {
        
            struct rusage usage;
            int status;
            wait4(pid, &status, 0, &usage);
            printf("CPU time used: %ld.%06ld seconds\n",
                   usage.ru_utime.tv_sec, usage.ru_utime.tv_usec);
            printf("Involuntary context switches: %ld\n", usage.ru_nivcsw);
            
        } else {
            perror("fork failed");
        }
    }

    return 0;
}



