#include <Arduino.h>
#include <SPI.h>
#include <WiFi101.h>
#include <LiquidCrystal.h>

#include "arduino_secrets.h" 

//--defining functions--
void printWiFiStatus();
void printWiFiStatusToLCD();
void printMsg(char msg[], int level, int row, int col, bool clearScreen);
//----------------------

char ssid[] = AP_SSID;
int keyIndex = 0;
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
int led =  LED_BUILTIN;
int status = WL_IDLE_STATUS;
WiFiServer server(80);
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
  //TODO: Serial handling
  Serial.begin(9600);
  pinMode(led, OUTPUT);

  while (!Serial) { ; } //wait for Serial... Deprecated?
  
  printMsg("Creating WiFi:", 2, 0, 0, true);
  printMsg(ssid, 2, 1, 0, false);

  status = WiFi.beginAP(ssid); // AP start

  if (status != WL_AP_LISTENING) {
    Serial.println("Creating access point failed");
    // don't continue
    while (true);
  }

  //???
  // delay(10000);

  // start the web server on port 80
  server.begin();
  
  // you're connected now, so print out the status
  printWiFiStatus();
  printWiFiStatusToLCD();
}

void loop() {
  // compare the previous status to the current status
  if (status != WiFi.status()) {
    // it has changed update the variable
    status = WiFi.status();

    if (status == WL_AP_CONNECTED) {
      // a device has connected to the AP
      Serial.println("Device connected to AP");
    } else {
      // a device has disconnected from the AP, and we are back in listening mode
      Serial.println("Device disconnected from AP");
    }
  }

  WiFiClient client = server.available();   // listen for incoming clients
  
  if (client) {                             // if you get a client,
    Serial.println("new client");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.print("Click <a href=\"/H\">here</a> turn the LED on<br>");
            client.print("Click <a href=\"/L\">here</a> turn the LED off<br>");

            // The HTTP response ends with another blank line:
            client.println();

            // break out of the while loop:
            break;
          }
          else {      // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        }
        else if (c != '\r') {    // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /H")) {
          digitalWrite(led, HIGH);               // GET /H turns the LED on
        }

        if (currentLine.endsWith("GET /L")) {
          digitalWrite(led, LOW);                // GET /L turns the LED off
        }
      }
    }

    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}

// ---------------------
String IpAddress2String(const IPAddress& ipAddress);

void printMsg(char msg[], int level = 2, int row = 0, int col = 0, bool clearScreen = true){
  switch(level){
    //LCD
    case 0:
      if(clearScreen) lcd.clear();
      lcd.setCursor(col, row);
      lcd.print(msg);
      break;
    //Serial
    case 1:
      Serial.println(msg);
      break;
    //Both
    case 2:
    default:
      if(clearScreen) lcd.clear();
      lcd.setCursor(col, row);
      lcd.print(msg);
      Serial.println(msg);
      break;
  }
}

void printWiFiStatus() {

  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}

void printWiFiStatusToLCD() {
  IPAddress ip = WiFi.localIP();
  char ipadr[] = "";
  sprintf(ipadr, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);

  printMsg("SSID:", 2, 0, 0, true);
  printMsg(WiFi.SSID(), 2, 0, 0, false);
  printMsg("IP:", 2, 1, 0, false);
  printMsg(ipadr, 2, 1, 0, false);
}