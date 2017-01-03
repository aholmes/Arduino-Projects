/*
References:

* RTC https://www.arduino.cc/en/Tutorial/WiFiRTC
* MKR1000 getting started https://www.arduino.cc/en/Guide/MKR1000
* Tone output https://www.arduino.cc/en/Tutorial/ToneMelody
* NTPClient http://playground.arduino.cc/Code/NTPclient
*/

#include <SPI.h>
#include <NTPClient.h>
#include <WiFi101.h>
#include <WiFiUdp.h>
#include <RTCZero.h>

#include "pitches.h"


const int TIMEZONE_OFFSET = -8;
char ssid[] = "Your WIFI SSID";
char pass[] = "Your WIFI password";

int status = WL_IDLE_STATUS;

RTCZero rtc;

static WiFiUDP udp;
NTPClient ntpClient = NTPClient(udp);

void setup() {
	Serial.begin(115200);

	connectToAP();
	rtcSetup();
}

void connectToAP() {
	// check for the presence of the shield:
	if (WiFi.status() == WL_NO_SHIELD) {
		Serial.println("WiFi shield not present");
		stopExecution();
	}

	// attempt to connect to Wifi network:
	while (status != WL_CONNECTED) {
		Serial.print("Attempting to connect to SSID: ");
		Serial.println(ssid);
		// Connect to WPA/WPA2 network. Change this line if using open or WEP network:
		status = WiFi.begin(ssid, pass);

		// wait 1 second for connection:
		delay(1000);
	}
}

void rtcSetup()
{
	rtc.begin();
	ulong epoch;
	int numberOfTries = 0, maxTries = 6;

	do {
		epoch = ntpClient.getEpochTime();
		numberOfTries++;
	} while(epoch == 0 || numberOfTries > maxTries);

	if (numberOfTries > maxTries) {
		Serial.print("NTP unreachable!!");
		stopExecution();
	}
	else {
		Serial.print("Epoch received: ");
		Serial.println(epoch);

		rtc.setEpoch(epoch);

		Serial.println();
	}
}

void loop() {
	outputRssiAudio();
}

void outputRssiAudio() {
	long rssi = WiFi.RSSI();
	const uint duration = 100;
	const uint pin = 8;
	double delayVal = getDelay(rssi);

	Serial.print("RSSI: ");
	Serial.println(rssi);
	Serial.print("Delay: ");
	Serial.println(delayVal);

	tone(pin, NOTE_B0, duration);
	delay(delayVal);
	noTone(pin);
}

double getDelay(double x, double a = -0.4, double b = -0.6, double c = 100)
{
	// (((ax)^2)bx)+c
	return (a * x) * (a * x) + c;
}

void stopExecution()
{
	 while(true) delay(UINT32_MAX);
}
