// ========================================================================================
//      Meine Welt in meinem Kopf
// ========================================================================================
// Projekt:       Gaestebuch / Visitors book
// Author:        Johannes P. Langner
// Controller:    WEMOS D1 Mini
// Actor:         SD Card
// Description:   Kleines gaestebuch
// ========================================================================================

#include <ESP8266WiFi.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>                             // library for the sd card

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

void setup() {
  Wire.begin();
  Wire.setClock(100000); 
  Serial.begin(115200);
  
  StartWebserver();
  
  pinMode(mChipSelect, OUTPUT);             // pin select for sd card
  InitSdCard();
}

void loop() {
  mErrorMessage = "";
  mShowErrorMessage = false;

  printOnWebside();

  delay(500);
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
