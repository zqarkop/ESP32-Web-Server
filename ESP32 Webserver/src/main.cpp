//Created Mar 2022
#include <Arduino.h>
#include <WiFi.h>

const char* ssid     = "zq";
const char* password = "12345098765";

WiFiServer server(80);
String header;
String LED1State = "off";
String LED2State = "off";

const int LED1 = 26;
const int LED2 = 27;

unsigned long currentTime = millis();
unsigned long previousTime = 0; 
const long timeoutTime = 2000;

void setup() {
  Serial.begin(115200);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop(){
  WiFiClient client = server.available();   

  if (client) {                           
    Serial.println("New Client.");         
    String currentLine = "";                
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) { 
      currentTime = millis();         
      if (client.available()) {             
        char c = client.read();        
        Serial.write(c);         
        header += c;
        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            if (header.indexOf("GET /1/on") >= 0) {
              Serial.println("LED 1 on");
              LED1State = "on";
              digitalWrite(LED1, HIGH);
            } else if (header.indexOf("GET /1/off") >= 0) {
              Serial.println("LED 1 off");
              LED1State = "off";
              digitalWrite(LED1, LOW);
            } else if (header.indexOf("GET /2/on") >= 0) {
              Serial.println("LED 2 on");
              LED2State = "on";
              digitalWrite(LED2, HIGH);
            } else if (header.indexOf("GET /2/off") >= 0) {
              Serial.println("LED 2 off");
              LED2State = "off";
              digitalWrite(LED2, LOW);
            }
            
            client.println("<!DOCTYPE html>");
            client.println("<html>");
            client.println("<head>");
            client.println("    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");

            client.println("<style>");
            client.println("html { ");
            client.println("    font-family: Helvetica; ");
            client.println("    display: inline-block; ");
            client.println("   margin: 0px ");
            client.println("    auto; text-align:"); 
            client.println("    center;}");
            client.println(".button { ");
            client.println("    background-color: #22ff00;");
            client.println("    border: none; ");
            client.println("    color: white; ");
            client.println("    padding: 16px 40px;");
            client.println("    text-decoration: none;"); 
            client.println("    font-size: 30px; ");
            client.println("    margin: 2px; ");
            client.println("    cursor: pointer;");
            client.println("}");
            client.println(".button2 {");
            client.println("    background-color: #ff0000;");
            client.println("    }");
            client.println("</style>");
            client.println("</head>");

            client.println("<body><h1>Controll LED on ESP32 via Web Server (port 80)</h1>");
            client.println("<table align='center'>");
            client.println("    <th>LED 1</th>");
            client.println("    <th>LED 2</th>");
            client.println("    <tr>");   
            // Button LED 1
            if (LED1State=="off") {
              client.println("<td><a href=\"/1/on\"><button class=\"button\">ON</button></a></td>");
            } else {
              client.println("<td><a href=\"/1/off\"><button class=\"button button2\">OFF</button></a></td>");
            }
            // Button LED 2
            if (LED2State=="off") {
              client.println("<td><a href=\"/2/on\"><button class=\"button\">ON</button></a></td>");
            } else {
              client.println("<td><a href=\"/2/off\"><button class=\"button button2\">OFF</button></a></td>");
            }

            client.println("    </tr>");
            client.println("</table>");
            client.println("</body></html>");
            break;
          } else { 
            currentLine = "";
          }
        } else if (c != '\r') { 
          currentLine += c;     
        }
      }
    }
    header = "";
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}