// #include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>
#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4
#define TURN_LEFT 5
#define TURN_RIGHT 6
#define STOP 0

#define LEFT_MOTOR 0
#define RIGHT_MOTOR 1

#define FORWARD 1
#define BACKWARD -1

struct MOTOR_PINS
{
  int pinIN1;
  int pintIN2;
}
std::vector<MOTOR_PINS> motorPins = 
{
  {26,27}, //LEFT_MOTOR
  {12,13}  //RIGHT_MOTOR
};

const char* ssid     = "WiFiCar";
const char* password = "12345";

AsyncWebServer server(80);
AsyncWebSocket ws("\ws");

// WEB CONTROL
// const char* htmlHomePage PROGMEM = R"HTMLHOMEPAGE(
//   <!DOCTYPE html>
//   <html>
//     <head>
//     <meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1, user-scalable=no">
//       <style>
//       .arrows {
//         font-size:70px;
//         color:red;
//       }
//       .circularArrows {
//         font-size:80px;
//         color:blue;
//       }
//       td {
//         background-color:black;
//         border-radius:25%;
//         box-shadow: 5px 5px #888888;
//       }
//       td:active {
//         transform: translate(5px,5px);
//         box-shadow: none; 
//       }

//       .noselect {
//         -webkit-touch-callout: none; /* iOS Safari */
//           -webkit-user-select: none; /* Safari */
//           -khtml-user-select: none; /* Konqueror HTML */
//             -moz-user-select: none; /* Firefox */
//               -ms-user-select: none; /* Internet Explorer/Edge */
//                   user-select: none; /* Non-prefixed version, currently
//                                         supported by Chrome and Opera */
//       }
//       </style>
//     </head>
//     <body class="noselect" align="center" style="background-color:white">
      
//       <h1 style="color: teal;text-align:center;">Hash Include Electronics</h1>
//       <h2 style="color: teal;text-align:center;">Wi-Fi &#128663; Control</h2>
      
//       <table id="mainTable" style="width:400px;margin:auto;table-layout:fixed" CELLSPACING=10>
//         <tr>
//           <td ontouchstart='onTouchStartAndEnd("5")' ontouchend='onTouchStartAndEnd("0")'><span class="arrows" >&#11017;</span></td>
//           <td ontouchstart='onTouchStartAndEnd("1")' ontouchend='onTouchStartAndEnd("0")'><span class="arrows" >&#8679;</span></td>
//           <td ontouchstart='onTouchStartAndEnd("6")' ontouchend='onTouchStartAndEnd("0")'><span class="arrows" >&#11016;</span></td>
//         </tr>
        
//         <tr>
//           <td ontouchstart='onTouchStartAndEnd("3")' ontouchend='onTouchStartAndEnd("0")'><span class="arrows" >&#8678;</span></td>
//           <td></td>    
//           <td ontouchstart='onTouchStartAndEnd("4")' ontouchend='onTouchStartAndEnd("0")'><span class="arrows" >&#8680;</span></td>
//         </tr>
        
//         <tr>
//           <td ontouchstart='onTouchStartAndEnd("7")' ontouchend='onTouchStartAndEnd("0")'><span class="arrows" >&#11019;</span></td>
//           <td ontouchstart='onTouchStartAndEnd("2")' ontouchend='onTouchStartAndEnd("0")'><span class="arrows" >&#8681;</span></td>
//           <td ontouchstart='onTouchStartAndEnd("8")' ontouchend='onTouchStartAndEnd("0")'><span class="arrows" >&#11018;</span></td>
//         </tr>
      
//         <tr>
//           <td ontouchstart='onTouchStartAndEnd("9")' ontouchend='onTouchStartAndEnd("0")'><span class="circularArrows" >&#8634;</span></td>
//           <td style="background-color:white;box-shadow:none"></td>
//           <td ontouchstart='onTouchStartAndEnd("10")' ontouchend='onTouchStartAndEnd("0")'><span class="circularArrows" >&#8635;</span></td>
//         </tr>
//       </table>

