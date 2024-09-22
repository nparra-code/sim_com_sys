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
#define DIV        0.665
#define TIME_ON_OFF_MS 40 // Time to blink a led in miliseconds

String lPins[] = {"Rr", "Gr", "Br", "Wr", "Rl", "Gl", "Bl", "Wl", "Z"};
String lPinsx  = "RGVW";
uint8_t pins[] = {4,  5,  6,  7,  8,  9,  10, 11, 3};

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
    // input = findYY(input);
    input = partirCadena(input);
    
    // Prints to keep track of the transmission
    Serial.print("Transmission started!. Sending: ");
    Serial.println(input);
    unsigned long startTime = millis();
    blinkLeds(6, 10);
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
  int limit = str.indexOf(",");

  // Loop through the string and turn on the leds
  for(int i=0; i<limit; i++){

    Serial.println("------------------------------");

    Serial.print("Letter in Right: ");
    Serial.println((String)str[i] + "r");
    if(i+limit+1 <= (str.length()-1)){
      Serial.print("Letter in Left: ");
      Serial.println((String)str[i+limit+1]+"l");
    }
    
    // Index for right led
    int targetIndexR = lPinsx.indexOf((String)str[i]);
    if((String)str[i] == "X")
      targetIndexR = lPinsx.indexOf("R");
    else if((String)str[i] == "Y")
      targetIndexR = lPinsx.indexOf("G");
    
    // Index for left led
    int targetIndexL;
    if(i+limit+1 <= (str.length())){
      targetIndexL = lPinsx.indexOf((String)str[i+limit+1]);
      if((String)str[i + limit+1] == "X")
        targetIndexL = lPinsx.indexOf("R");
      else if((String)str[i + limit+1] == "Y")
        targetIndexL = lPinsx.indexOf("G");
      targetIndexL += 4;
    }

    Serial.print("Pin to turn in Right: ");
    Serial.println(pins[targetIndexR]);
    Serial.print("Pin to turn in Left: ");
    Serial.println(pins[targetIndexL]);

    if(i+limit+1 > (str.length()-1)){
      Serial.println("Only turning right LED.");
      if(((String)str[i] == "R" || (String)str[i] == "G" || (String)str[i] == "V" || (String)str[i] == "W")){
        turnLeds(pins[targetIndexR], pins[8]);
      } else if(((String)str[i] == "X" || (String)str[i] == "Y")){
        blinkLeds(pins[targetIndexR], pins[8]);
      }
    } else{
      if(((String)str[i] == "R" || (String)str[i] == "G" || (String)str[i] == "V" || (String)str[i] == "W") &&
        ((String)str[i+limit+1] == "R" || (String)str[i+limit+1] == "G" || (String)str[i+limit+1] == "V" || (String)str[i+limit+1] == "W")){
        turnLeds(pins[targetIndexR], pins[targetIndexL]);
      } else if(((String)str[i] == "X" || (String)str[i] == "Y") &&
                ((String)str[i+limit+1] == "X" || (String)str[i+limit+1] == "Y")){
        blinkLeds(pins[targetIndexR], pins[targetIndexL]);
      } else if(((String)str[i] == "R" || (String)str[i] == "G" || (String)str[i] == "V" || (String)str[i] == "W") &&
                ((String)str[i+limit+1] == "X" || (String)str[i+limit+1] == "Y")){
        blinkTurnLeds(pins[targetIndexL], pins[targetIndexR]);
      } else if(((String)str[i] == "X" || (String)str[i] == "Y") &&
                ((String)str[i+limit+1] == "R" || (String)str[i+limit+1] == "G" || (String)str[i+limit+1] == "V" || (String)str[i+limit+1] == "W")){
        blinkTurnLeds(pins[targetIndexR], pins[targetIndexL]);
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
  delay(TIME_ON_MS*DIV);             // Wait for the time defined
  digitalWrite(led1, LOW);        // Turn off the led
  digitalWrite(led2, LOW);        // Turn off the led
  delay((PERIOD_MS - TIME_ON_MS)*DIV); // Wait for the period defined
}

/**
 * @brief This function blinks two leds for a period of time
 * 
 * @param led1 Pin of the first led
 * @param led2 Pin of the second led
 */
void blinkLeds(char led1, char led2){
  // Blink the led 10 times, total time is 10*2*TIME_ON_OFF_MS
  for(char i = 0; i<8; i++){
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
  for(char i = 0; i<8; i++){
    digitalWrite(blinkLed, HIGH); // Turn on the led
    delay(TIME_ON_OFF_MS);   // Wait for the time defined
    digitalWrite(blinkLed, LOW);  // Turn off the led
    delay(TIME_ON_OFF_MS);   // Wait for the time defined
  }
  digitalWrite(turnLed, LOW);
  delay(400);   // Wait for the period defined to diferentiate the blinks to the next led on
}

String findYY(String inStr){
  String str = inStr;
  int index = str.indexOf("YY");

  String str1 = str.substring(0, index);
  String str2 = str.substring(index+2);

  if(str1.length() >= str2.length())
    str = str1 + "," + str2;
  else if(str2.length() > str1.length())
    str = str2 + "," + str1;

  return str;
}

String findHalf(String str){
  int idx;
  int len = str.length()-1;
  Serial.println(len);
  idx = int(len/2);
  Serial.println(idx);
  char rightIdx, leftIdx;
  String str1, str2;
  if(str[idx]=="Y" || str[idx]=="X"){
    str1 = str.substring(0, idx);
    str2 = str.substring(idx+1);
  } else{
    for(char i=idx; i<len-idx; i++){
      Serial.println(str[idx+i]);
      if(str[idx+i]=="Y" || str[idx+i]=="X"){
        rightIdx = i;
        break;
      } else {
        rightIdx = -1;
      }
    }
    Serial.println(rightIdx);
    for(char i=idx; i<len-idx; i++){
      if(str[idx-i]=="Y" || str[idx-i]=="X"){
        leftIdx = i;
        break;
      } else {
        leftIdx = -1;
      }
    }
    Serial.println(leftIdx);
    if(rightIdx < leftIdx){
      str1 = str.substring(0, idx+rightIdx);
      str2 = str.substring(idx+rightIdx+1);
    } else if(leftIdx < rightIdx){
      str1 = str.substring(idx+leftIdx+1);
      str2 = str.substring(0, idx+leftIdx);
    }
  }
  String strAux = str1 + "," + str2;
  return strAux;
}

String partirCadena(String cadena) {
  int mitad = cadena.length() / 2;
  int indiceY = -1;

  // Verificar si hay una 'Y' en la mitad
  if (cadena.charAt(mitad) == 'Y') {
    indiceY = mitad;
  } else {
    // Buscar la 'Y' más cercana a la mitad
    int indiceYIzquierda = cadena.lastIndexOf('Y', mitad);
    int indiceYDerecha = cadena.indexOf('Y', mitad);

    // Determinar cuál 'Y' está más cerca de la mitad
    if (indiceYIzquierda != -1 && indiceYDerecha != -1) {
      indiceY = (mitad - indiceYIzquierda <= indiceYDerecha - mitad) ? indiceYIzquierda : indiceYDerecha;
    } else if (indiceYIzquierda != -1) {
      indiceY = indiceYIzquierda;
    } else if (indiceYDerecha != -1) {
      indiceY = indiceYDerecha;
    } else {
      // Si no se encuentra ninguna 'Y', usar la mitad
      indiceY = mitad;
    }
  }

  // Retornar las dos mitades separadas por una coma
  if(cadena.substring(0, indiceY).length() >= cadena.substring(indiceY+1).length())
    return cadena.substring(0, indiceY) + "," + cadena.substring(indiceY+1, cadena.length()-1);
  else
    return cadena.substring(indiceY+1, cadena.length()-1) + "," + cadena.substring(0, indiceY);
}