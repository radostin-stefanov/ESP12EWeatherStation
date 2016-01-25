#include "ESP8266WiFi.h"
#include "WiFiClient.h"
#include "ESP8266WebServer.h"
#include "ESP8266mDNS.h"
#include "DHT.h" 

#define DHTPIN 4 
#define DHTTYPE DHT11 
DHT dht(DHTPIN, DHTTYPE); 

const char* ssid = "betalab"; 
const char* password = ""; 

ESP8266WebServer server(80); 

const int led = 13; 

void handleRoot() { 

String message = "ESP8266 Server Nodemcu\n\n"; 

// Reading temperature or humidity takes about 250 milliseconds! 
// Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor) 
float h = dht.readHumidity(); 
// Read temperature as Celsius (the default) 
float t = dht.readTemperature(); 
// Read temperature as Fahrenheit (isFahrenheit = true) 
float f = dht.readTemperature(true); 

// Check if any reads failed and exit early (to try again). 
//if (isnan(h) || isnan(t) || isnan(f)) { 
//Serial.println("Failed to read from DHT sensor!"); 
//return; 
//} 

// Compute heat index in Fahrenheit (the default) 
float hif = dht.computeHeatIndex(f, h); 
// Compute heat index in Celsius (isFahreheit = false) 
float hic = dht.computeHeatIndex(t, h, false); 

message += "Humidity: "; 
message += h; 
message += " %\t"; 
message += "Temperature: "; 
message += t; 
message += " *C "; 
message += f; 
message += " *F\t"; 
message += "Heat index: "; 
message += hic; 
message += " *C "; 
message += hif; 
message += " *F \nADC = "; 
message += analogRead(A0); 
message += ", i.e. "; 
message += analogRead(A0) * 3.3 / 1024 * 2.11 / 2.2; 
message += "V, bat "; 
message += analogRead(A0) * 7.71 / 683; 
message += " V \n"; 


digitalWrite(led, 1); 
server.send(200, "text/plain", message); 
digitalWrite(led, 0); 

} 

void handleNotFound(){ 
digitalWrite(led, 1); 
String message = "File Not Found\n\n"; 
message += "URI: "; 
message += server.uri(); 
message += "\nMethod: "; 
message += (server.method() == HTTP_GET)?"GET":"POST"; 
message += "\nArguments: "; 
message += server.args(); 
message += "\n"; 
for (uint8_t i=0; i < server.args(); i++){
message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
}
server.send(404, "text/plain", message); 
digitalWrite(led, 0); 
} 

void setup(void){ 

pinMode(led, OUTPUT); 
pinMode(A0, INPUT); 
digitalWrite(led, 0); 
Serial.begin(115200); 
dht.begin(); 
WiFi.begin(ssid, password); 
Serial.println(""); 

// Wait for connection 
while (WiFi.status() != WL_CONNECTED) { 
delay(500); 
Serial.print("."); 
} 
Serial.println(""); 
Serial.print("Connected to "); 
Serial.println(ssid); 
Serial.print("IP address: "); 
Serial.println(WiFi.localIP()); 

if (MDNS.begin("esp8266")) { 
Serial.println("MDNS responder started"); 
} 

server.on("/", handleRoot); 

server.on("/inline", [](){ 
server.send(200, "text/plain", "this works as well"); 
}); 

server.onNotFound(handleNotFound); 

server.begin(); 
Serial.println("HTTP server started"); 
} 

void loop(void){ 
server.handleClient(); 
}
