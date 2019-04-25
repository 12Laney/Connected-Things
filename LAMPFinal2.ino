/*---------------------------------------------------------------------------------------------
  Open Sound Control (OSC) library for the ESP8266/ESP32
  Example for receiving open sound control (OSC) messages on the ESP8266/ESP32
  Send integers '0' or '1' to the address "/led" to turn on/off the built-in LED of the esp8266.
  This example code is in the public domain.
  --------------------------------------------------------------------------------------------- */
#ifdef ESP8266
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif
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

const int ZX_ADDR = 0x10;  // ZX Sensor I2C address 
ZX_Sensor zx_sensor = ZX_Sensor(ZX_ADDR);
uint8_t x_pos;
uint8_t z_pos;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(30, PIN, NEO_GRB + NEO_KHZ800);
int R = 0;
int G = 50; 
int B = 50; //variables for the colors
int counter = 0;
int RGBSend=0;//variable for send color val
const char* ssid = "things";
const char* password = "connected"; 

// A UDP instance to let us send and receive packets over UDP
WiFiUDP Udp;
const IPAddress outIp(10, 0, 1, 9);     // remote IP (not needed for receive)
IPAddress ip(10, 0, 1, 10); // my IP address (requested)
IPAddress gateway(10, 0, 1, 1);
IPAddress subnet(255, 255, 255, 0);
const unsigned int outPort = 9999;          // remote port (not needed for receive)
const unsigned int localPort = 8888;        // local port to listen for UDP packets (here's where we send the packets)


OSCErrorCode error;
unsigned int val = 0;              // LOW means led is *on*
unsigned int num =0;
int touch_sensor_value=0;
void setup() {
//  pinMode(BUILTIN_LED, OUTPUT);
//  digitalWrite(BUILTIN_LED, ledState);    // turn *on* led
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
//**************FOR ZX SENSOR AND LEDS

uint8_t ver;
  strip.begin();
  strip.show();

  if ( zx_sensor.init() ) {
    Serial.println("ZX Sensor initialization complete");
  } else {
    Serial.println("Something went wrong during ZX Sensor init!");
  }
  
  // Read the model version number and ensure the library will work
  ver = zx_sensor.getModelVersion();
  if ( ver == ZX_ERROR ) {
    Serial.println("Error reading model version number");
  } else {
    Serial.print("Model version: ");
    Serial.println(ver);
  }
  if ( ver != ZX_MODEL_VER ) {
    Serial.print("Model version needs to be ");
    Serial.print(ZX_MODEL_VER);
    Serial.print(" to work with this library. Stopping.");
    while(1);
  }
  
  // Read the register map version and ensure the library will work
  ver = zx_sensor.getRegMapVersion();
  if ( ver == ZX_ERROR ) {
    Serial.println("Error reading register map version number");
  } else {
    Serial.print("Register Map Version: ");
    Serial.println(ver);
  }
  if ( ver != ZX_REG_MAP_VER ) {
    Serial.print("Register map version needs to be ");
    Serial.print(ZX_REG_MAP_VER);
    Serial.print(" to work with this library. Stopping.");
    while(1);
  }
}


