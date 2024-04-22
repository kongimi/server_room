/* 
  ESP_WebConfig 

  Copyright (c) 2015 John Lassen. All rights reserved.
  This is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This software is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  Latest version: 1.1.3  - 2015-07-20
  Changed the loading of the Javascript and CCS Files, so that they will successively loaded and that only one request goes to the ESP.

  -----------------------------------------------------------------------------------------------
  History

  Version: 1.1.2  - 2015-07-17
  Added URLDECODE for some input-fields (SSID, PASSWORD...)

  Version  1.1.1 - 2015-07-12
  First initial version to the public


  First time to Connect with "ESP" network and password "12345678"
  Login to 192.168.4.1/admin.html for first configure.
  
  */


//#include <ESP8266WiFi.h>
#include <WiFi.h>
#include <WiFiClient.h>
//#include <ESP8266WebServer.h>
#include <WebServer.h>
#include <Ticker.h>
#include <EEPROM.h>
#include <WiFiUdp.h>
#include "helpers.h"
#include "global.h"
#include "mycode.h"
/*
Include the HTML, STYLE and Script "Pages"
*/
#include "Page_Root.h"
#include "Page_Admin.h"
#include "Page_Script.js.h"
#include "Page_Style.css.h"
#include "Page_NTPsettings.h"
#include "Page_Information.h"
#include "Page_General.h"
#include "PAGE_NetworkConfiguration.h"
#include "example.h"
#include "Page_Upload.h"

#include <TridentTD_LineNotify.h>
#include "esp_system.h"
#include "DHT.h"
#include <Time.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Update.h>
#include <HTTPClient.h>

#define ACCESS_POINT_NAME  "ESP_test"        
#define ACCESS_POINT_PASSWORD  "12345678" 
#define AdminTimeOut 180  // Defines the Time in Seconds, when the Admin-Mode will be diabled

//#define LINE_TOKEN "IF8bgLAzKslUZc4U1y9D6fo7WvngdsI2a0SHr6IGpDd"

#define DHTTYPE DHT22
byte DHTPIN = 4;  // กำหนดขา DHT22 วัดอุณหภูมิ
byte ACPin = 13; //กำหนดขาที่เชื่อมต่อกับเซ็นเซอร์ AC Power

//======= สำหรับ sensor วัดกระแส =======
int sensorTA12 = A7; // Analog input pin that sensor is attached to
float nVPP;   // Voltage measured across resistor
float nCurrThruResistorPP; // Peak Current Measured Through Resistor
float nCurrThruResistorRMS; // RMS current through Resistor
float nCurrentThruWire;     // Actual RMS current in Wire

LiquidCrystal_I2C lcd(0x3f, 20, 4);
DHT dht(DHTPIN, DHTTYPE);

String LINE_TOKEN;


long now = millis();
long lastMeasure = 0;

String t;

WiFiClientSecure client; //--> Create a WiFiClientSecure object

int firstTime=0;
int backl=0;

