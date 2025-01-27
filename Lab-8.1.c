
#include <avr/io.h>
#include <util/delay.h>

int main(){
    DDRD = 1<<DDD2 | 1<<DDD3; // I set PD2 and PD3 as output for LEDs
    PORTD = 1<<PORTD5 | 1<<PORTD6 | 1<<PORTD7; // Enable internal pull-up resistors for PD5, PD6, and PD7

    while(1) {
        // Check if multiple buttons are pressed simultaneously
        if (((PIND & (1 << PIND5)) == 0 && (PIND & (1 << PIND6)) == 0) || 
            ((PIND & (1 << PIND5)) == 0 && (PIND & (1 << PIND7)) == 0) || 
            ((PIND & (1 << PIND6)) == 0 && (PIND & (1 << PIND7)) == 0)) {
            PORTD &= ~((1 << PORTD2) | (1 << PORTD3)); // Turn off both LEDs
        } 
        else {
            // Check if PD5 is pressed
            if ((PIND & (1 << PIND5)) == 0) {
                _delay_ms(100); // to achieve 5 blinks in a second ((1/5)/2) = 0.1 sec
                PORTD ^= (1 << PORTD2); // Toggle PD2 (red LED)
            } 
            // Check if PD6 is pressed
            else if ((PIND & (1 << PIND6)) == 0) {
                _delay_ms(100); 
                PORTD ^= (1 << PORTD3); // Toggle PD3 (green LED)
            } 
            // Check if PD7 is pressed
            else if ((PIND & (1 << PIND7)) == 0) {
                _delay_ms(100); 
                PORTD ^= (1 << PORTD2) | (1 << PORTD3); // Toggle both LEDs
            } 
            else {
                PORTD &= ~((1 << PORTD2) | (1 << PORTD3)); // Turn off both LEDs if no button is pressed
            }
        }
    }
    return 0; // This line is theoretically unreachable because the program is designed to run indefinitely in the loop
}

