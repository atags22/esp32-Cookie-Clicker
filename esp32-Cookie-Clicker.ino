/*
 WiFi Web Server LED Blink

 A simple web server that lets you blink an LED via the web.
 This sketch will print the IP address of your WiFi Shield (once connected)
 to the Serial monitor. From there, you can open that address in a web browser
 to turn on and off the LED on pin 5.

 If the IP address of your shield is yourAddress:
 http://yourAddress/H turns the LED on
 http://yourAddress/L turns it off

 This example is written for a network using WPA encryption. For
 WEP or WPA, change the Wifi.begin() call accordingly.

 Circuit:
 * WiFi shield attached
 * LED attached to pin 5

 created for arduino 25 Nov 2012
 by Tom Igoe

ported for sparkfun esp32 
31.01.2017 by Jan Hendrik Berlin
 
 */

#include <WiFi.h>

const char* ssid     = "CHANGEME";
const char* password = "pass_CHANGEME";

WiFiServer server(80);
const int ledPin = 4;
void setup()
{
    Serial.begin(115200);
    pinMode(ledPin, OUTPUT);      // set the LED pin mode

    delay(10);

    // We start by connecting to a WiFi network

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
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    
    server.begin();

}

int value = 0;
const char *html = "\n\
<!DOCTYPE html>\n\
<html>\n\
<body>\n\
\n\
<h1>Cookie Clicker!</h1>\n\
\n\
<button id=\"baker\" onclick=\"myFunction()\">Bake</button>\n\
\n\
<button id=\"addAdd\" onclick=\"myFunction2()\">Increase cookies per click</button>\n\
\n\
<button id=\"BuyGrandma\" onclick=\"addGrandma()\">Buy a grandma!</button>\n\
\n\
<p id=\"numberOfCookies\"></p>\n\
\n\
<p id=\"cookiesYouNeed\"></p>\n\
\n\
<p id=\"howManyYouGet\"></p>\n\
\n\
<p id=\"costOfGrandma\"></p>\n\
\n\
<p id=\"cookiesPerGrandma\"></p>\n\
\n\
<p id=\"numberOfGrandmas\"></p>\n\
\n\
<script>\n\
\n\
\n\
var cookies = 1;\n\
var cookiesPerClick = 1;\n\
var cost = 2;\n\
var add = 2\n\
var fibonacci1 = 1;\n\
var fibonacci2 = 0;\n\
var currentFibonacci;\n\
var isGrandmaRunning = 0;\n\
var numGrandma = 0;\n\
\n\
document.getElementById(\"baker\").style.background='#ffa500';\n\
\n\
function getNextFibonacci() {\n\
  currentFibonacci = fibonacci1 + fibonacci2\n\
    fibonacci2 = fibonacci1\n\
    fibonacci1 = currentFibonacci\n\
    return currentFibonacci\n\
\n\
\n\
}\n\
\n\
function updateDisplay() {\n\
\n\
  document.getElementById(\"numberOfCookies\").innerHTML = \"You have \" + cookies + \" cookies!\";\n\
        document.getElementById(\"cookiesPerGrandma\").innerHTML = \"You have \" + numGrandma + \" Grandmas!\"\n\
  document.getElementById(\"cookiesYouNeed\").innerHTML = \"You need \" + cost + \" cookies untill you can upgrade cookies per click!\";\n\
  document.getElementById(\"howManyYouGet\").innerHTML = \"You get \" + cookiesPerClick + \" cookies per click!\"\n\
    if (cookies >= cost){\n\
      document.getElementById(\"addAdd\").style.background='#00ff00';\n\
    }\n\
    else{\n\
      document.getElementById(\"addAdd\").style.background='#ff0000';\n\
    }\n\
\n\
\n\
}\n\
\n\
function runGrandma() {\n\
  setTimeout(function(){\n\
  cookies = cookies + (numGrandma * 500);\n\
      runGrandma();\n\
                        updateDisplay();\n\
      }, 2000);\n\
\n\
\n\
}\n\
\n\
function addGrandma() {\n\
\n\
  if (cookies >= 1000) {\n\
    if(isGrandmaRunning == 1){\n\
      numGrandma = numGrandma + 1\n\
                        cookies = cookies - 1000\n\
    }\n\
    else{\n\
      runGrandma();\n\
      isGrandmaRunning = 1\n\
    }\n\
    updateDisplay();\n\
\n\
  }\n\
\n\
\n\
}\n\
\n\
\n\
function myFunction() {\n\
  cookies = cookies + cookiesPerClick;\n\
  updateDisplay();\n\
}\n\
function myFunction2() {\n\
  if (cookies >= cost) {\n\
    cookiesPerClick = cookiesPerClick + add;\n\
\n\
    cookies = cookies - cost;\n\
    add = add + 2\n\
      cost = getNextFibonacci();\n\
    updateDisplay();   \n\
\n\
  }\n\
}\n\
</script>\n\
\n\
</body>\n\
</html>\n\
";

void loop(){
 WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("New Client.");           // print a message out the serial port
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
            client.println("Content-type:text/html; charset=utf-8");
            client.println();

            // the content of the HTTP response follows the header:
            client.print(html);

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /H")) {
          digitalWrite(ledPin, HIGH);               // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(ledPin, LOW);                // GET /L turns the LED off
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
