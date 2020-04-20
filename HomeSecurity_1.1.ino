//ALARM - PROJECT

//Include all libraries
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <NewPing.h>
#include <Keypad.h>

//Keypad configuration + define pins of rows & columns
const byte rows = 4; //four rows
const byte columns = 4; //three columns
char keys[rows][columns] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte row_pins[rows] = {2, 3, 4, 5};
byte column_pins[columns] = {6, 7, 8, 9};

//Define sonar input pins
#define echo_pin 13
#define trig_pin 12
#define max_range 450

//Define led pins
#define red_led A1
#define green_led A0
//#define led 11

//Define buzzer pin
#define buzz_pin 10

//Define some variables
int distance = 0;
byte last_state = 0;
int attempt = 0;
//Keypad vars
char pressed;
String output;


//Initialize libraries
NewPing sonar(trig_pin, echo_pin, max_range);
LiquidCrystal_I2C lcd(0x27, 16, 2);
Keypad keypad = Keypad( makeKeymap(keys), row_pins, column_pins, rows, columns);

//Set the first pin
String pin = "3435";



/////SETUP/////
void setup() {

  //Define I/O pins
  pinMode(red_led, OUTPUT);
  pinMode(green_led, OUTPUT);
  pinMode(buzz_pin, OUTPUT);
  //pinMode(led, OUTPUT);

  //Begin Serial and lcd communication
  lcd.begin();
  Serial.begin(9600);
}



/////MAIN LOOP/////
void loop() {
  //Turn off leds
  digitalWrite(green_led, LOW);
  digitalWrite(red_led, LOW);
  //Print main options
  lcd.setCursor(1, 0);
  lcd.print("A - Alarm On");
  lcd.setCursor(1, 1);
  lcd.print("B - Change PIN");
  //Get the pressed key
  pressed = keypad.waitForKey();
  
  ///ALARM ON/// - Option A
  if (pressed == 'A') {
    lcd.clear(); 
    //Scroll the text - visual effect 
    for (int i=16; i>-3; i--) {
      lcd.setCursor(i, 0);
      lcd.print("Alarm in 10 secs");
      delay(200);
      }
    delay(500);
    lcd.clear();  
    //Countdown 10 seconds
    for (int i=9; i>=0; i--) {
      lcd.setCursor(8, 0);
      lcd.print(i);
      digitalWrite(green_led, HIGH);
      delay(200);
      digitalWrite(green_led, LOW);
      delay(800);
      }
    //Print the label "ALARM ON"
    lcd.setCursor(4,0);
    lcd.print("ALARM ON");
    digitalWrite(green_led, HIGH);
    //Execute next lines, if the ultrasonic sensor is receiving data
    distance = sonar.ping_cm();
    if (distance > 0) {
      Serial.println("measured");
      measure:
      //Clear the variable with value of distance    
      distance = 0;
      ///Measure 4 times for better evaluation
      for (int i=0; i<4; i++) {
        distance += sonar.ping_cm();
        delay(50);
        }
      distance /= 4;
      Serial.println(distance);
 
      //Opened door
      if (distance > 42) {
        digitalWrite(green_led, LOW);
        digitalWrite(red_led, HIGH);
        tone(buzz_pin, 1300);
        delay(1000);
        
        check_pin:
        lcd.setCursor(4,0);
        lcd.print("  PIN:   ");
        ////Check the pin////
        //Check the typed PIN - 4 cycles - 4 numbers
        pressed = keypad.waitForKey();
        if (pressed == '*') {
          for (byte i=6; i<10; i++) {
          //Wait for key press
            pressed = keypad.waitForKey();
            Serial.println(pressed);
            //If the key was pressed, add it to the valiable "output"
            if (pressed != NO_KEY) {
              output += pressed;
              lcd.setCursor(i, 1);
              lcd.print("*");   
              }
            }
          //If the typed PIN is correct
          //Then turn off the buzzer
          if (output == pin) {
            digitalWrite(red_led, LOW);
            digitalWrite(green_led, HIGH);
            lcd.setCursor(4, 1);
            lcd.print("CORRECT!");
            delay(200);
            digitalWrite(green_led, LOW);
            delay(100);
            digitalWrite(green_led, HIGH);
            delay(200);
            noTone(buzz_pin);
            output = "";
            lcd.clear();
            }
          //Otherwise do not accept the typed PIN
          else {
            digitalWrite(green_led, LOW);
            digitalWrite(red_led, HIGH);
            lcd.setCursor(5, 1);
            lcd.print("WRONG!");
            delay(100);
            digitalWrite(red_led, LOW);
            delay(100);
            digitalWrite(red_led, HIGH);
            delay(100);
            digitalWrite(red_led, LOW);
            delay(100);
            digitalWrite(red_led, HIGH);
            output = "";
            lcd.clear();
            //Go back and ask for PIN again
            goto check_pin;
            }
          }
        //Go back, if the "*" key was not pushed as first
        else {
          goto check_pin;
          }
        }
      //If door are closed
      else if(distance <= 42 ) {
        delay(50);
        goto measure;
        }
      }
    //End Option A
    }
    
  ///Change the PIN/// - Option B
  else if (pressed == 'B') {
    lcd.clear();
    lcd.setCursor(4, 0);
    lcd.print("OLD PIN:");
    output = "";
    //Check the typed current PIN - 4 cycles - 4 numbers
    for (int i=6; i<10; i++) {
      //Wait for key press
      pressed = keypad.waitForKey();
      if (pressed != NO_KEY) {
        lcd.setCursor(i, 1);
        lcd.print("*");
        //Add key to the variable
        output += pressed;
        }
      }
    //Check if typed PIN is the same as current
    if (output == pin) {
      pin = "";
      lcd.setCursor(4, 0);
      lcd.print("NEW PIN:");
      lcd.setCursor(6, 1);
      //Clear second row
      lcd.print("    ");
      for (int i=6; i<10; i++) {
        //Wait for key press
        pressed = keypad.waitForKey();
        if (pressed != NO_KEY) {
          lcd.setCursor(i, 1);
          lcd.print("*");
          //Add pressed key value to the variable
          pin += pressed;
          }
        }
      }
    //If typed pin not same as the current
    //Execute next lines
    else {
      lcd.setCursor(3, 1);
      lcd.print("WRONG PIN!");
      delay(750);
      }
    //End option B
    }
//End the main loop
}
       
    
   
