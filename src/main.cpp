/* Simple WiFi-controlled car on ESP8266 
 * and L9110S motor drive board
 * by Alan Wang
 */

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// WiFi settings
#define SSID_AP            "ESP8266_WiFi_Car" // AP server name
#define PASSWORD_AP        "12345678"         // AP server password

// motor settings
#define RIGHT_MOTOR_PIN1  4                   // pin 1 of right motor (D2) right forw
#define RIGHT_MOTOR_PIN2  5                   // pin 2 of right motor (D1) right back
#define LEFT_MOTOR_PIN1   12                  // pin 1 of left motor (D6) left forw
#define LEFT_MOTOR_PIN2   14                  // pin 2 of left motor (D5) left backw
#define RIGHT_MOTOR_SPEED 1023                // speed for right motor (0-1023)
#define LEFT_MOTOR_SPEED  1023                // speed for left motor (0-1023)
#define LED_PIN           2                   // built-in LED (D4)

IPAddress local_ip(192, 168, 4, 1);           //IP for AP mode
IPAddress gateway(192, 168, 100, 1);          //IP for AP mode
IPAddress subnet(255, 255, 255, 0);           //IP for AP mode
ESP8266WebServer server(80);

int car_mode = 0;                             // set car drive mode (0 = stop)

void car_control();
void handle_left();
void handle_OnConnect();
void handle_forward();
void handle_backward();
void handle_left();
void handle_right();
void handle_stop();
void handle_NotFound();
String SendHTML();

// initialize
void setup() {
  
  Serial.begin(9600);
  Serial.println("ESP8266 Wifi Car");
  pinMode(RIGHT_MOTOR_PIN1, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN2, OUTPUT);
  pinMode(LEFT_MOTOR_PIN1, OUTPUT);
  pinMode(LEFT_MOTOR_PIN2, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  car_control(); // stop the car

  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  WiFi.softAP(SSID_AP, PASSWORD_AP);

  // setup web server to handle specific HTTP requests
  server.on("/", HTTP_GET, handle_OnConnect);
  server.on("/forward", HTTP_GET, handle_forward);
  server.on("/backward", HTTP_GET, handle_backward);
  server.on("/left", HTTP_GET, handle_left);
  server.on("/right", HTTP_GET, handle_right);
  server.on("/stop", HTTP_GET, handle_stop);
  server.onNotFound(handle_NotFound);

  //start server
  server.begin();
  Serial.println("ESP8266 car server started.");
  digitalWrite(LED_PIN, LOW);
}

// handle HTTP requests and control car
void loop() { 
  server.handleClient();
  car_control();
}

// HTTP request: on connect
void handle_OnConnect() {
  car_mode = 0;
//   Serial.println("Client connected");
  server.send(200, "text/html", SendHTML());
}

// HTTP request: stop car
void handle_stop() {
  car_mode = 0;
  Serial.println("Stopped");
  server.send(200, "text/html", SendHTML());
}

// HTTP request: go forward
void handle_forward() {
  car_mode = 1;
  Serial.println("Go forward...");
  server.send(200, "text/html", SendHTML());
}

// HTTP request: go backward
void handle_backward() {
  car_mode = 2;
  Serial.println("Go backward...");
  server.send(200, "text/html", SendHTML());
}

// HTTP request: turn left
void handle_left() {
  car_mode = 4;
  Serial.println("Turn left...");
  server.send(200, "text/html", SendHTML());
}
//mudei!!
// HTTP request: turn right
void handle_right() {
  car_mode = 3;
  Serial.println("Turn right...");
  server.send(200, "text/html", SendHTML()); 
}

// HTTP request: other
void handle_NotFound() {
  car_mode = 0;
  Serial.println("Page error");
  server.send(404, "text/plain", "Not found");
}

// control car movement
void car_control() {
  switch (car_mode) {
    case 0: // stop car
      digitalWrite(RIGHT_MOTOR_PIN1, LOW);
      digitalWrite(RIGHT_MOTOR_PIN2, LOW);
      digitalWrite(LEFT_MOTOR_PIN1, LOW);
      digitalWrite(LEFT_MOTOR_PIN2, LOW);
      break;
    case 1: // go forward
      analogWrite(RIGHT_MOTOR_PIN1, RIGHT_MOTOR_SPEED);
      digitalWrite(RIGHT_MOTOR_PIN2, LOW);
      analogWrite(LEFT_MOTOR_PIN1, LEFT_MOTOR_SPEED);
      digitalWrite(LEFT_MOTOR_PIN2, LOW);
      break;
    case 2: // go backward
      digitalWrite(RIGHT_MOTOR_PIN1, LOW);
      analogWrite(RIGHT_MOTOR_PIN2, RIGHT_MOTOR_SPEED);
      digitalWrite(LEFT_MOTOR_PIN1, LOW);
      analogWrite(LEFT_MOTOR_PIN2, LEFT_MOTOR_SPEED);
      break;
    case 3: // turn left
      analogWrite(RIGHT_MOTOR_PIN1, RIGHT_MOTOR_SPEED);
      digitalWrite(RIGHT_MOTOR_PIN2, LOW);
      digitalWrite(LEFT_MOTOR_PIN1, LOW);
      analogWrite(LEFT_MOTOR_PIN2, LEFT_MOTOR_SPEED);
      break;
    case 4: // turn right
      digitalWrite(RIGHT_MOTOR_PIN1, LOW);
      analogWrite(RIGHT_MOTOR_PIN2, RIGHT_MOTOR_SPEED);
      analogWrite(LEFT_MOTOR_PIN1, LEFT_MOTOR_SPEED);
      digitalWrite(LEFT_MOTOR_PIN2, LOW);

  }
}

// output HTML web page for user
String SendHTML() {
  String html = "<!DOCTYPE html>";
  html += "<html>";
  html += "<head>";
  html += "<title>ESP8266 WiFi Car</title>";
  html += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  html += "<style>body {background-color: Moccasin;} h1 {color: SaddleBrown;} h2 {color: Olive;}</style>";
  html += "</head>";
  html += "<body>";
  html += "<div align=\"center\">";
  html += "<h1>ESP8266 WiFi Car</h1>";
  html += "<h2>Press \"stop\" after the server has been reset</h2>";
  html += "<br>\n";
  html += "<form method=\"GET\">";
  html += "<input type=\"button\" value=\"Go forward\" onclick=\"window.location.href='/forward'\">";
  html += "<br><br>\n";
  html += "<input type=\"button\" value=\"Go backward\" onclick=\"window.location.href='/backward'\">";
  html += "<br><br>\n";
  html += "<input type=\"button\" value=\"Turn left\" onclick=\"window.location.href='/left'\">";
  html += "<br><br>\n";
  html += "<input type=\"button\" value=\"Turn right\" onclick=\"window.location.href='/right'\">";
  html += "<br><br>\n";
  html += "<input type=\"button\" value=\"Car stop\" onclick=\"window.location.href='/stop'\">";
  html += "</form>\n";
  html += "</div>\n";
  html += "</body>\n";
  html += "</html>\n";
  return html;
}