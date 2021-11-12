// Date: 8.4.2018
// Drink: Fuse Tea

#include <ESP8266WiFi.h>            // For the access point
#include <ESP8266WebServer.h>       // For the web server
#include <WiFiClient.h>
#include <EEPROM.h>
#include <WiFiAC.h>
#include <FirebaseArduino.h>
#include <IRsend.h>
#include <SPI.h>
#include "RF24.h"

#define FIREBASE_HOST "wifi-ac-project.firebaseio.com"
#define FIREBASE_AUTH "7SqNrt22e0iKf4rq0OtCchKJU0U1uT8pVmN02LbK"
#define ledPin 5

ESP8266WebServer server(80);
IRsend irsend(15);
WiFiAC wifiac;
RF24 radio(2, 0);

byte addresses[][6] = {"1Node", "2Node"};             // Radio pipe addresses for the 2 nodes to communicate.
byte counter = 1;

char ssid[32];
char pass[32];
char uid[32];

String code = "";
String AC = "";
String target = "";
String path = "";

unsigned long timer;
uint16_t buff[400] = {0};
uint8_t buff8[400] = {0};

boolean change = false;

void resetPath(){
  path.remove(0);
  path.concat(uid);
  path.concat("/operation");
  Firebase.setString(path, "0");
}

void handleRoot() {
  String header = "HTTP/1.1 301 OK\r\nLocation: /login\r\nCache-Control: no-cache\r\n\r\n";
  server.sendContent(header);
}

void handleLogin() {
  Serial.println("handleLogin");
  
  String content = "<html><body><form action='/login' method='POST'>To log in, please use : admin/admin<br>";
  content += "User:<input type='text' name='USERNAME' placeholder='user name'><br>";
  content += "Password:<input type='password' name='PASSWORD' placeholder='password'><br>";
  content += "UID:<input type='text' name='UID' placeholder='uid' value='SCzEoE954PMvIpPtGParifQZalL2'><br>";
  content += "<input type='submit' name='SUBMIT' value='Submit'></form></body></html>";
  server.send(200, "text/html", content);

  if(server.hasArg("USERNAME") && server.arg("USERNAME") != "" && server.hasArg("UID") && server.arg("UID") != ""){
    String ssidS = server.arg("USERNAME");
    String passS = server.arg("PASSWORD");
    String uidS = server.arg("UID");

    Serial.print("UID: "); Serial.println(uidS);

  wifiac.clearEEPROM();
  
  for(int i=0; i<32; i++){
    ssid[i] = (char)0;
    pass[i] = (char)0;
    uid[i] = (char)0;
  }
  
  strcat(ssid,ssidS.c_str());
  strcat(pass,passS.c_str());
  strcat(uid,uidS.c_str());
  
  wifiac.writeEEPROM(0,32,ssid);
  wifiac.writeEEPROM(32,32,pass);
  wifiac.writeEEPROM(64,32,uid);

  if(strlen(ssid))
    change = true;
  }
}

void setWiFiCredentials(){
  change = false;

  server.on("/", handleRoot);
  server.on("/login", handleLogin);
  server.begin();
  Serial.println("HTTP server started");
  while(!change){//WiFi.status() != WL_CONNECTED){
    server.handleClient();
  }
}

void setup(){
  delay(1000);
  Serial.begin(115200);
  irsend.begin();
  pinMode(ledPin, OUTPUT);
  
  wifiac.readEEPROM(0,32,ssid);
  wifiac.readEEPROM(32,32,pass);
  wifiac.readEEPROM(64,32,uid);
  delay(1000);
  
  // deleting the chars array for debbuging only
  /*for(int i=0; i<32; i++){
    ssid[i] = (char)0;
    pass[i] = (char)0;
  }
  Serial.println("After:");
  Serial.print("ssid: "); Serial.println(ssid);
  Serial.print("pass: "); Serial.println(pass);*/
  /////////////////////
  
  WiFi.disconnect();
  delay(500);
  while(!wifiac.begin(ssid, pass)){       // begin() - try to connect for 15 seconds
    Serial.println("Configuring Access Point...");
    Serial.println("setWiFi START");
    WiFi.softAP("WiFi-AC", "12345678");
    IPAddress apIP = WiFi.softAPIP();                                 // Get access point IP
    Serial.print("Access Point IP is: "); Serial.println(apIP);       // and print it to screen
    setWiFiCredentials();
  }

  WiFi.softAPdisconnect(true);
  //WiFi.disconnect(true);
  
  Serial.println("Connected!!!");

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  path.remove(0);
  path.concat(uid);
  path.concat("/SSID");
  Firebase.setString(path, ssid);

  if(radio.begin())
    Serial.println("NRF On");
  else
    Serial.println("NRF Off");

  radio.enableAckPayload();                     // Allow optional ack payloads
  radio.enableDynamicPayloads();                // Ack payloads are dynamic payloads

  radio.openWritingPipe(addresses[1]);        // Both radios listen on the same pipes by default, but opposite addresses
  radio.openReadingPipe(1, addresses[0]);

  //radio.startListening();                       // Start listening

  radio.writeAckPayload(1, &counter, 1);        // Pre-load an ack-paylod into the FIFO buffer for pipe 1
  
  resetPath();
  timer = millis();
}

