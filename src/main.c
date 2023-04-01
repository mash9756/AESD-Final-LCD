/**
 *  @file       main.c
 *  @author     Mark Sherman
 * 
 *  @brief      main loop
 *  @date       4/1/23
 *  @version    1.0
*/

#include "LCD.h"
const int ledPin = 21;

int main(){
    wiringPiSetupGpio(); // Initialise WiringPi with Broadcom GPIO pins

    pinMode(ledPin, OUTPUT); // Set LED Pin as an output
    pinMode(RS,    OUTPUT);
    pinMode(E,     OUTPUT);
    pinMode(D4,    OUTPUT);
    pinMode(D5,    OUTPUT);
    pinMode(D6,    OUTPUT);
    pinMode(D7,    OUTPUT);

    printf("\nHello World!\n");

    // Turn LED On
    printf("LED On\n");
    digitalWrite(ledPin, HIGH);

    delay(1000);

    // Turn LED off
    printf("LED Off\n");
    digitalWrite(ledPin, LOW);

    delay(1000);

    LCD_init();

    LCD_test("It's me, Caralyn");
    return 0;
}