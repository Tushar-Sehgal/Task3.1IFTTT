#include <Wire.h>
#include <BH1750.h>
#include <WiFiNINA.h>
#include "secret.h" 

char ssid[] = SECRET_SSID;     
char pass[] = SECRET_PASS;     
String IFTTT_API_KEY = "duTDqg4GPf0Qnf3d4nYSJ7";

bool wasSunlight = false;        // Previous sunlight status
bool isSunlight = false;         // Current sunlight status

BH1750 lightMeter;

void setup() {
  Serial.begin(9600);
  while (!Serial) {}
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass);
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected.");
  }
  Serial.println("\nWiFi connected");
  Wire.begin();
  lightMeter.begin();
}

void loop() {
  float lux = lightMeter.readLightLevel();
  isSunlight = (lux > 100);

  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lx");

  if (isSunlight != wasSunlight) {
    if (isSunlight) {
      sendEvent("sunlight_start");
    } else {
      sendEvent("sunlight_stop");
    }
    wasSunlight = isSunlight;  // Updates last sunlight status
  }

  delay(10000); // Delay for 10 seconds before next read
}

void sendEvent(String eventName) {
  WiFiClient client;
  const char* host = "maker.ifttt.com";
  const int httpPort = 80;

  if (!client.connect(host, httpPort)) {
    Serial.println("Connection failed");
    return;
  }

  String url = "/trigger/" + eventName + "/with/key/" + IFTTT_API_KEY;
  
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  delay(10); // Wait for server's response

  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
}