void loop(){
  // TODO: Add button to reset wifi credentials

  // DEBUG
    //Serial.print("path: "); Serial.println(path);

  String opS = Firebase.getString(path);
  Serial.print("operation: "); Serial.println(opS);
  int op = opS.toInt();
  int len = 0;
  byte gotByte;
  uint8_t packSize = 1;
  uint8_t sendBuff[16] = {0};
  boolean flag = true;
  int j=0;
  
  // Timer to check connection
  if((millis() - timer) > 5000){
    path.remove(0);
    path.concat(uid);
    path.concat("/status");
    Firebase.setInt(path, 200);
    resetPath();
    timer = millis();
  }
  
  switch(op){
    case 0:
      break;
    
    case 1:
      Serial.println("Get code from Firebase and send it to AC");
      code.remove(0);
      path = uid;
      
      // DEBUG
        Serial.print("code: "); Serial.println(code);

      //path = setPath("/AC");
      path.concat("/AC");
      AC = Firebase.getString(path);
      // DEBUG
        Serial.print("AC: "); Serial.println(AC);
      AC = AC.substring(1, AC.length()-1);
      // DEBUG
        Serial.print("AC: "); Serial.println(AC);
        
      path = uid;
      path.concat("/target");
      //path = setPath("/target");
      target = Firebase.getString(path);
      // DEBUG
        Serial.print("target1: "); Serial.println(target);

      target = target.substring(1, target.length()-1);
      target.replace('\\','/');

      // DEBUG
        Serial.print("target2: "); Serial.println(target);

      path = uid;
      path.concat("/ACs/" + AC + "/Codes/" + target);

      // DEBUG
        AC = Firebase.getString(path);
      // DEBUG
        Serial.print("path: "); Serial.println(path);

      code = Firebase.getString(path);
      // DEBUG
        Serial.print("code: "); Serial.println(code);

      len = wifiac.countElements(code);
      
      wifiac.stringToIntArray(code, buff);

      wifiac.uint16To8(buff, buff8, len);

      // NRF
      radio.stopListening();                                  // First, stop listening so we can talk.
      
      counter = (len/16)+1;
      Serial.print("counter: "); Serial.println(counter);
      
      while (j <= len){
        Serial.print("j: "); Serial.println(j);
        
        if(flag){
          sendBuff[0] = counter;
          flag = false;
        }
        else{
          for(int x=0; x<16; x++,j++){
            sendBuff[x] = buff8[j];
            //Serial.print(sendBuff[x]); Serial.print(", ");
          }
          packSize = 16;
        }

        if ( radio.write(&sendBuff, packSize) ) {                       // Send the counter variable to the other radio
          for(int z=0; z<16; z++){
            Serial.print(sendBuff[z]); Serial.print(", ");
          }
          if (!radio.available()) {                           // If nothing in the buffer, we got an ack but it is blank
            Serial.print(F("Got blank response. round-trip delay: "));
            //Serial.print(micros() - time);
            //Serial.println(F(" microseconds"));
          } else {
            while (radio.available() ) {                    // If an ack with payload was received
              radio.read( &gotByte, 1 );                  // Read it, and display the response time
              unsigned long timer = micros();

              Serial.print(F("Got response "));
              Serial.print(gotByte);
              //Serial.print(F(" round-trip delay: "));
              //Serial.print(timer - time);
              Serial.println(F(" microseconds"));
              counter++;                                  // Increment the counter variable
            }
          }

        } else {
          Serial.println(F("Sending failed."));  // If no ack response, sending failed
          j-=16;
        }

      delay(100);
      }
      /////

      
      //irsend.sendRaw(buff, 400, 38);
      //Serial.println("Sent to AC");
    
      resetPath();
      break;
    
    case 2:
      Serial.println("Record new code");
      code.remove(0);
      path = uid;
      path.concat("/AC");
      
      AC = Firebase.getString(path);
      AC = AC.substring(1, AC.length()-1);

      path = uid;
      path.concat("/target");
      target = Firebase.getString(path);
      target = target.substring(1, target.length()-1);              // Remove quatation marks
      target.replace('\\','/');
      path = uid;
      path.concat("/ACs/" + AC + "/Codes/" + target);

      code = wifiac.receiveCode();                        // The problem is here!!!
      if(code != ""){
        Firebase.setString(path, code);
      }
      delay(50);

      resetPath();
      break;

      case 3:
        Serial.println("Reseting WiFi Credentials");
        //wifiac.writeEEPROM(0,64,"0");
        wifiac.clearEEPROM();
        delay(50);
        path.remove(0);
        path.concat(uid);
        path.concat("/SSID");
        Firebase.setString(path, "");
        delay(50);
        resetPath();
        delay(50);
        break;
  }
  delay(100);
}
