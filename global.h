#ifndef GLOBAL_H
#define GLOBAL_H

//ESP8266WebServer server(80);									// The Webserver
WebServer server(80);
boolean firstStart = true;										// On firststart = true, NTP will try to get a valid time
int AdminTimeOutCounter = 0;									// Counter for Disabling the AdminMode
strDateTime DateTime;											// Global DateTime structure, will be refreshed every Second
WiFiUDP UDPNTPClient;											// NTP Client
unsigned long UnixTimestamp = 0;								// GLOBALTIME  ( Will be set by NTP)
boolean Refresh = false; // For Main Loop, to refresh things like GPIO / WS2812
int cNTP_Update = 0;											// Counter for Updating the time via NTP
Ticker tkSecond;												// Second - Timer for Updating Datetime Structure
boolean AdminEnabled = true;		// Enable Admin Mode for a given Time
byte Minute_Old = 100;				// Helpvariable for checking, when a new Minute comes up (for Auto Turn On / Off)
byte Second_Old = 100;
byte Hour_Old = 100;
boolean Power = true;

struct strConfig {
	String ssid;
	String password;
	byte  IP[4];
	byte  Netmask[4];
	byte  Gateway[4];
	boolean dhcp;
	String ntpServerName;
	long Update_Time_Via_NTP_Every;
	long timezone;
	boolean daylight;
	String DeviceName;
	boolean AutoTurnOff[2];
	boolean AutoTurnOn[2];
  boolean AutoHumidity;
  boolean AutoTemp;
	byte TurnOffHour[2];
	byte TurnOffMinute[2];
	byte TurnOnHour[2];
	byte TurnOnMinute[2];
	byte LED_R;
	byte LED_G;
	byte LED_B;
  byte Humidity;
  byte Temp;
  String LineToken1;
  String LineToken2;
}   config;


/*
**
** CONFIGURATION HANDLING
**
*/
void ConfigureWifi()
{
	Serial.println("Configuring Wifi");
	WiFi.begin (config.ssid.c_str(), config.password.c_str());
	if (!config.dhcp)
	{
		WiFi.config(IPAddress(config.IP[0],config.IP[1],config.IP[2],config.IP[3] ),  IPAddress(config.Gateway[0],config.Gateway[1],config.Gateway[2],config.Gateway[3] ) , IPAddress(config.Netmask[0],config.Netmask[1],config.Netmask[2],config.Netmask[3] ));
	}
}

void WriteConfig()
{

	Serial.println("Writing Config");
	EEPROM.write(0,'C');
	EEPROM.write(1,'F');
	EEPROM.write(2,'G');

	EEPROM.write(16,config.dhcp);
	EEPROM.write(17,config.daylight);
	
	EEPROMWritelong(18,config.Update_Time_Via_NTP_Every); // 4 Byte

	EEPROMWritelong(22,config.timezone);  // 4 Byte


	EEPROM.write(26,config.LED_R);
	EEPROM.write(27,config.LED_G);
	EEPROM.write(28,config.LED_B);
  
  EEPROM.write(29,config.AutoHumidity);
  EEPROM.write(30,config.Humidity);
  
	EEPROM.write(32,config.IP[0]);
	EEPROM.write(33,config.IP[1]);
	EEPROM.write(34,config.IP[2]);
	EEPROM.write(35,config.IP[3]);

	EEPROM.write(36,config.Netmask[0]);
	EEPROM.write(37,config.Netmask[1]);
	EEPROM.write(38,config.Netmask[2]);
	EEPROM.write(39,config.Netmask[3]);

	EEPROM.write(40,config.Gateway[0]);
	EEPROM.write(41,config.Gateway[1]);
	EEPROM.write(42,config.Gateway[2]);
	EEPROM.write(43,config.Gateway[3]);

  EEPROM.write(44,config.AutoTemp);
  EEPROM.write(45,config.Temp);

	WriteStringToEEPROM(64,config.ssid);
	WriteStringToEEPROM(96,config.password);
	WriteStringToEEPROM(128,config.ntpServerName);

	EEPROM.write(300,config.AutoTurnOn[0]);
	EEPROM.write(301,config.AutoTurnOff[0]);
	EEPROM.write(302,config.TurnOnHour[0]);
	EEPROM.write(303,config.TurnOnMinute[0]);
	EEPROM.write(304,config.TurnOffHour[0]);
	EEPROM.write(305,config.TurnOffMinute[0]);
  EEPROM.write(306,config.AutoTurnOn[1]);
  EEPROM.write(307,config.AutoTurnOff[1]);
  EEPROM.write(308,config.TurnOnHour[1]);
  EEPROM.write(309,config.TurnOnMinute[1]);
  EEPROM.write(310,config.TurnOffHour[1]);
  EEPROM.write(311,config.TurnOffMinute[1]);
	WriteStringToEEPROM(312,config.DeviceName);
 
  WriteStringToEEPROM(350,config.LineToken1);
  WriteStringToEEPROM(383,config.LineToken2);

	EEPROM.commit();
}
boolean ReadConfig()
{

	Serial.println("Reading Configuration");
	if (EEPROM.read(0) == 'C' && EEPROM.read(1) == 'F'  && EEPROM.read(2) == 'G' )
	{
		Serial.println("Configurarion Found!");
		config.dhcp = 	EEPROM.read(16);

		config.daylight = EEPROM.read(17);

		config.Update_Time_Via_NTP_Every = EEPROMReadlong(18); // 4 Byte

		config.timezone = EEPROMReadlong(22); // 4 Byte

		config.LED_R = EEPROM.read(26);
		config.LED_G = EEPROM.read(27);
		config.LED_B = EEPROM.read(28);

    config.AutoHumidity = EEPROM.read(29);
    config.Humidity = EEPROM.read(30);

		config.IP[0] = EEPROM.read(32);
		config.IP[1] = EEPROM.read(33);
		config.IP[2] = EEPROM.read(34);
		config.IP[3] = EEPROM.read(35);
		config.Netmask[0] = EEPROM.read(36);
		config.Netmask[1] = EEPROM.read(37);
		config.Netmask[2] = EEPROM.read(38);
		config.Netmask[3] = EEPROM.read(39);
		config.Gateway[0] = EEPROM.read(40);
		config.Gateway[1] = EEPROM.read(41);
		config.Gateway[2] = EEPROM.read(42);
		config.Gateway[3] = EEPROM.read(43);
    config.AutoTemp = EEPROM.read(44);
    config.Temp = EEPROM.read(45);
    
		config.ssid = ReadStringFromEEPROM(64);
		config.password = ReadStringFromEEPROM(96);
		config.ntpServerName = ReadStringFromEEPROM(128);

		config.AutoTurnOn[0] = EEPROM.read(300);
		config.AutoTurnOff[0] = EEPROM.read(301);
		config.TurnOnHour[0] = EEPROM.read(302);
		config.TurnOnMinute[0] = EEPROM.read(303);
		config.TurnOffHour[0] = EEPROM.read(304);
		config.TurnOffMinute[0] = EEPROM.read(305);
    config.AutoTurnOn[1] = EEPROM.read(306);
    config.AutoTurnOff[1] = EEPROM.read(307);
    config.TurnOnHour[1] = EEPROM.read(308);
    config.TurnOnMinute[1] = EEPROM.read(309);
    config.TurnOffHour[1] = EEPROM.read(310);
    config.TurnOffMinute[1] = EEPROM.read(311);
		config.DeviceName= ReadStringFromEEPROM(312);

    config.LineToken1= ReadStringFromEEPROM(350);
    config.LineToken2= ReadStringFromEEPROM(383);
    
		return true;
		
	}
	else
	{
		Serial.println("Configurarion NOT FOUND!!!!");
		return false;
	}
}

