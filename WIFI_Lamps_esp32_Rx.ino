//RECIEVE LAMP

#include <WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include <OSCBundle.h>
#include <OSCData.h>
#include <ZX_Sensor.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 12 //pin for the LEDs
Adafruit_NeoPixel strip = Adafruit_NeoPixel(30, PIN, NEO_GRB + NEO_KHZ800);
int R = 70;
int G = 20; 
int B = 50; //variables for the colors

const char* ssid = "BTUgoo";
const char* password = "blowItUp";

// A UDP instance to let us send and receive packets over UDP
WiFiUDP Udp;
const IPAddress outIp(10, 0, 0, 100);     // remote IP (not needed for receive)
IPAddress ip(10, 0, 0, 101); // my IP address (requested)
IPAddress gateway(10, 0, 0, 1);
IPAddress subnet(255, 255, 255, 0);
const unsigned int outPort = 8888;          // remote port (not needed for receive)
const unsigned int localPort = 9999;        // local port to listen for UDP packets (here's where we send the packets)


OSCErrorCode error;

void setup() {
  Serial.begin(115200);
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  int i = 0;
  int state = true;
  WiFi.begin(ssid, password);
  WiFi.config(ip, gateway, subnet);
  Serial.println("");
  Serial.println("Connecting to WiFi");

  // Wait for connection
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (i > 10) {
      state = false;
      break;
    }
    i++;
  }


  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Starting UDP");
  Udp.begin(localPort);
  Serial.print("Local port: ");
#ifdef ESP32
  Serial.println(localPort);
#else
  Serial.println(Udp.localPort());
#endif
//FOR LEDS
  strip.begin();
  strip.show();

}

void loop() {
InititializeLights();
  OSCMessage msgREC;
  int size = Udp.parsePacket();

  if (size > 0) {
    while (size--) {
      msgREC.fill(Udp.read());
    }
    if (!msgREC.hasError()) 
    {
      // all possible OSC messages and handler functions
      msgREC.dispatch("/ZXpos", ZXpos);
      
    }  
    else 
    {
      error = msgREC.getError();
      Serial.print("error: ");
      Serial.println(error);
    }
  }
}

void ZXpos(OSCMessage &msgREC){
  int val = msgREC.getInt(0); //val is the Z position
  if(val == 0){
    for(int i=0; i<30;i++){
      strip.setPixelColor(i, 0, 0, 0);
      strip.show();
      Serial.print("Getting val == 0");
    }
  }
  if(val ==1){//LEVEL 1
      levelOne();
  }
  if(val ==2){//LEVEL 2
      levelOne();
      levelTwo();
  }
  if(val ==3){//LEVEL 3
      levelOne();
      levelTwo();
      levelThree();
  }
  if(val ==4){//LEVEL 4
      levelOne();
      levelTwo();
      levelThree();
      levelFour();
  }
  if(val ==5){//LEVEL 5
      levelOne();
      levelTwo();
      levelThree();
      levelFour();
      levelFive();
  }
  
}

void levelOne(){
  for(int i=0; i<30;i+=5){
      strip.setPixelColor(i, R, G, B);
      strip.show();
      Serial.println("Setting Lights to Level 1");
    }
}
void levelTwo(){
  for(int i=1; i<30;i+=5){
      strip.setPixelColor(i, R, G, B);
      strip.show();
      Serial.println("Setting Lights to Level 2");
    }
}
void levelThree(){
    for(int i=2; i<30;i+=5){
      strip.setPixelColor(i, R, G, B);
      strip.show();
      Serial.println("Setting Lights to Level 3");
    }
}
void levelFour(){
  for(int i=3; i<30;i+=5){
      strip.setPixelColor(i, R, G, B);
      strip.show();
      Serial.println("Setting Lights to Level 4");
    }
}
void levelFive(){
  for(int i=4; i<30;i+=5){
      strip.setPixelColor(i, R, G, B);
      strip.show();
      Serial.println("Setting Lights to Level 5");
    }
}

int InititializeLights(){
  for(int i=0; i<30; i++){
    strip.setPixelColor(i, 0, 0, 0);
  }
}
