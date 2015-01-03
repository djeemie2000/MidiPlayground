/*
Arduino template for Qt
Downloaded from http://www.lucidarme.me

In the menu Tools>Option set in the Terminal field : xterm -e

Set the following parameters in the project tab :
Build directory : add /src at the end of the path
Build steps : make all
Clean steps : make clean


*/


// Access to the Arduino Libraries
#include <Arduino.h>

/*
  Turns on an LED on for one second, then off for one second, repeatedly.
*/

// Define the pin where the led is connected
#define     LED_PIN     13
#define     DELAY_MS    1000


int main()
{
    // Initialize Arduino Librairies
    init();

    Serial.begin(115200);
    Serial.println("http://www.lucidarme.me \nHello world !");

    // Set the led pin as output
    pinMode(LED_PIN, OUTPUT);

    while (1)
    {
        // Set the LED on
        digitalWrite(LED_PIN, HIGH);
        // Wait for a second
        delay(DELAY_MS);
        // Set the LED off
        digitalWrite(LED_PIN, LOW);
        // Wait for a second
        delay(DELAY_MS);
    }

}


