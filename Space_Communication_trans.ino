# Connected-Things
#include <RadioHead.h>

#include <SPI.h>
#include <RF24.h>
RF24 radio(8, 10); // CE, CSN
String message; //variable to hold the message and display it on LCD Screen
String InputVal;
// address of the pipe
const byte address[6] = "00001";

void setup() {
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
  Serial.begin(9600);
  Serial.println("Hello New Horizons! Please type a message to send back to Earth.");//16 characters
}

// main loop 
void loop() {
  // send a message then wait a second
   const char text[18];
  if(Serial.available() >0){   
    message = Serial.readString();
    message.toCharArray(text,18);
  }
   radio.write(&text, sizeof(text));
    delay(1000);
    
}
