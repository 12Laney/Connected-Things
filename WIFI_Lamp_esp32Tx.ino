//TRANSMIT LAMP
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include <ZX_Sensor.h>

//#define PIN 12 //pin for the LEDs

const int ZX_ADDR = 0x10;  // ZX Sensor I2C address 
ZX_Sensor zx_sensor = ZX_Sensor(ZX_ADDR);
uint8_t x_pos;
uint8_t z_pos;
//Adafruit_NeoPixel strip = Adafruit_NeoPixel(30, PIN, NEO_GRB + NEO_KHZ800);
//int R = 70, G = 20, B = 50; //variables for the colors
//FOR WIFI SENDING AND RECIEVING
const char* ssid = "BTUgoo";
const char* password = "blowItUp"; 
WiFiUDP Udp;                                // A UDP instance to let us send and receive packets over UDP
const IPAddress outIp(10, 0, 0, 101);     // ***CHANGE THIS FOR SECOND MOD remote IP of your computer
// use to request a static IP
IPAddress ip(10, 0, 0, 100); // my IP address (requested)***CHANGE THIS FOR SECOND MOD
IPAddress gateway(10, 0, 0, 1);
IPAddress subnet(255, 255, 255, 0);
const unsigned int outPort = 9999;          //***CHANGE THIS FOR SECOND MOD remote port to receive OSC
const unsigned int localPort = 8888;        // ***CHANGE THIS FOR SECOND MODlocal port to listen for OSC packets (actually not used for sending)
void setup() {
  //*****************FOR WIFI MODS
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
//  Serial.begin(9600);
//  strip.begin();
//  strip.show();

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

//**********TRANSMIIT*********
//*****For the ZX Sensor animating the LEDs
//InititializeLights();
int sendZXVal=0; //variable for the send pos val
 if ( zx_sensor.positionAvailable() ) {
    x_pos = zx_sensor.readX();
    if ( x_pos != ZX_ERROR ) {
      Serial.print("X: ");
      Serial.print(x_pos);
    }
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

      Serial.print(" Z: ");
      Serial.println(z_pos);
      Serial.print("sending message /ZXpos ");
      OSCMessage msg("/ZXpos");
      msg.add(sendZXVal);//sending Zxpos val
      Udp.beginPacket(outIp, outPort);
      msg.send(Udp);
      Udp.endPacket();
      msg.empty();
      delay(100);
    }
  }
}