void setup ( void ) {
  pinMode(sensorTA12, INPUT);
  pinMode(RelayAirCon1, OUTPUT);
  pinMode(RelayAirCon2, OUTPUT);
  pinMode(ACPin, INPUT);
  lcd.begin();
  lcd.noBacklight();
  EEPROM.begin(512);
  Serial.begin(19200);
  dht.begin();
  //delay(500);
  Serial.println("Starting ES8266");
//  EEPROM.write(0,'A');
//  EEPROM.write(1,'B');
//  EEPROM.write(2,'C');
  if (!ReadConfig())
  {
    // DEFAULT CONFIG
    config.ssid = "MYSSID";
    config.password = "MYPASSWORD";
    config.dhcp = true;
    config.IP[0] = 192;config.IP[1] = 168;config.IP[2] = 1;config.IP[3] = 100;
    config.Netmask[0] = 255;config.Netmask[1] = 255;config.Netmask[2] = 255;config.Netmask[3] = 0;
    config.Gateway[0] = 192;config.Gateway[1] = 168;config.Gateway[2] = 1;config.Gateway[3] = 1;
    config.ntpServerName = "0.de.pool.ntp.org";
    config.Update_Time_Via_NTP_Every =  0;
    config.timezone = -10;
    config.daylight = true;
    config.DeviceName = "No Name";
    config.AutoTurnOff[1] = false;
    config.AutoTurnOn[1] = false;
    config.TurnOffHour[1] = 0;
    config.TurnOffMinute[1] = 0;
    config.TurnOnHour[1] = 0;
    config.TurnOnMinute[1] = 0;
    config.AutoTurnOff[0] = false;
    config.AutoTurnOn[0] = false;
    config.TurnOffHour[0] = 0;
    config.TurnOffMinute[0] = 0;
    config.TurnOnHour[0] = 0;
    config.TurnOnMinute[0] = 0;
    config.LineToken1 = "MyLineToken1";
    config.LineToken2 = "MyLineToken2";
    
    WriteConfig();
    Serial.println("General config applied");
  }
  
  if (AdminEnabled)
  {
    
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP( ACCESS_POINT_NAME , ACCESS_POINT_PASSWORD);
  }
  else
  {
    WiFi.mode(WIFI_STA);
  }

  ConfigureWifi();

  server.on ( "/", processExample  );
  server.on ( "/admin/filldynamicdata", filldynamicdata );
  
  server.on ( "/favicon.ico",   []() { Serial.println("favicon.ico"); server.send ( 200, "text/html", "" );   }  );


  server.on ( "/admin.html", []() { Serial.println("admin.html"); server.send ( 200, "text/html", PAGE_AdminMainPage );   }  );
  server.on ( "/config.html", send_network_configuration_html );
  server.on ( "/info.html", []() { Serial.println("info.html"); server.send ( 200, "text/html", PAGE_Information );   }  );
  server.on ( "/ntp.html", send_NTP_configuration_html  );
  server.on ( "/general.html", send_general_html  );
//  server.on ( "/example.html", []() { server.send ( 200, "text/html", PAGE_EXAMPLE );  } );
  server.on ( "/style.css", []() { Serial.println("style.css"); server.send ( 200, "text/plain", PAGE_Style_css );  } );
  server.on ( "/microajax.js", []() { Serial.println("microajax.js"); server.send ( 200, "text/plain", PAGE_microajax_js );  } );
  server.on ( "/admin/values", send_network_configuration_values_html );
  server.on ( "/admin/connectionstate", send_connection_state_values_html );
  server.on ( "/admin/infovalues", send_information_values_html );
  server.on ( "/admin/ntpvalues", send_NTP_configuration_values_html );
  server.on ( "/admin/generalvalues", send_general_configuration_values_html);
  server.on ( "/admin/devicename",     send_devicename_value_html);
  server.on ( "/upload.html", []() { Serial.println("upload.html"); server.send ( 200, "text/html", Page_Upload );   }  );
  server.on ( "/AirCon1.html", []() { press_remote(0); server.send ( 200, "text/html", PAGE_AdminMainPage );  }  );
  server.on ( "/AirCon2.html", []() { press_remote(1); server.send ( 200, "text/html", PAGE_AdminMainPage );  }  );
  server.on ( "/update", HTTP_POST, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, []() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Serial.printf("Update: %s\n", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      /* flashing firmware to ESP*/
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
    }
  });
 
  server.onNotFound ( []() { Serial.println("Page Not Found"); server.send ( 400, "text/html", "Page not Found" );   }  );
  server.begin();
  Serial.println( "HTTP server started" );
  tkSecond.attach(1,Second_Tick);
  UDPNTPClient.begin(2390);  // Port for NTP receive

  LINE_TOKEN = config.LineToken1;
  LINE_TOKEN += config.LineToken2;

  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();
}

 
void loop ( void ) {
  String myIp;
  ArduinoOTA.handle();
  if (AdminEnabled)
  {
    if (AdminTimeOutCounter > AdminTimeOut)
    {
       AdminEnabled = false;
       Serial.println("Admin Mode disabled!");
       WiFi.mode(WIFI_STA);
    }
  }
  if (config.Update_Time_Via_NTP_Every  > 0 )
  {
    if (cNTP_Update > 5 && firstStart)
    {
      NTPRefresh();
      cNTP_Update =0;
      firstStart = false;
    }
    else if ( cNTP_Update > (config.Update_Time_Via_NTP_Every * 60) )
    {

      NTPRefresh();
      cNTP_Update =0;
    }
  }

  if(DateTime.second != Second_Old)
  {
    Second_Old = DateTime.second;
    lcd.setCursor(0,1);
    lcd.print(PrintTime());
    if(backl++ >= 10){
      lcd.noBacklight();
    }
  }

  if(DateTime.minute != Minute_Old)
  {
    lcd.backlight();
    backl = 0;
    Minute_Old = DateTime.minute;
    for(int i=0; i<=1; i++){
      if (config.AutoTurnOn[i])
      {
        if (DateTime.hour == config.TurnOnHour[i] && DateTime.minute == config.TurnOnMinute[i])
        {
          Serial.println("SwitchON");
          press_remote(i);
        }
      }
      if (config.AutoTurnOff[i])
      {
        if (DateTime.hour == config.TurnOffHour[i] && DateTime.minute == config.TurnOffMinute[i])
        {
          Serial.println("SwitchOff");
          press_remote(i);
        }
      }
    }
    lcd.setCursor(0,0);
    myIp = WiFi.localIP().toString().c_str();
    int y = 20-myIp.length();
    for(int t=0;t<y;t++){
      myIp += " ";
    }
    lcd.print(myIp);
    lcd.setCursor(0,1);
    lcd.print(PrintTime());
     
    if(DateTime.hour != Hour_Old){
      Hour_Old = DateTime.hour;
      printDHT(1);
    }else{
      printDHT(0);
    }

    int ACValue = digitalRead(ACPin);
    if(ACValue==1){ // ไฟฟ้าปกติ
      lcd.setCursor(0,2); // บรรทัดที่ 1
      lcd.print("AC : ");
      lcd.print("ON             ");
      if(Power==false){
        //LINE.notify("ไฟฟ้ากลับมาแล้ว");
        Power = true;
      }
    }else{
      lcd.setCursor(0,2); // บรรทัดที่ 1
      lcd.print("AC : ");
      lcd.print("Lost           ");

      //LINE.notify("ไฟฟ้าดับ!!!");
      Power = false;
    }

    float curr = get_amp();
    ////LINE.notify("กระแสไฟฟ้า = " + String(curr));
    now = millis();
  // Publishes new temperature and humidity every 3 seconds
//    if (now - lastMeasure > 3000) {
//      lastMeasure = now;
//
//      sendData(dht.readTemperature(),dht.readHumidity());
//    }

    Serial.print("Wifi status : ");
    Serial.println(WiFi.status());
    if (WiFi.status() != WL_CONNECTED){
      ConfigureWifi();
    }
  }
  server.handleClient();


  /*
  *    Your Code here
  */
  if(firstTime==0){
    myIp = WiFi.localIP().toString().c_str();
    Serial.println( WiFi.localIP().toString().c_str());
    if(myIp == "0.0.0.0"){

    }
    else{
      firstTime=1;
      LINE.setToken(LINE_TOKEN);  // กำหนด Line Token
      //LINE.notify(myIp);
      lcd.setCursor(0,0);
      lcd.print(WiFi.localIP());
      
      printDHT(0);

      lcd.setCursor(0,1);
      lcd.print(PrintTime());

      //client.setInsecure();
    }
  }

  if (Refresh)  
  {
    Refresh = false;
    ///Serial.println("Refreshing...");
     //Serial.printf("FreeMem:%d %d:%d:%d %d.%d.%d \n",ESP.getFreeHeap() , DateTime.hour,DateTime.minute, DateTime.second, DateTime.year, DateTime.month, DateTime.day);
  }
}

