#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

// Sets up GPIO pin as input
void configure_gpio_input(int gpio_number) {
    char gpio_num[10];
    sprintf(gpio_num, "export%d", gpio_number);
    const char* GPIOExport = "/sys/class/gpio/export";
    // Exporting the GPIO to user space
    FILE* fp = fopen(GPIOExport, "w");
    fwrite(gpio_num, sizeof(char), sizeof(gpio_num), fp);
    fclose(fp);
    // Setting GPIO direction as input
    char GPIODirection[40];
    sprintf(GPIODirection, "/sys/class/gpio/gpio%d/direction", gpio_number);
    // Setting GPIO as input
    fp = fopen(GPIODirection, "w");
    fwrite("in", sizeof(char), 2, fp);
    fclose(fp);
}

// Setup input pin in given mode
void config_pin(char* pin_number, char* mode){
    // Creates environment to execute shell command
    if(!vfork()){
         // Execute shell command for pin configuration
         int ret = execl("/usr/bin/config-pin", "config-pin", pin_number, mode, NULL);
         if (ret < 0) {
            printf("Failed to configure pin in PWM mode.\n");
            exit(-1);
         }
    }
}

// Set PWM duty cycle
void set_pwm_duty_cycle(char* pwmchip, char* channel, char* duty_cycle){
    // Export file path
    char PWMDutyCycle[60];
    sprintf(PWMDutyCycle, "/sys/class/pwm/%s/pwm-4:%s/duty_cycle", pwmchip, channel);
    // Configure PWM device
    FILE* fp = fopen(PWMDutyCycle, "w");
    fwrite(duty_cycle, sizeof(char), strlen(duty_cycle), fp);
    fclose(fp);
}

// Set PWM period
void set_pwm_period(char* pwmchip, char* channel, char* period){
    long duty_cycle_int, period_int;
    // Before setting up the period, read old duty cycle
    char PWMDutyCycle[60], duty_cycle_str[20];
    sprintf(PWMDutyCycle, "/sys/class/pwm/%s/pwm-4:%s/duty_cycle", pwmchip, channel);
    FILE* fp = fopen(PWMDutyCycle, "r");
    fscanf(fp, "%ld", &duty_cycle_int);
    fclose(fp);
    period_int = atol(period);
    // If the old duty_cycle value is greater than the new period,
    // update the dummy_duty_cycle first to avoid errors with setting up
    // the period
    if( duty_cycle_int >= period_int){
         duty_cycle_int = period_int/2;
         // Convert long to char data type
         sprintf(duty_cycle_str, "%ld", duty_cycle_int);
         // Setup dummy duty cycle
         set_pwm_duty_cycle(pwmchip, channel, duty_cycle_str);
    }
    // Export file path
    char PWMPeriod[60];
    sprintf(PWMPeriod, "/sys/class/pwm/%s/pwm-4:%s/period", pwmchip, channel);
    fp = fopen(PWMPeriod, "w");
    fwrite(period, sizeof(char), strlen(period), fp);
    fclose(fp);
}

// Starts a PWM
void start_pwm(char* pin_number, char* pwmchip, char* channel, char* period, char* duty_cycle){
    // Configure the pin in PWM mode
    config_pin(pin_number, "pwm");
    // Export PWM device
    FILE* fp;
    char PWMExport[40];
    sprintf(PWMExport, "/sys/class/pwm/%s/export", pwmchip);
    fp = fopen(PWMExport, "w");
    fwrite(channel, sizeof(char), sizeof(channel), fp);
    fclose(fp);
    // Configure PWM Period
    set_pwm_period(pwmchip, channel, period);
    // Configure PWM Duty Cycle
    set_pwm_duty_cycle(pwmchip, channel, duty_cycle);
    // Enable PWM
    char PWMEnable[40];
    sprintf(PWMEnable, "/sys/class/pwm/%s/pwm-4:%s/enable", pwmchip, channel);
    // Configure generating PWM
    fp = fopen(PWMEnable, "w");
    fwrite("1", sizeof(char), 1, fp);
    fclose(fp);
}


// Stops PWM
void stop_pwm(char* pin_number, char* pwmchip, char* channel){
    char PWMDisable[40];
    sprintf(PWMDisable, "/sys/class/pwm/%s/pwm-4:%s/enable", pwmchip, channel);
    // Stop generating PWM
    FILE* fp = fopen(PWMDisable, "w");
    fwrite("0", sizeof(char), 1, fp);
    fclose(fp);
}


int main() {
    // Configure pin P8_8 as input with internal pull-up enabled
    int gpio_number = 67; // button 1
    int gpio_number2 = 69; // button 2

    char pin_number[32] = "P9_16";
    char pwmchip[32] = "pwmchip4";
    char channel[32] = "1";

    char period[32] = "100000000"; // for button 1 (P8_8)
    char duty_cycle[32] = "50000000";
    char period2[32] = "1000000"; // for button 2(p8_9)
    char duty_cycle2[32] = "500000";

    configure_gpio_input(gpio_number);
    configure_gpio_input(gpio_number2);

    char valuePath[40];
    char valuePath2[40];

    sprintf(valuePath, "/sys/class/gpio/gpio%d/value", gpio_number);
    sprintf(valuePath2, "/sys/class/gpio/gpio%d/value", gpio_number2);
    // Wait before first readings to avoid faulty readings
    sleep(1);

    int state;
    int state2;
    FILE *fp;
    FILE *fp2;
    // Loop to monitor events
    while (1) {

        fp = fopen(valuePath, "r");
        fp2 = fopen(valuePath2, "r");
        // Default state is 1 since buttons are configured with
        // internal pull-up resistors. So, reading 0 means button
        // is pressed
        fscanf(fp, "%d", &state);
        fscanf(fp2, "%d", &state2);
        fclose(fp);
        fclose(fp2);
        //event detected
        if (state == 0) {
        printf("Button 1 is pressed.\n");
        stop_pwm(pin_number, pwmchip, channel);  // Make sure the pwm pin and channel are stopped
        start_pwm(pin_number, pwmchip, channel, period, duty_cycle); // start pwm
        }
        else if (state2 == 0){
        printf("Button 2 is pressed.\n");
        stop_pwm(pin_number, pwmchip, channel);  // Make sure the pwm pin and channel are stopped
        start_pwm(pin_number, pwmchip, channel, period2, duty_cycle2); // start pwm
        }
        usleep(100000); // sleep for 100ms debounce
    }
    return 0;
}

