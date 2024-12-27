#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// WiFi credentials
const char* ssid = "Techome1";
const char* password = "sure1173";

// Flask server endpoint
const char* serverUrl = "http://192.168.1.25:5000/update-data"; // Replace <Your_PC_IP> with your Flask server's IP address

// Pin definitions
const int pinMachineOn = D0;
const int pinCycleStart = D1;
const int pinCycleStop = D2;

// Variables to store cycle times


// Flags for tracking states
bool isCycleRunning = false;

void setup() {
  Serial.begin(115200);

  // Configure pins as input
  pinMode(pinMachineOn, INPUT);
  pinMode(pinCycleStart, INPUT);
  pinMode(pinCycleStop, INPUT);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Read pin states
  int machineOnState = digitalRead(pinMachineOn);
  int cycleStartState = digitalRead(pinCycleStart);
  int cycleStopState = digitalRead(pinCycleStop);


sendDataToServer(machineOnState, cycleStartState, cycleStopState, 9); // Send data when machine is off



  delay(1000); // Small delay for stability
}

void sendDataToServer(int machineStatus, int startTime, int endTime, int duration) {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client; // Create a WiFiClient instance
    HTTPClient http;
    http.begin(client, serverUrl); // Pass the client and the server URL
    http.addHeader("Content-Type", "application/json");

    // Create JSON payload
    String jsonPayload = "{";
    jsonPayload += "\"machine_status\":\"" + String(machineStatus == HIGH ? "ON" : "OFF") + "\",";
    jsonPayload += "\"cycle\":\"" + String(startTime == HIGH ? "START" : "STOP") + "\",";
    jsonPayload += "\"cycle_time\":\"" + String(endTime == HIGH ? "5.00" : "0.00") + "\",";
    jsonPayload += "\"qty_ms\":" + String(duration);
    jsonPayload += "}";

    // Send POST request
    int httpResponseCode = http.POST(jsonPayload);

    // Print response
    if (httpResponseCode > 0) {
      Serial.println("Data sent successfully!");
      Serial.println(http.getString());
    } else {
      Serial.print("Error sending data. HTTP code: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("WiFi not connected!");
  }
}
