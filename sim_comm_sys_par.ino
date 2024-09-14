/**
 * @file sim_comm_sys_par.ino
 * @brief This code defines an automated system to send data through RGB LED in 
 * 
 * The program waits for a string sent through serial by user, such as:
 *              VGXVRXVWYGRGYRGGYG
 * The string is divided in two, the first half is sent to the right led and the second half to the left led.
 * The program starts the transmission taking into account the times defined.
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

// #define Rr 4  // Pin to right red led
// #define Gr 5  // Pin to right green led
// #define Br 6  // Pin to right blue led
// #define Wr 7  // Pin to right white led
// #define Rl 8  // Pin to left red led
// #define Gl 9  // Pin to left green led
// #define Bl 10 // Pin to left blue led
// #define Wl 11 // Pin to left white led
// #define Z  3  // Dead pin to do nothing
#define TIME_ON_MS 600 // Time to keep a led on in miliseconds
#define PERIOD_MS  1100 // Period (time on + time off) of each led in miliseconds
#define TIME_ON_OFF_MS 50 // Time to blink a led in miliseconds

String lPins[] = {"Rr", "Gr", "Br", "Wr", "Rl", "Gl", "Bl", "Wl", "Z"};
String lPinsx = "RGVW";
uint8_t   pins[]  = {4,  5,  6,  7,  8,  9,  10, 11, 3};

void setup() {

  // Set LED pins to output
  for(char i=0; i<4+4+1; i++)
    pinMode(pins[i], OUTPUT);
  
  // Init the serial port
  Serial.begin(9600);

}

void loop() {

  // Init variable to store user input
  String input;

  // While doing nothing, turn leds off
  for(char i=0; i<4+4+1; i++)
    digitalWrite(pins[i], LOW);

  // Validates if something is in the serial port (user input)
  if(Serial.available() > 0){
    // Read the string sent by the user
    input = Serial.readString();
    // Prints to keep track of the transmission
    Serial.print("Transmission started!. Sending: ");
    Serial.println(input);
    unsigned long startTime = millis();
    // blinkLeds(Br, Bl);
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
  int limit = int((str.length()-1) / 2);
  bool even = (str.length()-1)%2==0;
  if(!even)
    limit++;
  // Loop through the string and turn on the leds
  for(int i=0; i<limit; i++){

    Serial.print("Letter in Right: ");
    Serial.println((String)str[i] + "r");
    Serial.print("Letter in Left: ");
    Serial.println((String)str[i+limit]+"l");

    // Index for right led
    int targetIndexR = lPinsx.indexOf((String)str[i]);
    if((String)str[i] == "X")
      targetIndexR = lPinsx.indexOf("R");
    else if((String)str[i] == "Y")
      targetIndexR = lPinsx.indexOf("G");
    
    // Index for left led
    int targetIndexL = lPinsx.indexOf((String)str[i+limit]);
    if((String)str[i + limit] == "X")
      targetIndexL = lPinsx.indexOf("R");
    else if((String)str[i + limit] == "Y")
      targetIndexL = lPinsx.indexOf("G");
    targetIndexL += 4;

    Serial.print("Pin to turn in Right: ");
    Serial.println(pins[targetIndexR]);
    Serial.print("Pin to turn in Left: ");
    Serial.println(pins[targetIndexL]);

    if(!even && i==(limit-1)){
      if(((String)str[i] == "R" || (String)str[i] == "G" || (String)str[i] == "V" || (String)str[i] == "W")){
        turnLeds(pins[targetIndexR], pins[8]);
      } else if(((String)str[i] == "X" || (String)str[i] == "Y")){
        blinkLeds(pins[targetIndexR], pins[8]);
      }
    } else{
      if(((String)str[i] == "R" || (String)str[i] == "G" || (String)str[i] == "V" || (String)str[i] == "W") &&
        ((String)str[i+limit] == "R" || (String)str[i+limit] == "G" || (String)str[i+limit] == "V" || (String)str[i+limit] == "W")){
        turnLeds(pins[targetIndexR], pins[targetIndexL]);
      } else if(((String)str[i] == "X" || (String)str[i] == "Y") &&
                ((String)str[i+limit] == "X" || (String)str[i+limit] == "Y")){
        blinkLeds(pins[targetIndexR], pins[targetIndexL]);
      } else if(((String)str[i] == "R" || (String)str[i] == "G" || (String)str[i] == "V" || (String)str[i] == "W") &&
                ((String)str[i+limit] == "X" || (String)str[i+limit] == "Y")){
        blinkTurnLeds(pins[targetIndexR], pins[targetIndexL]);
      } else if(((String)str[i] == "X" || (String)str[i] == "Y") &&
                ((String)str[i+limit] == "R" || (String)str[i+limit] == "G" || (String)str[i+limit] == "V" || (String)str[i+limit] == "W")){
        blinkTurnLeds(pins[targetIndexL], pins[targetIndexR]);
      }
    }
  }

}

/**
 * @brief This function turns on two leds for a period of time
 * 
 * @param led1 Pin of the first led
 * @param led2 Pin of the second led
 */
void turnLeds(char led1, char led2){
  digitalWrite(led1, HIGH);       // Turn on the led
  digitalWrite(led2, HIGH);       // Turn on the led
  delay(TIME_ON_MS);             // Wait for the time defined
  digitalWrite(led1, LOW);        // Turn off the led
  digitalWrite(led2, LOW);        // Turn off the led
  delay(PERIOD_MS - TIME_ON_MS); // Wait for the period defined
}

/**
 * @brief This function blinks two leds for a period of time
 * 
 * @param led1 Pin of the first led
 * @param led2 Pin of the second led
 */
void blinkLeds(char led1, char led2){
  // Blink the led 10 times, total time is 10*2*TIME_ON_OFF_MS
  for(char i = 0; i<10; i++){
    digitalWrite(led1, HIGH); // Turn on the led
    digitalWrite(led2, HIGH);
    delay(TIME_ON_OFF_MS);   // Wait for the time defined
    digitalWrite(led1, LOW);  // Turn off the led
    digitalWrite(led2, LOW);
    delay(TIME_ON_OFF_MS);   // Wait for the time defined
  }
  delay(400);   // Wait for the period defined to diferentiate the blinks to the next led on
}

/**
 * @brief This function blinks one led and turns on the other for a period of time
 * 
 * @param blinkLed Pin of the led to blink
 * @param turnLed Pin of the led to turn on
 */
void blinkTurnLeds(char blinkLed, char turnLed){
  // Blink the led 10 times, total time is 10*2*TIME_ON_OFF_MS
  digitalWrite(turnLed, HIGH);
  for(char i = 0; i<10; i++){
    digitalWrite(blinkLed, HIGH); // Turn on the led
    delay(TIME_ON_OFF_MS);   // Wait for the time defined
    digitalWrite(blinkLed, LOW);  // Turn off the led
    delay(TIME_ON_OFF_MS);   // Wait for the time defined
  }
  digitalWrite(turnLed, LOW);
  delay(400);   // Wait for the period defined to diferentiate the blinks to the next led on
}