void loop() {
// toggle value to write

//**********TRANSMIIT*********
//*****For the ZX Sensor animating the LEDs
InititializeLights();
int sendZXVal=0; //variable for the send pos val
 
 if ( zx_sensor.positionAvailable() ) {
    x_pos = zx_sensor.readX();
    z_pos = zx_sensor.readZ();
    if ( z_pos != ZX_ERROR ) {
      if(z_pos<25){
        sendZXVal = 1;
      }
      if(z_pos>=25 and z_pos<50){
        sendZXVal = 2;
      }
      if(z_pos>=50 and z_pos<75){
        sendZXVal = 3;
      }
      if(z_pos>=75 and z_pos<100){
        sendZXVal = 4;
        
      }
       if(z_pos>=100){
        sendZXVal = 5;
      }

//      Serial.print(" Z: ");
//      Serial.println(z_pos);
//      Serial.println("sending message /ZXpos " + sendZXVal);
      OSCMessage msgOut("/ZXpos");
      msgOut.add(sendZXVal);//sending Zxpos val
      Udp.beginPacket(outIp, outPort);
      msgOut.send(Udp);
      Udp.endPacket();
      msgOut.empty();
      delay(100);
    }
  }
  
touch_sensor_value=touchRead(T0);
if(touch_sensor_value < 15){ //if copper is touched
    if(counter == 0){
      RGBSend = 1;
    }else if(counter == 1){
      RGBSend = 2;
    }else if(counter == 2){
      RGBSend = 3;
    }else if(counter ==3){
      RGBSend =4;
    }else if(counter == 4){
      RGBSend =5;
    }else{
      RGBSend =0;
    }
    counter++;
    if(counter>=5){
      counter = 0;
    }
    OSCMessage msgOut2("/RGBSendV");
      msgOut2.add(RGBSend);//sending RGB val
      Udp.beginPacket(outIp, outPort);
      msgOut2.send(Udp);
      Udp.endPacket();
      msgOut2.empty();
      delay(100);
}
  
//***************************
  OSCMessage msg; //RECIEVING MSG
  int size = Udp.parsePacket();
  if (size > 0) {
    while (size--) {
      msg.fill(Udp.read());
    }
    if (!msg.hasError())
    {
      // all possible OSC messages and handler functions
      msg.dispatch("/ZXpos", led);
      msg.dispatch("/RGBSendV", colorChange);
//      msg.dispatch("/analog", gotAnalog);
    }
    else
    {
      error = msg.getError();
      Serial.print("error: ");
      Serial.println(error);
    }
  }
}
void colorChange(OSCMessage &msg){
  num = msg.getInt(0);
  if(num == 0){
    R=70;
    B=0;
    G=0;
  }else if(num == 1){
    R=0;
    B = 70;
    G = 0;
  }else if(num == 2){
    R=0;
    B=0;
    G=70;
  }else if(num == 3){
    R=45;
    B=30;
    G=0;
  }else if(num == 4){
    R=35;
    B=0;
    G=35;
  }else{
    R=50;
    B=0;
    G=20;
  }
}
void led(OSCMessage &msg) {
     val = msg.getInt(0); //val is the Z position
    Serial.println(val);
  if(val == 1){//2 used to be 0
    for(int i=0; i<30;i+=5){
      strip.setPixelColor(i, R, G, B);
      strip.show();
    }
  }else if(val == 2){
    strip.setPixelColor(0,R, G, B);
    strip.setPixelColor(5,R, G, B);
    strip.setPixelColor(10,R, G, B);
    strip.setPixelColor(15,R, G, B);
    strip.setPixelColor(20,R, G, B);
    strip.setPixelColor(25,R, G, B);
    strip.setPixelColor(1,R, G, B);
    strip.setPixelColor(6,R, G, B);
    strip.setPixelColor(11,R, G, B);
    strip.setPixelColor(16,R, G, B);
    strip.setPixelColor(21,R, G, B);
    strip.setPixelColor(26,R, G, B);
    strip.show();
  }else if(val == 3){
    strip.setPixelColor(0,R, G, B);
    strip.setPixelColor(5,R, G, B);
    strip.setPixelColor(10,R, G, B);
    strip.setPixelColor(15,R, G, B);
    strip.setPixelColor(20,R, G, B);
    strip.setPixelColor(25,R, G, B);
    strip.setPixelColor(1,R, G, B);
    strip.setPixelColor(6,R, G, B);
    strip.setPixelColor(11,R, G, B);
    strip.setPixelColor(16,R, G, B);
    strip.setPixelColor(21,R, G, B);
    strip.setPixelColor(26,R, G, B);
    strip.setPixelColor(2,R, G, B);
    strip.setPixelColor(7,R, G, B);
    strip.setPixelColor(12,R, G, B);
    strip.setPixelColor(17,R, G, B);
    strip.setPixelColor(22,R, G, B);
    strip.setPixelColor(27,R, G, B);
    strip.show();
  }else if(val == 4){
    strip.setPixelColor(0,R, G, B);
    strip.setPixelColor(5,R, G, B);
    strip.setPixelColor(10,R, G, B);
    strip.setPixelColor(15,R, G, B);
    strip.setPixelColor(20,R, G, B);
    strip.setPixelColor(25,R, G, B);
    strip.setPixelColor(1,R, G, B);
    strip.setPixelColor(6,R, G, B);
    strip.setPixelColor(11,R, G, B);
    strip.setPixelColor(16,R, G, B);
    strip.setPixelColor(21,R, G, B);
    strip.setPixelColor(26,R, G, B);
    strip.setPixelColor(2,R, G, B);
    strip.setPixelColor(7,R, G, B);
    strip.setPixelColor(12,R, G, B);
    strip.setPixelColor(17,R, G, B);
    strip.setPixelColor(22,R, G, B);
    strip.setPixelColor(27,R, G, B);
    strip.setPixelColor(3,R, G, B);
    strip.setPixelColor(8,R, G, B);
    strip.setPixelColor(13,R, G, B);
    strip.setPixelColor(18,R, G, B);
    strip.setPixelColor(23,R, G, B);
    strip.setPixelColor(28,R, G, B);
    strip.show();
  }else if(val == 5){
    strip.setPixelColor(0,R, G, B);
    strip.setPixelColor(5,R, G, B);
    strip.setPixelColor(10,R, G, B);
    strip.setPixelColor(15,R, G, B);
    strip.setPixelColor(20,R, G, B);
    strip.setPixelColor(25,R, G, B);
    strip.setPixelColor(1,R, G, B);
    strip.setPixelColor(6,R, G, B);
    strip.setPixelColor(11,R, G, B);
    strip.setPixelColor(16,R, G, B);
    strip.setPixelColor(21,R, G, B);
    strip.setPixelColor(26,R, G, B);
    strip.setPixelColor(2,R, G, B);
    strip.setPixelColor(7,R, G, B);
    strip.setPixelColor(12,R, G, B);
    strip.setPixelColor(17,R, G, B);
    strip.setPixelColor(22,R, G, B);
    strip.setPixelColor(27,R, G, B);
    strip.setPixelColor(3,R, G, B);
    strip.setPixelColor(8,R, G, B);
    strip.setPixelColor(13,R, G, B);
    strip.setPixelColor(18,R, G, B);
    strip.setPixelColor(23,R, G, B);
    strip.setPixelColor(28,R, G, B);
    strip.setPixelColor(4,R, G, B);
    strip.setPixelColor(9,R, G, B);
    strip.setPixelColor(14,R, G, B);
    strip.setPixelColor(19,R, G, B);
    strip.setPixelColor(24,R, G, B);
    strip.setPixelColor(29,R, G, B);
    strip.show();
  }else{
    for(int i=0; i<30;i++){
      strip.setPixelColor(i, 0, 0, 0);
      strip.show();
    }
  }
}

int InititializeLights(){
  for(int i=0; i<30; i++){
    strip.setPixelColor(i, 0, 0, 0);
  }
}
