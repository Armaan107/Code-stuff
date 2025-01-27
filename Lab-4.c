#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <time.h>





time_t timestamps[10];
int buffer_index = 0;

// Thread function to handle interrupt using epoll
void* threadFunction(void *var) {
    int gpio_number = 67; // P8_8 gpio pin

    // Configure interrupt settings using epoll
    char InterruptPath[40];
    sprintf(InterruptPath, "/sys/class/gpio/gpio%d/value", gpio_number);

    int epfd;
    struct epoll_event ev;

    FILE* fp = fopen(InterruptPath, "r");
    int fd = fileno(fp);

    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = fd;

    epfd = epoll_create(1);
    epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);

    struct epoll_event ev_wait;
    struct timespec tm;

    epoll_wait(epfd, &ev_wait, 1, -1); // to ensure it doesnt repeat in the startimg

    // Wait for rising edge interrupt on pin P8_8 and store timestamps
    while (buffer_index < 10) {
        epoll_wait(epfd, &ev_wait, 1, -1);
        clock_gettime(CLOCK_MONOTONIC, &tm);
        timestamps[buffer_index++] = tm.tv_sec;
    }

    close(epfd);
    return NULL;
}

int main() {
    pthread_t thread_id;
    printf("Before Thread\n");

    // Create a new thread to handle interrupt using epoll
    pthread_create(&thread_id, NULL, threadFunction, NULL);

   
    pthread_join(thread_id, NULL);

    printf("Timestamps:\n");
    for (int i = 0; i < 10; i++) {
        printf("%ld\n", timestamps[i]);
    }

    printf("After Thread\n");
    return 0;
}






// int main() {
//     int gpio_number = 67; // P8_8 gpio pin

//     // The following code can be used to receive interrupts on the registered pin
//     char InterruptPath[40];
//     sprintf(InterruptPath, "/sys/class/gpio/gpio%d/value", gpio_number);

//     int epfd;
//     struct epoll_event ev;

//     // (STEP 1) Open the interrupt file
//     // File pointer (C abstraction to manipulate files)
//     FILE* fp = fopen(InterruptPath, "r");
//     // File descriptor (Unix/Linux file identifier used by system calls)
//     int fd = fileno(fp);

//     // (STEP 3) Register events that will be monitored
//     // Detects whenever new data is available for read (EPOLLIN)
//     // Signals the read events when the available read value has changed (EPOLLET)
//     ev.events = EPOLLIN | EPOLLET;

//     // (STEP 4) Register interrupt file with epoll interface for monitoring
//     ev.data.fd = fd;

//     // (STEP 2) Create epoll instance to monitor I/O events on interrupt file
//     epfd = epoll_create(1);
//     epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);

//     int capture_interrupt;
//     struct epoll_event ev_wait;
//     struct timespec tm;

//     for(int i = 0; i < 10; i++) { // Capture interrupt ten times
//         // (STEP 5) Wait for epoll interface to signal the change
//         capture_interrupt = epoll_wait(epfd, &ev_wait, 1, -1);
//         clock_gettime(CLOCK_MONOTONIC_RAW, &tm);
//         printf("Interrupt received: %d at %ld\n", capture_interrupt, tm.tv_sec);
//     }

//     // (STEP 6) Close the epoll interface
//     close(epfd);

//     return 0;
// }

// // A normal C function that will be executed as a thread
// void* threadFunction(void *var) {
//     // Casting input argument type to integer
//     int* input = (int *) var;
//     printf("Received %d inside thread argument.\n", *input);
//     return NULL;
// }

// int main() {
//     int data = 6;

//     // Pause the program for 1 second
//     sleep(1);

//     pthread_t thread_id;
//     printf("Before Thread\n");

//     // First argument requires reference of pthread_t variable, this variable can be used to manipulate
//     // the created thread in future
//     // Second argument can be used to set thread related settings, we will use default setting and pass NULL
//     // Third argument is the function name which will be executed once the thread starts
//     // Fourth argument is the pointer variable that will be passed as input to the thread function named
//     pthread_create(&thread_id, NULL, threadFunction, (void*)(&data));

//     // Blocks the main function (thread) until the thread function is preempted
//     pthread_join(thread_id, NULL);

//     printf("After Thread\n");
    
//     // Release the thread once finished
//     pthread_exit(0);

//     return 0;
// }