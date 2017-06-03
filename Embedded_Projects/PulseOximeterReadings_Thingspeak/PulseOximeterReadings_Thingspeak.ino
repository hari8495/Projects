/*
  Arduino-MAX30100 oximetry / heart rate integrated sensor library
  Copyright (C) 2016  OXullo Intersecans <x@brainrapers.org>
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <ESP8266WiFi.h>

// Replace with your network details
const char* ssid = "Kuruganti";             //Replace with your WiFi Name
const char* password = "rockstar";       //Your WiFi password

//Thingspeak Server Host and API Key
const char* server_thingspeak = "api.thingspeak.com"; //The Thingspeak Host Server
String apiKey = "C53MOK04AYJ1RAME"; //Replace API Key with your Thingspeak Channel Key

// Web Server on port 80
WiFiServer server(80);
WiFiClient thingspeak;

#define REPORTING_PERIOD_MS     5000
#define THINGSPEAK_MS 15000

// PulseOximeter is the higher level interface to the sensor
// it offers:
//  * beat detection reporting
//  * heart rate calculation
//  * SpO2 (oxidation level) calculation
PulseOximeter pox;

uint32_t tsLastReport = 0;
uint32_t thingspeakLastTransmit = 0;
float hr,spo2,temp;
// Callback (registered below) fired when a pulse is detected
void onBeatDetected()
{
  Serial.println("Beat!");
}

void setup()
{
  Serial.begin(57600);
  Serial.println("Initializing MAX30100");
  // Initialize the PulseOximeter instance and register a beat-detected callback
  pox.begin();
  pox.setOnBeatDetectedCallback(onBeatDetected);
  // Connecting to WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

}

void loop()
{
  // Make sure to call update as fast as possible
  pox.update();

  // Asynchronously dump heart rate and oxidation levels to the serial
  // For both, a value of 0 means "invalid"
  if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
    Serial.print("Heart rate:");
    hr = pox.getHeartRate();
    Serial.print(hr);
    Serial.print("bpm / SpO2:");
    spo2 = pox.getSpO2();
    Serial.print(spo2);
    Serial.print("% / temp:");
    temp = pox.getTemperature();
    Serial.print(temp);
    Serial.println("C");
    tsLastReport = millis();

    if (millis() - thingspeakLastTransmit > THINGSPEAK_MS)
    {

      if (thingspeak.connect(server_thingspeak, 80)) { //   "184.106.153.149" or api.thingspeak.com
        String postStr = apiKey;
        postStr += "&field1=";
        postStr += hr;
        postStr += "&field2=";
        postStr += spo2;
        postStr += "&field3=";
        postStr += temp;

        thingspeak.print("POST /update HTTP/1.1\n");
        thingspeak.print("Host: api.thingspeak.com\n");
        thingspeak.print("Connection: close\n");
        thingspeak.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
        thingspeak.print("Content-Type: application/x-www-form-urlencoded\n");
        thingspeak.print("Content-Length: ");
        thingspeak.print(postStr.length());
        thingspeak.print("\n\n");
        thingspeak.print(postStr);
        //Serial.print(" degrees Celcius Humidity: ");
        //Serial.print(h);
        Serial.println(" Sending to Thingspeak...");
      }
      thingspeakLastTransmit = millis();
      thingspeak.stop();
    }

  }

}
