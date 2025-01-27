

#include <avr/io.h>
#include <util/delay.h>
#define TRIG PB1    //PB1 = pin 15
#define ECHO PB0    //PB0 = pin 14
#define RANGE_PER_CLOCK 1.098
#include <string.h>  // Defines strlen() function
#include <stdlib.h>  // Defines itoa() function
#define persistence 1
void timer0_init(void);

int main(void) {
    unsigned char rising_edge_clocks, falling_edge_clocks, echo_width_clocks;
    unsigned char ledDigits[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x67};
    unsigned char DIG1, DIG2, DIG3, DIG4;
    
    float target_range;
    DDRB = 1<<TRIG;         // TRIG is output pin;
    PORTB &= ~(1<<TRIG);    // Set the TRIG pin low
    timer0_init();          // Initialize timer0

    while(1){
        TCNT0 = 0;          // Load counter with 0
        PORTB |= 1<<TRIG;   // These three lines of code
        _delay_us(10);      // Put a 10 usec pulse on the
        PORTB &= ~(1<<TRIG);// TRIG pin.

        // Wait till the ECHO pulse goes high
        while ((PINB & (1<<ECHO)) ==0);
        rising_edge_clocks = TCNT0; // Note the time
        // Now wait till the ECHO pulse goes low
        while (!(PINB & (1<<ECHO))==0);
        falling_edge_clocks = TCNT0;
    
        if (falling_edge_clocks > rising_edge_clocks){
            // Compute target range and send it to the serial monitor
            echo_width_clocks = falling_edge_clocks - rising_edge_clocks;
            target_range = echo_width_clocks * RANGE_PER_CLOCK;
            if(target_range<200 && target_range>5){
                DDRD = 0xFF;
                DDRB |= 0b11111100;
                PORTB |= 0b11111100; 

                DIG4 = (int)target_range % 10;
                PORTD = ledDigits[DIG4];
                PORTB = ~(1 << 2);
                _delay_ms(persistence);

                DIG3 = (int)(target_range / 10) % 10;
                PORTD = ledDigits[DIG3];
                PORTB = ~(1 << 3);
                _delay_ms(persistence);

                DIG2 = (int)(target_range / 100) % 10;
                PORTD = ledDigits[DIG2];
                PORTB = ~(1 << 4);
                _delay_ms(persistence);

                DIG1 = (int)(target_range / 1000) % 10;
                PORTD = ledDigits[DIG1];
                PORTB = ~(1 << 5);
                _delay_ms(persistence);
            
            }
            
        _delay_us(1);
        }
       
    }
}


// Initialize timer0: normal mode (count up), divide clock by 1024
void timer0_init(){
    TCCR0A = 0;         // Timer 1 Normal mode (count up)
    TCCR0B = 5;         // Divide clock by 1024
    TCNT0=0;            // Start the timer at 0

}


// #include <avr/io.h>
// #include <util/delay.h>
// #define persistence 5



// int main(void) {
//     unsigned char ledDigits[] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x67};
//     unsigned char i=100;
//     unsigned char DIG1,DIG2,DIG3,DIG4;
//     DDRD = 0xFF;
//     DDRB = 0b11111100;
//     PORTB = 0b11111100;
//     while (1) {
//         i++;
//         if(i>9999) i=100;

//         DIG4 = i%10;
//         PORTD = ledDigits[DIG4];
//         PORTB = ~(1<<2); 
//         _delay_ms(persistence);

//         DIG3 = (i/10)%10;
//         PORTD = ledDigits[DIG3];
//         PORTB = ~(1<<3); 
//         _delay_ms(persistence);

//         DIG2 = (i/100)%10;
//         PORTD = ledDigits[DIG2];
//         PORTB = ~(1<<4); 
//         _delay_ms(persistence);

//         DIG1 = (i/1000)%10;
//         PORTD = ledDigits[DIG1];
//         PORTB = ~(1<<5); 
//         _delay_ms(persistence);
            
//     }
// }

