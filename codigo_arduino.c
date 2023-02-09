#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <Servo.h>. 

ESP8266WebServer server;
WebSocketsServer webSocket = WebSocketsServer(81);

char* ssid = "AndroidAP";
char* password = "mkdm8039";
int ligar = 0;

const int ledPin =  LED_BUILTIN;
int ledState = LOW; 
unsigned long previousMillis = 0;  
const long interval = 1000;  

int motorApin1 = D1;
int motorApin2 = 15;

int motorBpin1 = 12;
int motorBpin2 = 13;

// Defines Tirg and Echo pins of the Ultrasonic Sensor
const int servoPin = 4;
const int echoPin = 14;
const int trigPin = 2;
// Variables for the duration and the distance
long duration;
int distance;
Servo myServo; 

char webpage[] PROGMEM = R"=====(
<html>
<head>
  <script>
    var Socket;
    function init() {
      Socket = new WebSocket('ws://' + window.location.hostname + ':81/');
      Socket.onmessage = function(event){
        document.getElementById("rxConsole").value += event.data;
      }
    }
//    function sendText(){
//      Socket.send(document.getElementById("txBar").value);
//      document.getElementById("txBar").value = "";
//    }      
function moveup_start() {
    Socket.send("Aup");
}
function movedown_start() {
        Socket.send("Bdown");
}
function moveleft_start() {
        Socket.send("Cleft");
}
function moveright_start() {
        Socket.send("Dright"); 
}

function moveup_end() {
    Socket.send("ENO_up");
}
function movedown_end() {
        Socket.send("FNO_down");
}
function moveleft_end() {
        Socket.send("GNO_left");
}
function moveright_end() {
        Socket.send("HNO_right"); 
}
function ligarL() {
        Socket.send("Ligar"); 
}
  </script>
</head>
<body onload="javascript:init()">
  <div>
    <textarea id="rxConsole"></textarea>
  </div>
  <hr/>
  Iniciar sonar e driver uma vez pressionado, para desligar somente resetando o carrinho.  
  <button ontouchstart="ligarL()" >*Ligar*</button><br><br>
  <hr/> 
  <div style="text-align:center;width:480px;">
  <button ontouchstart="moveup_start()" ontouchend="moveup_end()">UP</button><br><br>
  <button ontouchstart="moveleft_start()" ontouchend="moveleft_end()">LEFT</button>
  <button ontouchstart="moveright_start()" ontouchend="moveright_end()">RIGHT</button><br><br>
  <button ontouchstart="movedown_start()" ontouchend="movedown_end()">DOWN</button>
</div>
  <hr/>
</body>
</html>
)=====";
void setup(){
pinMode(ledPin, OUTPUT);  
pinMode (motorApin1, OUTPUT);
pinMode (motorApin2, OUTPUT);

pinMode (motorBpin1, OUTPUT);
pinMode (motorBpin2, OUTPUT);
pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
pinMode(echoPin, INPUT); // Sets the echoPin as an Input

digitalWrite (15, LOW);
digitalWrite (2, HIGH);
digitalWrite (0, HIGH); 

  WiFi.begin(ssid,password);
  Serial.begin(115200);
    Serial.setDebugOutput(true);
      Serial.println();

  
  myServo.attach(servoPin);
  
  while(WiFi.status()!=WL_CONNECTED)  {
    Serial.print(".");
    delay(500);  }
  Serial.println("");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/",[](){
    server.send_P(200, "text/html", webpage);  
  });
  server.begin();
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

    int calculateDistance(){ 
   
  digitalWrite(trigPin, LOW); 
  unsigned long currentMicros = micros();
  while (micros() - currentMicros < 2) {
  yield();
}
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH); 
  while (micros() - currentMicros < 10) {
  yield();
}
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH); // Reads the echoPin, returns the sound wave travel time in microseconds
  distance= duration*0.034/2;

  return distance;
   }  

void loop()
{
  webSocket.loop();
  server.handleClient();
  if(Serial.available() > 0){
    char c[] = {(char)Serial.read()};
    webSocket.broadcastTXT(c, sizeof(c));
  }

  unsigned long currentMillis = millis();
    
if (ligar == 1){
  for(int i=15;i<=165;i = i+10){  
  myServo.write(i);

  currentMillis = millis();
  while (millis() - currentMillis < 15) {
  yield();
}
  distance = calculateDistance();// Calls a function for calculating the distance measured by the Ultrasonic sensor for each degree
  String json = "(D=";
  json += distance; // Sends the distance value into the Serial Port
  json += "),"; // Sends addition character right next to the previous value needed later in the Processing IDE for indexing
  webSocket.broadcastTXT(json.c_str(), json.length());
  }
//  // Repeats the previous lines from 165 to 15 degrees
  for(int i=165;i>15;i = i-10){  
  myServo.write(i);

  currentMillis = millis();
  while (millis() - currentMillis < 15) {
  yield();
}
  distance = calculateDistance();// Calls a function for calculating the distance measured by the Ultrasonic sensor for each degree
  String json = "(";
  json += distance; // Sends the distance value into the Serial Port
  json += "."; // Sends addition character right next to the previous value needed later in the Processing IDE for indexing
  webSocket.broadcastTXT(json.c_str(), json.length());

  }
}

}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length){
  if(type == WStype_TEXT){
     if(payload[0] == 'A'){
      digitalWrite (motorApin1, HIGH);
      digitalWrite (motorApin2, LOW);

      digitalWrite (motorBpin1, HIGH);
      digitalWrite (motorBpin2, LOW);
    }
    else if(payload[0] == 'B'){
      digitalWrite (motorApin1, LOW);
      digitalWrite (motorApin2, HIGH);

      digitalWrite (motorBpin1, LOW);
      digitalWrite (motorBpin2, HIGH);
    }
    else if(payload[0] == 'C'){
      digitalWrite (motorApin1, LOW);
      digitalWrite (motorApin2, LOW);

      digitalWrite (motorBpin1, HIGH);
      digitalWrite (motorBpin2, LOW);
    }
    else if(payload[0] == 'D'){
      digitalWrite (motorApin1, HIGH);
      digitalWrite (motorApin2, LOW);

      digitalWrite (motorBpin1, LOW);
      digitalWrite (motorBpin2, LOW);
    }
    else if(payload[0] == 'L'){
      if (ligar == 1) {
        ligar = 0; }
        else {
          ligar = 1; }
        }
      
      else {
      digitalWrite (motorApin1, LOW);
      digitalWrite (motorApin2, LOW);

      digitalWrite (motorBpin1, LOW);
      digitalWrite (motorBpin2, LOW);
    }
    }
    }
