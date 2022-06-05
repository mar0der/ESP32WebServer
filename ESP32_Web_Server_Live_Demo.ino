#include <WiFi.h>

const char* ssid = "PeterWorkshop";
const char* password = "PA$$word1";

//Current time
unsigned long currentTime = millis();
unsigned long previousTime = 0;
const long timeontTime = 2000;
WiFiServer server(80);
String flashStatus = "off";
String header;
const int flashPin = 4;

void setup(){
  //setup the pint for the flash
  pinMode(flashPin, OUTPUT);
  digitalWrite(flashPin, LOW);
  //setup the serial montor
  Serial.begin(115200); 
  Serial.print("Connecting to ");
  Serial.println(ssid);
  //connect to wifi
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print("+");
  }
  Serial.println("");
  Serial.println("Connected to WiFi");
  Serial.println("IP address:");
  Serial.println(WiFi.localIP());
  //start the server
  server.begin();
  
}

void loop(){
  WiFiClient client = server.available();

  if(client){
     currentTime = millis();
     previousTime = currentTime;
     Serial.println("New client connected");
     String currentLine = "";
     while(client.connected() && currentTime - previousTime <= timeontTime){
        currentTime = millis();
        if(client.available()){
            char c = client.read();
            header += c;
            Serial.print(c);
            if(c == '\n' ){
                if(currentLine.length() == 0) {
                  //send a reponce
                  SendResponse(client);
                  //do something with the GPIOs
                  DoSomething();
                  //generate webpage
                  SendWebPage(client);
                  break;
                }else{
                  currentLine = "";
                }
            }else if(c != '\r'){
               currentLine += c;  
            }
        }
     }
     header = "";
     client.stop();
     Serial.println("The client was discconected");
     Serial.println("");
  }
}

void SendResponse(WiFiClient client){
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println("Connection: close");
  client.println();
}

void SendWebPage(WiFiClient client){
  client.println("<!DOCTYPE html>\n<html>");
  client.println("<head>\n<title>This is ESP32 Cam Custom Web server</title>\n<head>");
  client.println("<body>\n<h1>ESP32 CAM Custom Web Server</h1>\n");
  client.println("<p>GPIO 4 - State is "+flashStatus+"</p>\n");
  if(flashStatus == "off"){
    client.println("<p><a href=\"/pin4/on\">ON</a></p>\n");
  }else{
    client.println("<p><a href=\"/pin4/off\">OFF</a></p>\n");
  }
  client.println("</body></html>");
}

void DoSomething(){
  if(header.indexOf("GET /pin4/on") >= 0){
    flashStatus = "on";
    digitalWrite(flashPin, HIGH);
  }
  if(header.indexOf("GET /pin4/off") >= 0){
    flashStatus = "off";
    digitalWrite(flashPin, LOW);
  }
}