/*
**
**  NTP 
**
*/

const int NTP_PACKET_SIZE = 48; 
byte packetBuffer[ NTP_PACKET_SIZE]; 
void NTPRefresh()
{
	if (WiFi.status() == WL_CONNECTED)
	{
		IPAddress timeServerIP; 
		WiFi.hostByName(config.ntpServerName.c_str(), timeServerIP); 
		//sendNTPpacket(timeServerIP); // send an NTP packet to a time server


		Serial.println("sending NTP packet...");
		memset(packetBuffer, 0, NTP_PACKET_SIZE);
		packetBuffer[0] = 0b11100011;   // LI, Version, Mode
		packetBuffer[1] = 0;     // Stratum, or type of clock
		packetBuffer[2] = 6;     // Polling Interval
		packetBuffer[3] = 0xEC;  // Peer Clock Precision
		packetBuffer[12]  = 49;
		packetBuffer[13]  = 0x4E;
		packetBuffer[14]  = 49;
		packetBuffer[15]  = 52;
		UDPNTPClient.beginPacket(timeServerIP, 123); 
		UDPNTPClient.write(packetBuffer, NTP_PACKET_SIZE);
		UDPNTPClient.endPacket();


		delay(1000);
  
		int cb = UDPNTPClient.parsePacket();
		if (!cb) {
			Serial.println("NTP no packet yet");
		}
		else 
		{
			Serial.print("NTP packet received, length=");
			Serial.println(cb);
			UDPNTPClient.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
			unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
			unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
			unsigned long secsSince1900 = highWord << 16 | lowWord;
			const unsigned long seventyYears = 2208988800UL;
			unsigned long epoch = secsSince1900 - seventyYears;
			UnixTimestamp = epoch;
			Serial.println(WiFi.localIP().toString().c_str());
		}
	}
}

void Second_Tick()
{
	strDateTime tempDateTime;
	AdminTimeOutCounter++;
	cNTP_Update++;
	UnixTimestamp++;
	ConvertUnixTimeStamp(UnixTimestamp +  (config.timezone *  360) , &tempDateTime);
	if (config.daylight) // Sommerzeit beachten
		if (summertime(tempDateTime.year,tempDateTime.month,tempDateTime.day,tempDateTime.hour,0))
		{
			ConvertUnixTimeStamp(UnixTimestamp +  (config.timezone *  360) + 3600, &DateTime);
		}
		else
		{
			DateTime = tempDateTime;
		}
	else
	{
			DateTime = tempDateTime;
	}
	Refresh = true;
}
 

#endif
