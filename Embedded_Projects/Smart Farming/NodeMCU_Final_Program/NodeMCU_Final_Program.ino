// Including the ESP8266 WiFi library
#include <ESP8266WiFi.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <SimpleTimer.h>

//Macros used in the program
#define DHTTYPE DHT11   // DHT 11
#define D2 4            //Pin D2 for Relay 1
#define D3 0            //Pin D3 for Relay 2
#define D6 12           //Pin D6 for Relay 3

//Moisture Sensor Input
const int analogIn = A0;

// Replace with your network details
const char* ssid = "Kuruganti";             //Replace with your WiFi Name
const char* password = "rockstar";       //Your WiFi password

//Thingspeak Server Host and API Key
const char* server_thingspeak = "api.thingspeak.com"; //The Thingspeak Host Server
String apiKey="9IWUUHTTCWJLRHHC"; //Replace API Key with your Thingspeak Channel Key

// Web Server on port 80
WiFiServer server(80);
WiFiClient thingspeak;

SimpleTimer timer; //Timer for publishing data to thingspeak every 15 minutes

// DHT Sensor
const int DHTPin = 5;
DHT dht(DHTPin, DHTTYPE);

// Temporary variables
static char celsiusTemp[7];
static char fahrenheitTemp[7];
static char humidityTemp[7];
int moisture = 0;
static float h,t,f;

// only runs once on boot
void setup() 
{
 
  // Initializing serial port for debugging purposes
  Serial.begin(115200);
  delay(10);
  
  //Initializing Relay connections
  pinMode(D2,OUTPUT);
  pinMode(D3,OUTPUT);
  pinMode(D6,OUTPUT);
  dht.begin();
  // Connecting to WiFi network
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
  
  // Starting the web server
  server.begin();
  Serial.println("Web server running. Waiting for the ESP IP...");
  delay(10000);
  
  // Printing the ESP IP address
  Serial.println(WiFi.localIP()); 
  timer.setInterval(15000, repeatMe);
}

