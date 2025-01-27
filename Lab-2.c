

#include <stdio.h>
#include <unistd.h>
#include <time.h>

void append_to_file(long long int time_diff) {
    FILE *file = fopen("time_diff_file.txt", "a"); // file to contain the 1000 time differences
    fprintf(file, "%lld\n", time_diff);
    fclose(file);
}

int main() {
    printf("\n LED Flash Start \n");

    FILE *LEDHandle = NULL;
    const char *LEDBrightness = "/sys/class/leds/beaglebone:green:usr2/brightness"; 
    struct timespec start, end;
    long long int time_diff;

    // Flash the LED 10 times
    for (int i = 0; i < 1000; i++) {
        // Open the file for reading and writing
        LEDHandle = fopen(LEDBrightness, "r+");
        if (LEDHandle != NULL) {
            clock_gettime(CLOCK_MONOTONIC, &start);
            // Turn the LED on by writing "1"
            fwrite("1", sizeof(char), 1, LEDHandle);
            fclose(LEDHandle); // Close the LED file
        }

        usleep(500000); // Sleep for 0.5 second

        // Open the file again
        if ((LEDHandle = fopen(LEDBrightness, "r+")) != NULL) {
            // Turn the LED off by writing "0"
            fwrite("0", sizeof(char), 1, LEDHandle);
            fclose(LEDHandle); // Close the LED file
        }

        clock_gettime(CLOCK_MONOTONIC, &end);
        time_diff = (end.tv_sec - start.tv_sec) * 1000000000 + (end.tv_nsec - start.tv_nsec);
        append_to_file(time_diff);
    }

    printf("\n LED Flash End \n");

    return 0;
}