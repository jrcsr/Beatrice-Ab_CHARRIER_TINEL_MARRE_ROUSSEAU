/*
 * Romain Tribout - 28/01/2018
 * Learn to send a Sigfox message with
 * NodeMCU (ESP8266) 
 * BRKWS01 (Sigfox Wisol module - SFM10R1)
*/

#include <SoftwareSerial.h>
#include <Adafruit_Sensor.h>
//#include <DHT.h>
#include <DHT_U.h>
#define DHTPIN 0
#define DHTTYPE    DHT11 

#define RxNodePin 13
#define TxNodePin 15

DHT_Unified dht(DHTPIN, DHTTYPE);

uint32_t delayMS;

// Setup UART Communication with 
SoftwareSerial Sigfox(RxNodePin, TxNodePin);

// 12 bytes message buffer
uint8_t sigfoxMsg[12];

void setup () {
  Serial.begin(115200);
  dht.begin();
  sensor_t sensor;
  delay(200);
  Serial.println("\n***** START *****");


  pinMode(RxNodePin, INPUT);
  pinMode(TxNodePin, OUTPUT);
  Sigfox.begin(9600);
  delay(100);

  Serial.print("Device ID: " + getID()); 
  Serial.print("Device PAC Number: " + getPAC());
  delay(100);
  delayMS = sensor.min_delay / 1000;
}

void loop () {
  sigfoxMsg[0]=getTempHum()[0];
  sigfoxMsg[1]=getTempHum()[1];
  sigfoxMsg[2]=0x45;
  sigfoxMsg[3]=0x67;
  sigfoxMsg[4]=0x89;
  Serial.println(getTempHum()[0], HEX);
  Serial.println(getTempHum()[1], HEX);
  Serial.println("Send message: " + sendMessage(sigfoxMsg, 5));
  

  // Send every 10 minutes&
  delay(60000);
}

// Get device ID
String getID () {
  String deviceId = "";
  char sigfoxBuffer;

  // Send AT$I=10 to WISOL to GET ID number
  Sigfox.print("AT$I=10\r");

  while (!Sigfox.available()){
     delay(10);
  }
  
  while(Sigfox.available()){
    sigfoxBuffer = Sigfox.read();
    deviceId += sigfoxBuffer;
    delay(10);
  }
  return deviceId;
}


// Get PAC number
String getPAC (){
  String pacNumber = "";
  char sigfoxBuffer;

  // Send AT$I=11 to WISOL to GET PAC number
  Sigfox.print("AT$I=11\r");
  while (!Sigfox.available()){
     delay(10);
  }
  while(Sigfox.available()){
    sigfoxBuffer = Sigfox.read();
    pacNumber += sigfoxBuffer;
    delay(10);
  }
  return pacNumber;
}

String sendMessage(uint8_t sigfoxMsg[], int bufferSize) {
  String status = "";
  char sigfoxBuffer;

  // Send AT$SF=xx to WISOL to send XX (payload data of size 1 to 12 bytes)
  Sigfox.print("AT$SF=");
  for(int i= 0;i<bufferSize;i++){
    if (sigfoxMsg[i]<0x10) {
      Sigfox.print("0");
    }
    Sigfox.print(String(sigfoxMsg[i], HEX));
  }

  Sigfox.print("\r");

  while (!Sigfox.available()){
     delay(10);
  }

  while(Sigfox.available()){
    sigfoxBuffer = (char)Sigfox.read();
    status += sigfoxBuffer;
    delay(10);
  }

  return status;
}

int* getTempHum(){
  int t_h[2];
  float t;
  float h;
  int tt;
  int hh;

  //delay(delayMS);
  // Get temperature event and print its value.
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println(F("Error reading temperature!"));
  }
  else {
    Serial.print(F("Temperature: "));
    t = event.temperature;
    //tt = (int) t;
    t_h[0] = t;//t;
    Serial.print(t);
    Serial.println(F("°C"));
  }
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println(F("Error reading humidity!"));
  }
  else {
    Serial.print(F("Humidity: "));
    h = event.relative_humidity;
    //hh = (int) h;
    t_h[1] = h;//h;
    Serial.print(h);
    Serial.println(F("%"));
  }
  return t_h;
}
