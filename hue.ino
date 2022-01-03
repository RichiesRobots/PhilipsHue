#include <WiFi.h> 
#include <WiFiClient.h>



#define HUESERVER "IP ADDRESS OF THE HUE BRIDGE" // Philips Hue
#define host "esp32"
#define WIFI_SSID "WIFI NAME"  //  network SSID (name)
#define WIFI_PW "WIFI PASSWORD"       // network password
#define API "YOUR HUE API"
#define PORT 80

String Light = "THE NUMBER ID OF THE LIGHT YOU WANT TO CONTROL";
String readString;

WiFiClient client;


void setHueValue(String command, String value, String Light)
{
   checkReconnectWifi();
  
  if (client.connect(HUESERVER, PORT))
  {
    client.print("PUT /api/");
    client.print(API);
    client.print("/lights/");
    client.print(Light);
    client.println("/state HTTP/1.1");
    client.println("keep-alive");
    client.print("Host: ");
    client.println(HUESERVER);
    client.print("Content-Length: ");
    client.println(command.length() + value.length() + 5);
    client.println("Content-Type: text/plain;charset=UTF-8");
    client.println();  // blank line before body
    client.println("{\"" + command + "\":" + value + "}");

    delay(500);

    client.stop();
  }
}

String getHueValue(String value, String Light) {

   checkReconnectWifi();

  readString = "";

  if (client.connect(HUESERVER, PORT)) {  //starts client connection, checks for connection
    client.print("GET /api/");
    client.print(API);
    client.print("/lights/");
    client.print(Light);
    client.println(" HTTP/1.1"); //download text
    client.print("Host: ");
    client.println(HUESERVER);
    client.println("Connection: close");  //close 1.1 persistent connection
    client.println(); //end of get request

  }
  else {
    Serial.println("connection failed"); //error message if no client connect
    Serial.println();
  }

  while (client.connected() && !client.available()) delay(1); //waits for data
  while (client.connected() || client.available()) { //connected or data available
    char c = client.read(); //gets byte from ethernet buffer
    readString += c; //places captured byte in readString

  }

  return readString.substring(readString.indexOf(value + "\":") + value.length() + 2, readString.indexOf(",", readString.indexOf(value + "\":")));
  client.stop();

  delay(500);

}

void initializeWiFi(String initialMessage) {
 
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PW);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(WIFI_SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  delay(2000);
}

void checkReconnectWifi(){

  // if WiFi is down, try reconnecting every CHECK_WIFI_TIME seconds
  if ((WiFi.status() != WL_CONNECTED)) {
   
    WiFi.disconnect();
    initializeWiFi("Reconnecting To ");
 }
}


void setup(){

Serial.begin(115200);
  
initializeWiFi("Connecting to "); 

}

void loop(){

setHueValue("on", "true", Light);
Serial.println("Light is on");
delay(3000);
setHueValue("bri", "254", Light);  
Serial.println("Light is at maximum brightness");
delay(3000);
setHueValue("bri", "127", Light);  
Serial.println("Light is at half brightness");
delay(3000);
setHueValue("on", "false", Light);  
Serial.println("Light is off");
delay(3000);
}  
