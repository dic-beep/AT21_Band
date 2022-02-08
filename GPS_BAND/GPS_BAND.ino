//module gps
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

//blynk decraltion
#define BLYNK_TEMPLATE_ID "" //your blynk tamplate id
#define BLYNK_DEVICE_NAME "T21 Band"
//#define TINY_GSM_MODEM_SIM800 
//#include <Wire.h>
//#include <TinyGsmClient.h> //module gsm
#include <ESP8266WiFi.h> //module ESP8266
#include <BlynkSimpleEsp8266.h>  //module blynk
#define BLYNK_AUTH_TOKEN ""//your blynk auth token


//decralation gps
const int rxgps = 4;
const int txgps = 5;
static const uint32_t GPSBaud = 9600; 

//decralatoon buzzer
const int buzz = 14;

TinyGPSPlus gps; // The TinyGPS++ object
WidgetMap myMap(V11);  // V11 for virtual pin of Map Widget

SoftwareSerial ss(rxgps, txgps);  // The serial connection to the GPS device
//SoftwareSerial sim800(rxgsm,txgsm); //untuk menghubungkan ke sim

BlynkTimer timer;


float spd;       //Variable  to store the speed
float sats;      //Variable to store no. of satellites response
String bearing;  //Variable to store orientation or direction of GPS

//char apn[]  = "mms";
char auth[] = BLYNK_AUTH_TOKEN;              //Your Project authentication key
char ssid[] = "Redmi";                                       // Name of your network (HotSpot or Router name)
char pass[] = "qwerty123";                                      // Corresponding Password


//TinyGsm modem(SerialAT);

unsigned int move_index = 1;       // fixed location for now

  
BLYNK_WRITE(V0){  //untuk tombol buzzer
  int value = param.asInt();
  Serial.println(value);
  if(value == 1 || value == 1000000)  //jika ditekan buzzernya hidup
  {
    digitalWrite(buzz, HIGH);
    Serial.println("buzz ON");
  }
  if(value == 0 || value == -1000000) //jika tidak di tekan buzzer mati
  {
     digitalWrite(buzz, LOW);
     Serial.println("buzz OFF");
  }
}


void setup(){
  Serial.begin(9600);
  delay(100);
  Serial.println(); 
//  sim800.begin(9600); 
//  delay(100); 
  
//  sim800.println("AT");
//  sim800.println("AT+CPIN"); 
//  sim800.println("AT+CMGF=1\r"); 
//  delay(500); 
 
  // delay(1000);  
  // sim800.println("AT+CMGS=\"085125447849\"\r");
  // delay(1000);
  // sim800.println("mms,M2MINTERNET#");
  // delay(100);
  // sim800.println((char)26);
  // delay(1000);

  //setup buzzer
  pinMode(buzz, OUTPUT);
  delay(1000);
  digitalWrite(buzz, HIGH);
  delay(1000);
  digitalWrite(buzz, LOW);
  delay(1000);

//  SerialAT.begin(9600);
//  delay(3000);

//  Serial.println("Initializing modem...");
//  modem.restart();

//  String modemImei = modem.getIMEI();
//  SerialMon.print("Modem: ");
//  Serial.println(modemInfo);
//  SerialMon.print("Modem IMEI: ");
//  Serial.println(modemImei);

//  SerialMon.print("Waiting for network...");
//  if (!modem.waitForNetwork(240000L)) {
//    SerialMon.println(" fail");
//    delay(10000);
//    return;
//    }
//  SerialMon.println(" OK");
//
//  if (modem.isNetworkConnected()) {
//    SerialMon.println("Network connected");
//    }
//    
//  SerialMon.print(F("Connecting to APN: "));
//  SerialMon.print(apn);
//  if (!modem.gprsConnect(apn, ssid, pass)) {
//    SerialMon.println(" fail");
//    delay(10000);
//    return;
//  }
//  SerialMon.println(" OK");
  
  ss.begin(GPSBaud);
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(5000L, checkGPS); // every 5s check if GPS is connected, only really needs to be done once
}

void checkGPS(){
  if (gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
      Blynk.virtualWrite(V4, "GPS ERROR");  // Value Display widget  on V4 if GPS not detected
  }
}

void loop(){
//  while(sim800.available()){
//    Serial.println(sim800.readString());
//  }
//  while(Serial.available())  {
//    sim800.println(Serial.readString());
//  }
  
    while (ss.available() > 0) {
      // sketch displays information every time a new sentence is correctly encoded.
      if (gps.encode(ss.read()))
        displayInfo();
  }
  Blynk.run();
  timer.run();
}

void displayInfo()
{ 
  if (gps.location.isValid() ) 
  {    
    float latitude = (gps.location.lat());     //Storing the Lat. and Lon. 
    float longitude = (gps.location.lng()); 
    
    Serial.print("LAT:  ");
    Serial.println(latitude,6);  // float to x decimal places
    Serial.print("LONG: ");
    Serial.println(longitude,6);

    Blynk.virtualWrite(V1, String(latitude, 6));   
    Blynk.virtualWrite(V2, String(longitude, 6));  
    myMap.location(move_index,latitude,longitude, "GPS_Location");

       spd = gps.speed.kmph();               //get speed
       Blynk.virtualWrite(V3, spd);
       
       sats = gps.satellites.value();    //get number of satellites
       Blynk.virtualWrite(V4, sats);

       bearing = TinyGPSPlus::cardinal(gps.course.value()); // get the direction
       Blynk.virtualWrite(V5, bearing);                   
  }
  
 Serial.println();
}
