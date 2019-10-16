/*
 *  Contains code from: 
 *  http://randomnerdtutorials.com/esp8266-remote-controlled-sockets/
 *  http://www.bruhautomation.com/p/cheapest-wifi-automated-blinds.html
 *  https://www.arduino.cc/en/Tutorial/Sweep
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except
 *  in compliance with the License. You may obtain a copy of the License at:
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed
 *  on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License
 *  for the specific language governing permissions and limitations under the License.
 */

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

//Network Settings
#define WLAN_SSID       "Weavery"
#define WLAN_PASS       "HiddenValley"

//Web Server Settings
WiFiClient client;
MDNSResponder mdns;
ESP8266WebServer server(80);
String webPage = "";


int pos = 0;
int state = 0;
int dirc = 0;

// Stepper Motor Pin Settings
const int stepPin = 12; //D6
const int dirPin = 13;  //D7
const int enablePin = 14; //D5


//Motion Properties
const double doorTravelDist = 12; //12 inch opening
const double mvSpeed = 1; // inch per second (UNUSED)
const double gearCircum = 1.75; //inch


void setup(void) {
  webPage += "<h1>Chicken Coop</h1><p>Door <a href=\"open\"><button>OPEN</button></a>&nbsp;<a href=\"close\"><button>CLOSE</button></a></p>";

  delay(1000);
  Serial.begin(9600);
  delay(500);
  Serial.println("Blind Startup Sequence");
  delay(500);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  // Set a static IP (optional)
  //IPAddress ip(192, 168, 1, 142);
  //IPAddress gateway(192, 168, 1, 1);
  //IPAddress subnet(255, 255, 255, 0);
  //WiFi.config(ip, gateway, subnet);
  // End of set a static IP (optional)
  delay(500);
  int i = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    //ESP.wdtFeed();
    //if (i > 40)                                        // Try 40 times to connect to Wifi
     // Serial.print("Restarting Wifi"); ESP.reset();    // Reset Wifi stack if more than 40 trys
    //i++;
    //WiFi.begin(WLAN_SSID, WLAN_PASS);
    //// Set a static IP retry (optional)
    //IPAddress ip(192, 168, 1, 142);
    //IPAddress gateway(192, 168, 1, 1);
    //IPAddress subnet(255, 255, 255, 0);
    //WiFi.config(ip, gateway, subnet);
    // End of set a static IP retry (optional)
  }

  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  
  //Web Hosting Settings
  if (mdns.begin("esp8266", WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }

  server.on("/", []() {
    server.send(200, "text/html", webPage);
  });
  server.on("/open", []() {
    server.send(200, "text/html", webPage);
    Serial.println("HTTP OPEN COMMAND RECEIVED");
    dirc = LOW;  // direction for servo to run
    state = 1; // sets current state
  });
  server.on("/close", []() {
    server.send(200, "text/html", webPage);
    Serial.println("HTTP CLOSE COMMAND RECEIVED");
    dirc = HIGH; // direction for servo to run
    state = 1;  // sets current state
  });
  server.begin();
  Serial.println("HTTP server started");
  
  //STEPPER MOTOR SETUP
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(enablePin,OUTPUT);
  digitalWrite(enablePin,HIGH);
  
}


void spinMotor(int dist, int direct) {
  state= 0 ;
  double steps_d;
  int steps;
  //200 steps per rotation, 1 inch travel per rotation
  steps_d = round(dist / gearCircum * 200);
  steps = (int)steps_d;
  //int steps = 200;
  //int delay = gearCircum / 200 / speed * 10000;
  int delay = 8000;
  
  digitalWrite(dirPin, direct); // Enables the motor to move in a particular direction
  digitalWrite(enablePin,LOW);

  for (int x = 0; x < steps; x++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(delay / 2);
    yield();
    digitalWrite(stepPin, LOW);
    delayMicroseconds(delay / 2);
    yield();
  }

  digitalWrite(enablePin,HIGH);
  return;
}

void loop(void) {
  if (state == 1) {
    Serial.println("Move Ordered");
    //servo_move();
    spinMotor(doorTravelDist, dirc);
  }
  state = 0;
  server.handleClient();

  ArduinoOTA.handle();
}
