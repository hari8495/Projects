#include <DHT.h>
#include <ESP8266WiFi.h>
 
// replace with your channel's thingspeak API key, 
String apiKey = "9IWUUHTTCWJLRHHC";
const char* ssid = "Harish";
const char* password = "rockstar";
 
const char* server = "api.thingspeak.com";
#define DHTPIN 5 // D1 pin on Nodemcu
 
DHT dht(DHTPIN, DHT22);
WiFiClient client;

void setup() {                
  Serial.begin(115200);
  delay(15000);
  dht.begin();
  
  WiFi.begin(ssid, password);
 
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
   
  WiFi.begin(ssid, password);
   
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
}
 
 
void loop() {   
  
  float h = dht.readHumidity();
  float t = dht.readTemperature(true);
  if ( isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
 
  if (client.connect(server,80)) {  //   "184.106.153.149" or api.thingspeak.com
    String postStr = apiKey;
           postStr +="&temperature=";
           postStr += String((int)t);
           //postStr +="&field2=";
           //postStr += String((int)h);
 
     client.print("POST /update HTTP/1.1\n"); 
     client.print("Host: api.thingspeak.com\n"); 
     client.print("Connection: close\n"); 
     client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n"); 
     client.print("Content-Type: application/x-www-form-urlencoded\n"); 
     client.print("Content-Length: "); 
     client.print(postStr.length()); 
     client.print("\n\n"); 
     client.print(postStr);
           
 
     Serial.print("Temperature: ");
     Serial.print(t);
     //Serial.print(" degrees Celcius Humidity: "); 
     //Serial.print(h);
     Serial.println("% send to Thingspeak");    
  }
  client.stop();
   
  Serial.println("Waiting...");    
  // time between updates
  delay(60000); // 1 mins
}
