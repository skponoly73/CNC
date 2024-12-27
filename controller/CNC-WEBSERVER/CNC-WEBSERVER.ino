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

unsigned long cycleStartTime = 0; // Time when the cycle starts
unsigned long cycleStopTime = 0;  // Time when the cycle stops
unsigned long cycleDuration = 0; // Duration of the cycle
int cycleCount = 0;              // Counter for completed cycles

bool isCycleRunning = false;     // Track if a cycle is in progress

// Variables to store cycle times


static int machine_count = 0;
static unsigned long duration = 0;

static unsigned long qty = 0;



// Flags for tracking states

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
  // Read pin states
  int machineOnState = digitalRead(pinMachineOn);
  int cycleStartState = digitalRead(pinCycleStart);
  int cycleStopState = digitalRead(pinCycleStop);

  // If the cycle starts
  if (cycleStartState == HIGH) {
    if (cycleStartTime == 0) { // Record the start time only once
      cycleStartTime = millis() / 1000; // Convert milliseconds to seconds
    }

    unsigned long currentCycleTime = millis() / 1000; // Current time in seconds
     duration = currentCycleTime - cycleStartTime; // Calculate duration

    Serial.print("Cycle duration (seconds): ");
    Serial.println(duration);
    

    // Send data to the server
    sendDataToServer(machineOnState, 1, duration, 0);
  }

  // Reset the cycle start time when the cycle ends
  if (cycleStartState == LOW) {
   // cycleStartTime = 0; // Reset start time
   // machine_count++;
       if (cycleStartTime != 0) {  // Only reset when cycle was started
      cycleStartTime = 0;  // Reset start time
      qty++;         // Increment the cycle count

      Serial.print("Cycle ended. Total cycles: ");
      Serial.println(qty);

      // Send the cycle count to the server
     sendDataToServer(machineOnState, 0,duration, qty);
    }
   

    //sendDataToServer(machineOnState, 0,duration, 1);
  }

  delay(1000); // Delay for stability

/*
  // If machine is ON and a cycle stops
  if (machineOnState == HIGH && cycleStopState == HIGH && isCycleRunning) {
    cycleStopTime = millis(); // Record stop time
    cycleDuration = cycleStopTime - cycleStartTime; // Calculate duration
    cycleCount++; // Increment the cycle count

    // Send data to the server
    sendDataToServer(machineOnState, cycleStartTime, cycleStopTime, cycleCount);

    isCycleRunning = false; // Reset cycle status
  }
*/
  // Small delay for stability
  delay(100);
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
    jsonPayload += "\"cycle_time\":\"" + String(endTime) + "\",";
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
