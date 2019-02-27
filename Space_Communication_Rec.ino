#include <Wire.h>
//#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <RF24.h>

RF24 radio(8, 10); // CE, CSN
#include <SoftwareSerial.h>

SoftwareSerial OpenLCD(6, 7); //RX, TX

// address of the pipe
const byte address[6] = "00001";

String message; //variable to hold the message and display it on LCD Screen
String InputVal;

void setup() {
  
  Serial.begin(9600);

  //To make the recieve node start listening//
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
  //////////////////////////////////////////
  Serial.println("Waiting for message from New Horizons"); //***change to display on LCD Screen***\\\
  
  pinMode(A2, OUTPUT);//first LED lit
  pinMode(3, OUTPUT);//second LED lit
  pinMode(4, OUTPUT);//third LED lit
  pinMode(5, OUTPUT);//fourth LED lit   
  OpenLCD.begin(9600); //Start communication with OpenLCD
  //display code on LCD Screen


}

void loop() {

 //This statement allows the module to be listening for messages from the Transmit module
    OpenLCD.write('|'); //Setting character
    OpenLCD.write('-'); //Clear display
    OpenLCD.println("Hello Earthlings");//16 characters
    delay(1000);
 if (radio.available()) {
    char text[32] = "";
    radio.read(&text, sizeof(text));
//    Serial.print(text);
//    delay(4000);
    message = text;//sets the string value to the value of the characters that come in via the transmitter
    Serial.println(text);
    OpenLCD.write('|'); //Setting character
    OpenLCD.write('-'); //Clear display
    OpenLCD.println(text);
    delay(1000);
///////////*****For the LEDs, to add a delay to the code to mimic how long it takes for data to come back to Earth, scaled down******\\\\\\\\\ 
    digitalWrite(A2, HIGH);
    delay (200);//delay for 2 seconds (do real math)
    digitalWrite(3,HIGH);
    delay(200);//delay 2 seconds for the second light
    digitalWrite(4,HIGH);
    delay(200);
   digitalWrite(5,HIGH);
   delay(200);
   digitalWrite(A2,LOW);
   digitalWrite(3,LOW);
   digitalWrite(4,LOW);
   digitalWrite(5,LOW);
    //no delay after the second light because that one indicates it has "reached earth"
    //display user's findings on the LCD screen
    
//
}
}