//       <script>
//         var webSocketUrl = "ws:\/\/" + window.location.hostname + "/ws";
//         var websocket;
        
//         function initWebSocket() 
//         {
//           websocket = new WebSocket(webSocketUrl);
//           websocket.onopen    = function(event){};
//           websocket.onclose   = function(event){setTimeout(initWebSocket, 2000);};
//           websocket.onmessage = function(event){};
//         }

//         function onTouchStartAndEnd(value) 
//         {
//           websocket.send(value);
//         }
            
//         window.onload = initWebSocket;
//         document.getElementById("mainTable").addEventListener("touchend", function(event){
//           event.preventDefault()
//         });      
//       </script>
      
//     </body>
//   </html> 

//   )HTMLHOMEPAGE";

void processCartMovement(String InVal)
{
  Serial.printf("Got value as %s %d\n", InVal.c_str(), InVal.toInt())
  switch(InVal.toInt())
  {
    case UP:
      rotateMotor(RIGHT_MOTOR, FORWARD);
      rotateMotor(LEFT_MOTOR, FORWARD);
      break;
    
    case DOWN:
      rotateMotor(RIGHT_MOTOR, BACKWARD);
      rotateMotor(LEFT_MOTOR, BACKWARD);
      break;

    case LEFT:
      rotateMotor(RIGHT_MOTOR, FORWARD);
      rotateMotor(LEFT_MOTOR, BACKWARD);
      break;

    case RIGHT:
      rotateMotor(RIGHT_MOTOR, BACKWARD);
      rotateMotor(LEFT_MOTOR, FORWARD);
      break;
      
    case TURN_LEFT:
      rotateMotor(RIGHT_MOTOR, FORWARD);
      rotateMotor(LEFT_MOTOR, BACKWARD);
      break;
    
    case TURN_RIGHT:
      rotateMotor(RIGHT_MOTOR, BACKWARD);
      rotateMotor(LEFT_MOTOR, FORWARD);
      break;
    
    case STOP:
      rotateMotor(RIGHT_MOTOR, STOP);
      rotateMotor(LEFT_MOTOR, STOP);
      break;

    default:
      rotateMotor(RIGHT_MOTOR, STOP);
      rotateMotor(LEFT_MOTOR, STOP);
      break;

  }
}

void handleRoot(AsyncWebServerRequest *request) 
{
  request->send_P(200, "text/html", htmlHomePage);
}
void handleNotFound(AsyncWebServerRequest *request) 
{
    request->send(404, "text/plain", "File Not Found");
}
void onWebSocketEvent(AsyncWebSocket *server, 
                      AsyncWebSocketClient *client, 
                      AwsEventType type,
                      void *arg, 
                      uint8_t *data, 
                      size_t len) 
{                      
  switch (type) 
  {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      //client->text(getRelayPinsStatusJson(ALL_RELAY_PINS_INDEX));
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      processCarMovement("0");
      break;
    case WS_EVT_DATA:
      AwsFrameInfo *info;
      info = (AwsFrameInfo*)arg;
      if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) 
      {
        std::string myData = "";
        myData.assign((char *)data, len);
        processCarMovement(myData.c_str());       
      }
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
    default:
      break;  
  }
}

void setUpPinModes()
{
  for (int i = 0; i < motorPins.size(); i++)
  {
    pinMode(motorPins[i].pinIN1, OUTPUT);
    pinMode(motorPins[i].pinIN2, OUTPUT);  
    rotateMotor(i, STOP);  
  }
}

void setup() {
  setUpPinModes();
  Serial.begin(115200);

  WiFi.softAP(ssid, password);
  IPAddress IP = WiFI.softAP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.on("/", HTTP_GET, handleRoot);
  server.onNotFound(handleNotFound);

  ws.onEvent(onWebSocketEvent);

}

void loop() {
  // put your main code here, to run repeatedly:

}