// runs over and over again
 void loop() {
  // Listening for new clients
  // Check if a client has connected
      
  timer.run();
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
 
    Serial.println("New client connected.");
    // bolean to locate when the http request ends
    boolean blank_line = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();

        String request = client.readStringUntil('\r');
        Serial.println(request);
 
        // Match the request
 
        if (request.indexOf("/RELAY1=ON") != -1)  {
        
        digitalWrite(D2, LOW);
        }
        if (request.indexOf("/RELAY1=OFF") != -1)  {
        
        digitalWrite(D2, HIGH);
        }
        if (request.indexOf("/RELAY2=ON") != -1)  {
        
        digitalWrite(D3, LOW);
        }
        if (request.indexOf("/RELAY2=OFF") != -1)  {
        
        digitalWrite(D3, HIGH);
        } 
        if (request.indexOf("/RELAY3=ON") != -1)  {
        
        digitalWrite(D6, HIGH);
        }
        if (request.indexOf("/RELAY3=OFF") != -1)  {
        
        digitalWrite(D6, LOW);
        } 
        
        if (c == '\n' && blank_line) {
            // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
             h = dht.readHumidity();
            // Read temperature as Celsius (the default)
             t = dht.readTemperature();
            // Read temperature as Fahrenheit (isFahrenheit = true)
             f = dht.readTemperature(true);
            
            moisture = analogRead(analogIn);
            // Check if any reads failed and exit early (to try again).
            if (isnan(h) || isnan(t) || isnan(f) || isnan(moisture)) {
              Serial.println("Failed to read from DHT sensor!");
              strcpy(celsiusTemp,"Failed");
              strcpy(fahrenheitTemp, "Failed");
              strcpy(humidityTemp, "Failed");         
              Serial.println("Failed to read from Moisture Sensor!");
            }
            else{
              // Computes temperature values in Celsius + Fahrenheit and Humidity
              float hic = dht.computeHeatIndex(t, h, false);       
              dtostrf(hic, 6, 2, celsiusTemp);             
              float hif = dht.computeHeatIndex(f, h);
              dtostrf(hif, 6, 2, fahrenheitTemp);         
              dtostrf(h, 6, 2, humidityTemp);
              // You can delete the following Serial.print's, it's just for debugging purposes
              Serial.print("Humidity: ");
              Serial.print(h);
              Serial.print(" %\t Temperature: ");
              Serial.print(t);
              Serial.print(" *C ");
              Serial.print(f);
              Serial.print(" *F\t Heat index: ");
              Serial.print(hic);
              Serial.print(" *C ");
              Serial.print(hif);
              Serial.print(" *F");
              Serial.print("Humidity: ");
              Serial.print(h);
              Serial.print(" %\t Temperature: ");
              Serial.print(t);
              Serial.print(" *C ");
              Serial.print(f);
              Serial.print(" *F\t Heat index: ");
              Serial.print(hic);
              Serial.print(" *C ");
              Serial.print(hif);
              Serial.println(" *F");
              Serial.print(" Moisture (PPM): ");
              Serial.println(moisture);
            }
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println("Connection: close");
            client.println();
            // your actual web page that displays temperature and humidity
            client.println("<!DOCTYPE HTML>");
            client.println("<html>");
            client.println("<head></head><body><h1><strong><p style=\"font-family: Georgia\">Allied Electronics Company</p></strong></h1><h3>Temperature in Celsius: ");
            client.println(celsiusTemp);
            client.println("*C</h3><h3>Temperature in Fahrenheit: ");
            client.println(fahrenheitTemp);
            client.println("*F</h3><h3>Humidity: ");
            client.println(humidityTemp);
            client.println("%</h3><h3>");
            client.println("</h3><h3>Moisture: ");
            client.println(moisture);
            client.println("  PPM</h3><h3>");
            client.println("<br><br>");
            client.println("<a href=\"/RELAY1=ON\"\"><button>Turn On Switch 1 </button></a>     ");
            client.println("<a href=\"/RELAY1=OFF\"\"><button>Turn Off Switch 1</button></a>    ");
            client.println("<br><br>");  
            client.println("<a href=\"/RELAY2=ON\"\"><button>Turn On Switch 2 </button></a>     ");
            client.println("<a href=\"/RELAY2=OFF\"\"><button>Turn Off Switch 2</button></a>    ");  
            client.println("<br><br>");
            client.println("<a href=\"/RELAY3=ON\"\"><button>Turn On Switch 3 </button></a>     ");
            client.println("<a href=\"/RELAY3=OFF\"\"><button>Turn Off Switch 3</button></a>    ");
            client.println("</html>");
            client.println("</body></html>");     
            break;
        }
        if (c == '\n') {
          // when starts reading a new line
          blank_line = true;
        }
        else if (c != '\r') {
          // when finds a character on the current line
          blank_line = false;
        }
      }
    }  
    // closing the client connection
    delay(1);
    client.stop();
    Serial.println("Client 1 disconnected.");

}   

void repeatMe() 
{
    Serial.print("Uptime (s): ");
    Serial.println(millis() / 1000);

    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    t = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    f = dht.readTemperature(true);
            
    moisture = analogRead(analogIn);  
   
    if (thingspeak.connect(server_thingspeak,80)) {  //   "184.106.153.149" or api.thingspeak.com
    String postStr = apiKey;
           postStr +="&field1=";
           postStr += String(t);
           postStr +="&field2=";
           postStr += String(moisture);
           postStr +="&field3=";
           postStr += String(h);
 
     thingspeak.print("POST /update HTTP/1.1\n"); 
     thingspeak.print("Host: api.thingspeak.com\n"); 
     thingspeak.print("Connection: close\n"); 
     thingspeak.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n"); 
     thingspeak.print("Content-Type: application/x-www-form-urlencoded\n"); 
     thingspeak.print("Content-Length: "); 
     thingspeak.print(postStr.length()); 
     thingspeak.print("\n\n"); 
     thingspeak.print(postStr);
           
 
     Serial.print("  Temperature: ");
     Serial.print(t);
     Serial.print("  Humidity: ");
     Serial.print(h);
     Serial.print("  Moisture: ");
     Serial.print(moisture);
     //Serial.print(" degrees Celcius Humidity: "); 
     //Serial.print(h);
     Serial.println("% send to Thingspeak");    
  }
  thingspeak.stop();
   
  Serial.println("Waiting...");  

}