void printDHT(int sendLine){
  //=================================== temperature sensor =====================================
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      return ;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));

  lcd.setCursor(0,3);
  String myDhtText = "Humi:"+String(h)+"Temp:"+String(t);
  lcd.print(myDhtText);

  String lineText = "ความชื้น="+String(h)+"%, อุณหภูมิ="+String(t)+"°C ";
  if(sendLine==1 ){
    //LINE.notify(lineText);
  }

  if(config.AutoHumidity == true){
    if(h>=config.Humidity){
      lineText = "ความชื้นสูง!! " + String(h) + "%";
      //LINE.notify(lineText);
    }
  }

  if(config.AutoTemp == true){
    if(t>config.Temp){
      lineText = "อุณหภูมิสูง!! " + String(t) + "°C";
      //LINE.notify(lineText);
    }
  }

  
}

float getVPP()
{
  float result;
  int readValue;             //value read from the sensor
  int maxValue = 0;          // store max value here
  uint32_t start_time = millis();
  while((millis()-start_time) < 1000) //sample for 1 Sec
  {
     readValue = analogRead(sensorTA12);
     // see if you have a new maxValue
     if (readValue > maxValue) 
     {
         /*record the maximum sensor value*/
         maxValue = readValue;
     }
  }
  
  // Convert the digital data to a voltage
  result = (maxValue * 5.0)/1024.0;
  return result;
}