// #define TRIG PB1    //PB1 = pin 15
// #define ECHO PB0    //PB0 = pin 14
// #define RANGE_PER_CLOCK 1.098
// #include <avr/io.h>
// #include <util/delay.h>
// #include <string.h>  // Defines strlen() function
// #include <stdlib.h>  // Defines itoa() function
// void uart_init(void);
// void uart_send(unsigned char);
// void send_string(char *stringAddress);
// void timer0_init(void);
// void send_to_monitor(unsigned char, unsigned char, float);

// int main(void){
//     unsigned char rising_edge_clocks, falling_edge_clocks, 
//                                         echo_width_clocks;
//     float target_range;
//     DDRB = 1<<TRIG;         // TRIG is output pin;
//     PORTB &= ~(1<<TRIG);    // Set the TRIG pin low
//     uart_init();
//     timer0_init();          // Initialize timer0

//     while(1){
//         TCNT0 = 0;          // Load counter with 0
//         PORTB |= 1<<TRIG;   // These three lines of code
//         _delay_us(10);      // Put a 10 usec pulse on the
//         PORTB &= ~(1<<TRIG);// TRIG pin.

//         // Wait till the ECHO pulse goes high
//         while ((PINB & (1<<ECHO)) ==0);
//         rising_edge_clocks = TCNT0; // Note the time
//         // Now wait till the ECHO pulse goes low
//         while (!(PINB & (1<<ECHO))==0);
//         falling_edge_clocks = TCNT0;
        
//         if (falling_edge_clocks > rising_edge_clocks){
//             // Compute target range and send it to the serial monitor
//             echo_width_clocks = falling_edge_clocks - rising_edge_clocks;
//             target_range = echo_width_clocks * RANGE_PER_CLOCK;
//             send_to_monitor(rising_edge_clocks, falling_edge_clocks, target_range);
//         }
//        _delay_ms(500);  // Delay then go again
//     }
// }

// // Send info to the serial monitor via the UART
// void send_to_monitor(unsigned char t1, unsigned char t2, float range){
//         char buffer[10];
//         send_string("Rising edge: ");
//         utoa(t1, buffer, 10);   //send the delay count to comm port
//         send_string(buffer);
//         //send_string(" clocks; ");
//         send_string(" Falling edge: ");
//         utoa(t2, buffer, 10);   //send the delay count to comm port
//         send_string(buffer);
//         //send_string(" clocks; ");
    
//         send_string(" Echo pulse width: ");
//         utoa(t2-t1, buffer, 10);
//         send_string(buffer);
//         send_string(" clock pulses.     Target Range = ");
//         dtostrf(range, 3, 0, buffer);
//         send_string(buffer);
//         send_string(" cm "); //PROBLEM?
//         dtostrf(range/2.54, 3,0,buffer); //send the delay count to comm port
//         send_string(buffer);
//         send_string(" inch");
//         uart_send(13); //tx carriage return
//         uart_send(10); //tx new line
// }

// // Initialize timer0: normal mode (count up), divide clock by 1024
// void timer0_init(){
//     TCCR0A = 0;         // Timer 1 Normal mode (count up)
//     TCCR0B = 5;         // Divide clock by 1024
//     TCNT0=0;            // Start the timer at 0

// }

// // Send a string, char by char, to UART via uart_send()
// // Input is pointer to the string to be sent
// void send_string(char *stringAddress){
//     for (unsigned char i = 0; i < strlen(stringAddress); i++)
//         uart_send(stringAddress[i]);
// }

// // Initialize the UART
// void uart_init(void){
//     UCSR0B = (1 << TXEN0); //enable the UART transmitter
//     UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); //set 8 bit character size
//     UBRR0L = 103; //set baud rate to 9600 for 16 MHz crystal
// }

// // Sent a single character to serial monitor via UART
// void uart_send(unsigned char ch){
//     while (!(UCSR0A & (1 << UDRE0))); //wait til tx data buffer empty
//     UDR0 = ch; //write the character to the USART data register
// }