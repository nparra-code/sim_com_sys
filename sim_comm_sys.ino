/**
 * @file sim_comm_sys.ino
 * @brief This code defines an automated system to send data through RGB LED.
 * 
 * The program waits for a string sent through serial by user, such as:
 *              VGXVRXVWYGRGYRGGYG
 * and starts the transmission taking into account the times defined.
 * 
 * R -> red
 * G -> green
 * V -> blue
 * W -> white
 * X -> red led blink
 * Y -> green led blink
 * 
 * @author  Nelson F. Parra G.
 * @version 1.0
 * @date    28-08-2024
 */


#include <Arduino.h>

#define R 4 // Pin to red led
#define G 5 // Pin to green led
#define B 6 // Pin to blue led
#define W 7 // Pin to white led
#define TIME_ON_MS 600 // Time to keep a led on in miliseconds
#define PERIOD_MS  1100 // Period (time on + time off) of each led in miliseconds
#define TIME_ON_OFF_MS 50 // Time to blink a led in miliseconds

void setup() {

  // Set LED pins to output
  pinMode(R, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(W, OUTPUT);
  
  // Init the serial port
  Serial.begin(9600);

}

void loop() {

  // Init variable to store user input
  String input;

  // While doing nothing, turn leds off
  digitalWrite(R, LOW);
  digitalWrite(G, LOW);
  digitalWrite(B, LOW);
  digitalWrite(W, LOW);

  // Validates if something is in the serial port (user input)
  if(Serial.available() > 0){
    // Read the string sent by the user
    input = Serial.readString();
    // Prints to keep track of the transmission
    Serial.print("Transmission started!. Sending: ");
    Serial.println(input);
    unsigned long startTime = millis();
    blinkLed(B);
    delay(500);
    // Invoke sendData() function that controls the leds
    sendData(input);
    // Print to notify that transmission ended
    Serial.print("Transmission done! Took: ");
    Serial.print((millis() - startTime)/1000);
    Serial.println(" s.");
  }
}

/**
 * @brief This function controls the leds to send the data
 * 
 * @param str String to be sent
 */
void sendData(String str){
  // Loop through the string and turn on the leds
  for(int i=0; i<str.length(); i++){
    if(str[i]=='R')
      turnLed(R); // Turn on red led if 'R' is found
    else if(str[i]=='G')
      turnLed(G); // Turn on green led if 'G' is found
    else if(str[i]=='V')
      turnLed(B); // Turn on blue led if 'V' is found
    else if(str[i]=='W')
      turnLed(W); // Turn on white led if 'W' is found
    else if(str[i]=='X')
      blinkLed(R); // Blink red led if 'X' is found
    else if(str[i]=='Y')
      blinkLed(G); // Blink green led if 'Y' is found
  }

}

/**
 * @brief This function turns a led on for a period of time
 * 
 * @param led Pin of the led to be turned on
 */
void turnLed(char led){
  digitalWrite(led, HIGH);       // Turn on the led
  delay(TIME_ON_MS);             // Wait for the time defined
  digitalWrite(led, LOW);        // Turn off the led
  delay(PERIOD_MS - TIME_ON_MS); // Wait for the period defined
}

/**
 * @brief This function blinks a led for a period of time
 * 
 * @param led Pin of the led to be blinked
 */
void blinkLed(char led){
  // Blink the led 10 times, total time is 10*2*TIME_ON_OFF_MS
  for(char i = 0; i<10; i++){
    digitalWrite(led, HIGH); // Turn on the led
    delay(TIME_ON_OFF_MS);   // Wait for the time defined
    digitalWrite(led, LOW);  // Turn off the led
    delay(TIME_ON_OFF_MS);   // Wait for the time defined
  }
  delay(400);   // Wait for the period defined to diferentiate the blinks to the next led on
}