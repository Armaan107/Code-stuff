
#include <avr/io.h>
#include <util/delay.h>
#include "my_adc_lib.h"
#include "my_uart_lib.h"
#include "i2c.h"
#include "SSD1306.h"
#include <stdlib.h>


// Define the analog pin, the TMP36's Vout pin is connected to
int main(void) {
    unsigned int digitalValue;
    //double analogValue;
    char buffer1[6], buffer2[6]; 
    float TOO_HOT = 85.0;
    
    uart_init();
    adc_init();
    OLED_Init();

    DDRC = 0x00;
    PORTD = 1<<PORTD7;
    DDRD = 1<<DDD2;
    while (1) {
//offer/out67ad9f9c218c1c0c
        // Take an ADC reading and create ASCII strings for digital representation & corresponding analog voltage
        digitalValue = get_adc();                     // The an ADC reading
        float tempC = ((V_REF*1000)/1023.0 * digitalValue - 500) / 10.0;    // Convert digitalValue to temperature in celcius
        float tempF = (tempC*1.8)+32;
        dtostrf(tempF, 4, 1, buffer1);               // Convert the int to a character string in decimal
        dtostrf(tempC, 4, 1, buffer2);          // Convert floating point to a character string
        

        
        if ((PIND & (1 << PIND7)) == 0) {
            // Send the temperature to the serial monitor
            send_string("Temperature:  ");
            send_string(buffer2);
            send_string("C");
            uart_send(13);
            uart_send(10);

            // Display the temperature on the OLED
            OLED_GoToLine(1);
            OLED_DisplayString("Temperature");
            OLED_GoToLine(3);
            OLED_DisplayString(buffer2);
            OLED_DisplayString("C");
            _delay_ms(1000);
            
            }

        else {
            // Send the temperature to the serial monitor
            send_string("Temperature:  ");
            send_string(buffer1);
            send_string("F");
            uart_send(13);
            uart_send(10);

            // Display the temperature  on the OLED
            OLED_GoToLine(1);
            OLED_DisplayString("Temperature");
            OLED_GoToLine(3);
            OLED_DisplayString(buffer1);
            OLED_DisplayString("F");
            _delay_ms(1000);  
            
        }
        if(tempF>TOO_HOT){ 
            PORTD |= (1 << PORTD2); // flash the LED if above threshhold
        }
        else{
            PORTD &= ~(1 << PORTD2);
        }
    }
}
