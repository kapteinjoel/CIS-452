#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <signal.h>

int total_requests = 0;
int thread_count = 0;

// Signal handler for sigint (Ctrl+C)
void sigint_handler(int sig) {
    // Wait for worker threads to complete
    while (thread_count > 0) {
        usleep(100000);  // Sleep for 100ms to give threads time to finish
    }
    printf("\nServer simulation shutting down. Total file requests: %d\n", total_requests);
    exit(0); // Exit
}

// Worker thread function
void* worker_thread(void* arg) {
    int access_time;
    char* filename = (char*)arg;
    
    // Simulate accessing a file with an 80% chance to take 1 second and a 20% chance to take 7-10 seconds
    if (rand() % 10 < 8) {
        access_time = 1;
    } else {
        access_time = 7 + rand() % 4;
    }
    
    sleep(access_time);
    
    // Print results
    printf("Worker: Finished accessing file '%s' in %d seconds.\n", filename, access_time);
    
    
    // Free memory and decrement total thread ongoing
    thread_count--;
    free(filename);
    return NULL;
}

int main() {
    // Populate the random number generator with a new seed
    srand(time(NULL));
    pthread_t thread;
    char input[100];

    // Signal handler for SIGINT (Ctrl+C press)
    signal(SIGINT, sigint_handler);

    printf("Enter filenames (Ctrl+C to exit):\n");

    while (1) {
        // Read user input 
        if (scanf("%99s", input) != 1) break;

        // Allocate memory to pass filename to worker thread
        char* filename = strdup(input);
        if (!filename) {
            perror("Memory allocation failed");
            continue;
        }

        // Create a worker thread and handle failure
        if (pthread_create(&thread, NULL, worker_thread, filename) != 0) {
            perror("Thread creation failed");
            free(filename);
        } else {
            total_requests++; // Update total requests
            thread_count++;
            pthread_detach(thread); // Clean up thread once done
        }
    }

    return 0;
}