float get_amp(){
  nVPP = getVPP();
  nCurrThruResistorPP = (nVPP/200.0) * 1000.0;
  nCurrThruResistorRMS = nCurrThruResistorPP * 0.707;
  nCurrentThruWire = nCurrThruResistorRMS * 1000;
  return nCurrentThruWire;
}
//
//void sendData(float value,float value2) {
//  Serial.println("==========");
//  Serial.print("connecting to ");
//  Serial.println(host);
//  
//  //----------------------------------------Connect to Google host
//  if (!client.connect(host, httpsPort)) {
//    Serial.println("connection failed");
//    return;
//  }
//  //----------------------------------------
//
//  //----------------------------------------Proses dan kirim data  
//
//  float string_temp = value; 
//  float string_humi = value2;
//  String url = "/macros/s/" + GAS_ID + "/exec?temp=" + string_temp + "&humi="+string_humi; //  2 variables 
//  Serial.print("requesting URL: ");
//  Serial.println(url);
//
//  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
//         "Host: " + host + "\r\n" +
//         "User-Agent: BuildFailureDetectorESP8266\r\n" +
//         "Connection: close\r\n\r\n");
//
//  Serial.println("request sent");
//  //----------------------------------------
//
//  //---------------------------------------
//  while (client.connected()) {
//    String line = client.readStringUntil('\n');
//    if (line == "\r") {
//      Serial.println("headers received");
//      break;
//    }
//  }
//  String line = client.readStringUntil('\n');
//  if (line.startsWith("{\"state\":\"success\"")) {
//    Serial.println("esp8266/Arduino CI successfull!");
//  } else {
//    Serial.println("esp8266/Arduino CI has failed");
//  }
//  Serial.print("reply was : ");
//  Serial.println(line);
//  Serial.println("closing connection");
//  Serial.println("==========");
//  Serial.println();
//  //----------------------------------------
//} 

void sendData(float value,float value2) {
   HTTPClient http;
   float string_temp = value; 
   float string_humi = value2;
   //String url="/macros/s/"+GAS_ID+"/exec?temp=" + string_temp + "&humi="+string_humi;
   //String url="https://script.google.com/macros/s/"+GOOGLE_SCRIPT_ID+"/exec?temp=" + string_temp + "&humi="+string_humi;
   String url="https://maker.ifttt.com/trigger/demo/with/key/cRz-RoXxC60ROTaOFBBA2l?value1="+String(string_temp)+"&value2="+String(string_humi);
   Serial.print(url);
    Serial.println("Making a request");
    http.begin(url); //Specify the URL and certificate
    int httpCode = http.GET();  
    if (httpCode > 0) { //ถ้าส่งสำเร็จ
      String payload = http.getString();// อ่านค่าผลลัพธ์
      Serial.println(payload);
    }
    http.end();
    Serial.println(": done "+httpCode);
}
