// ========================================================================================
//      Meine Welt in meinem Kopf
// ========================================================================================
// Projekt:       Gaestebuch / Visitors book
// Author:        Johannes P. Langner
// Controller:    WEMOS D1 Mini
// Actor:         RTC DS 1307, SD Card
// Description:   Kleines gaestebuch
// ========================================================================================

#include <ESP8266WiFi.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>                             // library for the sd card
#define DS1307_I2C_ADDRESS 0x68

// ========================================================================================
// information
int mVisitorsCount = 0;

// ========================================================================================
// error message
bool mShowErrorMessage = false;
String mErrorMessage;

// ========================================================================================
// SD Card
String mGuestList = String("guest.txt");
Sd2Card mCard;
SdVolume mVolume;
SdFile mRoot;
const int mChipSelect = D8; // D4;

// ========================================================================================
// server
WiFiServer mServer(80);

// ========================================================================================
// 0 = second, 1 = minute, 2 = hour, 3 = day of the week, 
// 4 = day of the month, 5 = month, 6 = year
byte mActualTime[7];

// ========================================================================================
// convert a decimal to binary value
// ----------------------------------------------------------------------------------------
// parameter
// val              = deciaml value
// ----------------------------------------------------------------------------------------
// return           = binary value
byte decToBcd(byte val){
  return( (val/10*16) + (val%10) );
}

// ========================================================================================
// convert binary to decimal
// ----------------------------------------------------------------------------------------
// parameter
// val              = binary value
// ----------------------------------------------------------------------------------------
// return           = decimal value
byte bcdToDec(byte val){
  return( (val/16*10) + (val%16) );
}

void setup() {
   Wire.begin();
   Wire.setClock(100000); 
   Serial.begin(115200);

   if(!checkAndSetTimeSetup(0)){
      Serial.println("error");
      while(true) {}
   }

  StartWebserver();

  pinMode(mChipSelect, OUTPUT);             // pin select for sd card
  InitSdCard();

  setDS1307();
}

void loop() {
  mErrorMessage = "";
  mShowErrorMessage = false;
  
  bool isTimeSet = checkAndSetTimeSetup(0);

  printOnWebside();

  delay(500);
}

// ========================================================================================
// check rtc and set time if is zero
// TIP: time setup start the last entered time. (config method 'setDs1307()')
// ----------------------------------------------------------------------------------------
// tryCount = counting recursiv run
bool checkAndSetTimeSetup(int tryCount){ 

  if(tryCount >= 3){
    Serial.println("can not setup time..");
    return false;
  }

  //Serial.println("check time");
  readActualTime();
  byte zeroCount = 0;

  for(int index = 0; index < 7; index++) {
    if(mActualTime[index] == 0){
      zeroCount++;
    }
  }

  if(zeroCount >= 3){
    Serial.println("setup time...");

    setDS1307();                                    // must setup actual last tim
    tryCount++;
    return checkAndSetTimeSetup(tryCount);
  }

//  Serial.print("read time success ("); 
//  Serial.print(zeroCount, DEC); 
//  Serial.println(")"); 
  return true;
}

// ========================================================================================
// read actual time from rtc
void readActualTime(){
  Wire.beginTransmission(DS1307_I2C_ADDRESS);
  Wire.write(0);
  Wire.endTransmission();
  Wire.requestFrom(DS1307_I2C_ADDRESS, 7);

  for(int index = 0; index < 7; index++) {
    mActualTime[index] = Wire.read();
  }
}

// ========================================================================================
// set actual last time to update the rtc
void setDS1307(){

  byte setSecond = 0;
  byte setMinute = 31;
  byte setHour = 21;
  byte setDayOfWeek = 1;
  byte setDayOfMonth = 25;
  byte setMonth = 8;
  byte setYear = 19;

  byte toConv1 = decToBcd(setYear);
  Serial.print("byte to bin: ");  Serial.println(toConv1, DEC); 
  byte toConv2 = bcdToDec(toConv1);
  Serial.print("bin to byte: ");  Serial.println(toConv2, DEC); 
  
  Wire.beginTransmission(DS1307_I2C_ADDRESS);
  Wire.write(0);
  Wire.write(decToBcd(setSecond)); 
  Wire.write(decToBcd(setMinute));
  Wire.write(decToBcd(setHour)); 
  Wire.write(decToBcd(setDayOfWeek)); 
  Wire.write(decToBcd(setDayOfMonth)); 
  Wire.write(decToBcd(setMonth)); 
  Wire.write(decToBcd(setYear)); 
  Wire.endTransmission();
}

// ========================================================================================
// start webserver and print information on serial
// setup the ESP8266 to AccessPoint
void StartWebserver() {

  Serial.println("Start access point");
  WiFi.mode(WIFI_AP);                    

  byte setMode = 0;                              // 0 = disable, 1 = enable router information
  wifi_softap_set_dhcps_offer_option(OFFER_ROUTER, &setMode);
  WiFi.softAP("VisitorsBook", "");

  mServer.begin();                                  // Start the HTTP Server

  Serial.print("Ip Address: ");
  Serial.println(WiFi.softAPIP()); // Send the IP address of the ESP8266 to the computer
}

// ========================================================================================
// if the client call 
void printOnWebside() {
  
  WiFiClient webclient = mServer.available();
  
  if (!webclient) {
    return;
  }
  if (!WaitClientSendsData(webclient, 100)) {
    return;
  }

  String request = webclient.readStringUntil('\r');
  Serial.println(request);
  webclient.flush();
  
  // GET /?guestname=test&settime=gggh HTTP/1.1
  int indexGuest = request.indexOf("guestname=");
  Serial.print("index guestname: "); Serial.println(indexGuest, DEC);

  if(indexGuest != -1) {

    int indexEndGuestname = request.indexOf('&');
    String guestName = request.substring(indexGuest + 10, indexEndGuestname);
    Serial.print("substring of guestname: ");
    Serial.println(guestName);

    int indexSetTime = request.indexOf("settime=");
    Serial.print("index settime: "); Serial.println(indexSetTime, DEC);
    int indexEndSettime = request.indexOf(" HTTP");
    String settime = request.substring(indexSetTime + 8, indexEndSettime);
    // 2019-08-25T19%3A40
    // filter technical colon
    settime.replace("T", " ");
    settime.replace("%3A", ":");
    Serial.print("substring of settime: ");
    Serial.println(settime);
    
    WriteGuest(guestName, settime);
  }

  BuildWebside(webclient);
}

// ========================================================================================
// Waiting for sending data from client.
// ----------------------------------------------------------------------------------------
// timeout = set max time to wait for incoming data.
boolean WaitClientSendsData(WiFiClient webClient, int timeout) {

  int timeOutCount = 0;
  while(!webClient.available()){

    timeOutCount++;
    
    if(timeOutCount >= timeout) {
      return false;
    }
    delay(1);
  }

  return true;
}
