#include <SPI.h>
#include <Ethernet.h>
#include <ArduinoJson.h>

//  Set-up Ethernet Client Configuration
byte mac[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x01 }; // RESERVED MAC ADDRESS
EthernetClient client;

StaticJsonBuffer<200> jsonBuffer;
JsonObject& data = jsonBuffer.createObject();

// 30 minutes = 1800000 milliseconds
// 2 minutes  = 120000 milliseconds
// 30 seconds = 30000 milliseconds
#define delayMillis 120000
long lastMillis = 0;
unsigned long currentMillis = 0;

//  Sensor information
int sensorPin = 0; // analog pin for the TMP36's Vout (sense) pin

void setup() {
  Serial.begin(115200);

  data["auth_token"] = "<insert-api-key-here>";

  Serial.println("Beginning Process");

  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP"); 
  } else {
    Serial.println("Successfully began Ethernet using DHCP");
    Serial.println("connecting...");
  }

}

void loop(){

  currentMillis = millis();
  
  if(currentMillis - lastMillis > delayMillis) {
    int reading = analogRead(sensorPin);
    float voltage = reading * 5.0;
    voltage /= 1024.0;
  
    float temperatureC = (voltage - 0.5) * 100;
    float temperatureF = (temperatureC * 9.0 / 5.0) + 32.0;
  
    currentMillis = millis();
  
    lastMillis = currentMillis;
    
    client.connect("<insert-host-here>", 80);
    
    if (client.connected()) {
      Serial.println("Connected to the HOST on PORT 80");
      Serial.print("The current temperature in Fahrenheit:");
      Serial.println(temperatureF);
      data["current"] = temperatureF;

      writeResponse(client, data);
      delay(100);
    } else {
      Serial.println("Client hasn't been connected to the server.");
    }
    
    if (client.connected()) {
      Serial.println("client is not connected, stopping now");
      client.stop();
    }
  }
}

void writeResponse(EthernetClient& client, JsonObject& data) {
  client.print("POST /widgets/<insert-custom-widget-name-here> HTTP/1.1\r\nHost: <insert-host-here>\r\n");
  client.print("Connection:close\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length:");
  client.print(data.measureLength());
  client.print("\r\n\r\n");
  data.printTo(client);
  //client.print("\r\n");
}
