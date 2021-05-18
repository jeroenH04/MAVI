#include <Keypad.h>

/* Keypad rows and columns */
const byte ROWS = 4; 
const byte COLS = 3; 

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

byte rowPins[ROWS] = {9, 8, 7, 6}; 
byte colPins[COLS] = {5, 4, 3}; 

// Digital output pin of speaker
int BUZZER_PIN = 2;


Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

void setup(){
  Serial.begin(9600);
  pinMode(BUZZER_PIN, OUTPUT); // Initialize buzzer 
}
  
void loop(){
  /* Get keypad input */
  char customKey = customKeypad.getKey();
  
  if (customKey){
    Serial.println(customKey); // Print number to Serial Monitor
    tone(BUZZER_PIN, 1400, 100); // Turn on buzzer
    
  } else {
    digitalWrite(BUZZER_PIN, LOW); // Turn off buzzer
  }
}
