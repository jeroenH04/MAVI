#include <Keypad.h>                       // Library used for the keypad
#include <Wire.h>                         // Library used for the water level sensor


// Initialize sensor:
#ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
#define SERIAL SerialUSB
#else
#define SERIAL Serial
#endif
 
unsigned char low_data[8] = {0};
unsigned char high_data[12] = {0};
 
#define NO_TOUCH       0xFE
#define THRESHOLD      100
#define ATTINY1_HIGH_ADDR   0x78
#define ATTINY2_LOW_ADDR   0x77


// Initialize keypad
const byte ROWS = 4; 
const byte COLS = 3; 

char hexaKeys[ROWS][COLS] = {             // Set characters according to the correspondig keys
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

byte rowPins[ROWS] = {9, 8, 7, 6};        // Set the pins for the rows
byte colPins[COLS] = {5, 4, 3};           // Set the pins for the columns

char Data[3];                             // Create an array for the input data that is of length 4 
byte data_count = 0;                      // Set the data_count to 0

int Value;                                //Int value for the input Data
int m_Value;                              //Measured value by the sensor

int BUZZER_PIN = 2;                       // Digital output pin of speaker

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 


// Code for sensor
void getHigh12SectionValue(void)
{
  memset(high_data, 0, sizeof(high_data));
  Wire.requestFrom(ATTINY1_HIGH_ADDR, 12);
  while (12 != Wire.available());
 
  for (int i = 0; i < 12; i++) {
    high_data[i] = Wire.read();
  }
  delay(10);
}
 
void getLow8SectionValue(void)
{
  memset(low_data, 0, sizeof(low_data));
  Wire.requestFrom(ATTINY2_LOW_ADDR, 8);
  while (8 != Wire.available());
 
  for (int i = 0; i < 8 ; i++) {
    low_data[i] = Wire.read();            // Receive a byte as character
  }
  delay(10);
}

void check()
{
  int sensorvalue_min = 250;
  int sensorvalue_max = 255;
  int low_count = 0;
  int high_count = 0;
  
  int m_Value = 0;
  int off_Value = 0;

   
  while (off_Value < 1 )                   // While measued value is less than input Value, loop the sensor code
  {char customKey = customKeypad.getKey(); 
    uint32_t touch_val = 0;
    uint8_t trig_section = 0;
    low_count = 0;
    high_count = 0;
    getLow8SectionValue();
    getHigh12SectionValue();
 
    Serial.println("low 8 sections value = ");
    for (int i = 0; i < 8; i++)
    {
      Serial.print(low_data[i]);
      Serial.print(".");
      if (low_data[i] >= sensorvalue_min && low_data[i] <= sensorvalue_max)
      {
        low_count++;
      }
      if (low_count == 8)
      {
        Serial.print("      ");
        Serial.print("PASS");
      }
    }
    Serial.println("  ");
    Serial.println("  ");
    Serial.println("high 12 sections value = ");
    for (int i = 0; i < 12; i++)
    {
      Serial.print(high_data[i]);
      Serial.print(".");
 
      if (high_data[i] >= sensorvalue_min && high_data[i] <= sensorvalue_max)
      {
        high_count++;
      }
      if (high_count == 20)
      {
        Serial.print("      ");
        Serial.print("PASS");
      }
    }
 
    Serial.println("  ");
    Serial.println("  ");
 
    for (int i = 0 ; i < 8; i++) {
      if (low_data[i] > THRESHOLD) {
        touch_val |= 1 << i;
 
      }
    }
    for (int i = 0 ; i < 12; i++) {
      if (high_data[i] > THRESHOLD) {
        touch_val |= (uint32_t)1 << (8 + i);
      }
    }
 
    while (touch_val & 0x01)
    {
      trig_section++;
      touch_val >>= 1;
    }
    //SERIAL.print("water level = ");
    //SERIAL.print(trig_section * 5);
    //SERIAL.println("% ");
    //SERIAL.println(" ");
    SERIAL.println("*********************************************************");
    SERIAL.println(trig_section);
    //SERIAL.println(trig_section*5*0.01*350);
    
    int m_Value = trig_section*5*0.01*1000;    // m_Value is an integer of the percentage measured liquid in mL.
    alarm(m_Value, Value, trig_section);
    
    if(m_Value >= (Value - 10)){           //If-statement to turn off the loop, if the measured value is more than/equal to the input value, off_Value = 1 
      off_Value++;
      tone(BUZZER_PIN, 3136, 3000);  //Alarm sound 
      Serial.println("Measured value reached the input value, turning off sensor loop.");
    }
    if (customKey) {
      tone(BUZZER_PIN, 2400, 100);
      Serial.println("Sensor loop will be turning off.");
      off_Value++;
    }
     digitalWrite(BUZZER_PIN, LOW);  //Turn off the buzzer
   
    delay(100);
    
  }
}

void alarm(int mV, int V, uint8_t trig){
    Serial.println("Measued Value = ");
    Serial.println(mV);
    Serial.println("Input Value = ");
    Serial.println(V);

    if (mV < V){
      if( (0.5*V-30)<= mV & mV <= (0.5*V+30)){
      Serial.println ("Meassured Value has reached 50% of input value.");
      tone(BUZZER_PIN, 2800, 100);
        } else if( (0.75*V-50)<= mV & mV <= (0.75*V+50)){
            Serial.println ("Meassured Value has reached 75% of input value.");
            tone(BUZZER_PIN, 2800, 100);
            delay (500);
            tone(BUZZER_PIN, 2800, 100); 
        } else if ((0.90*V-50)<= mV & mV <= (0.90*V+25)) {
            Serial.println ("Meassured Value has reached 95% of input value.");
            tone(BUZZER_PIN, 2800, 100);
            delay (250);
            tone(BUZZER_PIN, 2800, 100); 
            delay (250);
            tone(BUZZER_PIN, 2800, 100); 
        } if ((0.95*V-25)<= mV & mV <= (0.95*V+50)){
            Serial.println ("Meassured Value has reached 95% of input value.");
            tone(BUZZER_PIN, 2800, 100);
            delay (125);
            tone(BUZZER_PIN, 2800, 100); 
            delay (125);
            tone(BUZZER_PIN, 2800, 100);
            delay (125);
            tone(BUZZER_PIN, 2800, 100); 
        }
       else{
        digitalWrite(BUZZER_PIN, LOW);
       }
    }   
}
/* ------------------------ */


void clearData(){
  while(data_count !=0){                  // As long as there is still data in the array
    Data[data_count--] = 0;               // Delete the data
  }
  return;
}

void setup(){
  Serial.begin(9600);
  pinMode(BUZZER_PIN, OUTPUT);            // Initialize buzzer 
  pinMode(3, OUTPUT);                     // Sensor
  Wire.begin();                           // Begin sensor
}
  
void loop(){                                                                                      // NOTE FOR KEYPAD INPUT: '*' and '#' are temporarliy swapped due to the deffective keypad
  /* Get keypad input */
  char customKey = customKeypad.getKey();
    
  if (customKey){
    Serial.println(customKey);            // Print number to Serial Monitor
    if (customKey == '#') {               // User pressed * to enter the data
      Serial.println(Data);
      tone(BUZZER_PIN, 2400, 100);        // Turn on buzzer
      Value = atoi(Data);                 // Convert the data array to an int called Value
      check();                            // Pass the data into the check function of the sensor
      clearData();                        // Clear the data
    } else if (customKey == '*') {        // User pressed # to delete the last entry
      Data[data_count] = 0;               // Delete the last entry
      data_count --;                      // length of number decreases by 1
      tone(BUZZER_PIN, 2100, 100);        // Turn on buzzer
    } else { 
      Data[data_count] = customKey;       // Save entry on the array
      data_count++;                       // Increase the length of the number
      tone(BUZZER_PIN, 4000, 100);        // Turn on buzzer
    }
  } else {
    digitalWrite(BUZZER_PIN, LOW);        // Turn off buzzer
  }
}
