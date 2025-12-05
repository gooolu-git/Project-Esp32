#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <DHT.h>

// ---------------- WiFi Credentials ----------------
#define WIFI_SSID "HOTSPOT"
#define WIFI_PASSWORD "aaaaaaaaa"

// ---------------- Firebase Credentials ----------------
#define API_KEY "AIzaSyCpxdj3ayV9qMycIRiebLdDNMZMvV4uTKc"
#define DATABASE_URL "https://project-esp32-cbe7f-default-rtdb.firebaseio.com/"

// Firebase Authentication
#define USER_EMAIL "goooluraja@gmail.com"
#define USER_PASSWORD "#Golu@1233GecArwal"

// ---------------- Pin Config ----------------
#define DHTPIN 4
#define DHTTYPE DHT11
#define MOISTURE_PIN 34 // Soil moisture analog pin
#define WATER_LEVEL_PIN 35 // Water level analog pin
#define MOTOR_PIN 5
#define LED_BUILTIN_PIN 2 // Built-in LED on ESP32 DevKit V1

// ---------------- Objects ----------------
DHT dht(DHTPIN, DHTTYPE);
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// ---------------- Timing ----------------
unsigned long previousMillis = 0;
const long sensorInterval = 1000; // 1 second
const long wifiReconnectInterval = 5000; // 5 seconds

void setup() {
  Serial.begin(115200);
  delay(500);

  // --- Pin setup ---
  pinMode(MOTOR_PIN, OUTPUT);
  digitalWrite(MOTOR_PIN, HIGH); // Relay OFF at start (active LOW)

  pinMode(LED_BUILTIN_PIN, OUTPUT);
  digitalWrite(LED_BUILTIN_PIN, LOW);

  // --- Wi-Fi ---
  WiFi.mode(WIFI_STA);
  Serial.print("Connecting to Wi-Fi: ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  unsigned long wifiStart = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - wifiStart < 20000) { // 20 sec timeout
    Serial.print(".");
    delay(500);
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWi-Fi Connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nWi-Fi Failed. Will retry in loop.");
  }

  // --- Firebase ---
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  dht.begin();
  Serial.println("Firebase and DHT initialized.");
}

void loop() {
  // --- Wi-Fi reconnect ---
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi not connected. Attempting reconnect...");
    WiFi.disconnect();
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    unsigned long reconnectStart = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - reconnectStart < wifiReconnectInterval) {
      Serial.print(".");
      delay(500);
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\nWi-Fi Reconnected!");
      Serial.print("IP Address: ");
      Serial.println(WiFi.localIP());
    } else {
      Serial.println("\nReconnect failed. Will retry in next loop.");
      delay(wifiReconnectInterval);
      return; // skip this loop cycle
    }
  }

  // --- Motor Control from Firebase (Active LOW relay) ---
  if (Firebase.RTDB.getString(&fbdo, "/ESP32/commands/motor")) {
    String motorCommand = fbdo.stringData();
    if (motorCommand == "ON") {
      digitalWrite(MOTOR_PIN, LOW); // Active LOW: Motor ON
      Serial.println("Motor ON");
    } else if (motorCommand == "OFF") {
      digitalWrite(MOTOR_PIN, HIGH); // Active LOW: Motor OFF
      Serial.println("Motor OFF");
    }
  }

  // --- LED Control from Firebase (Active HIGH onboard LED) ---
  if (Firebase.RTDB.getString(&fbdo, "/ESP32/commands/led")) {
    String ledCommand = fbdo.stringData();
    if (ledCommand == "ON") {
      digitalWrite(LED_BUILTIN_PIN, HIGH);
      Serial.println("LED ON");
    } else if (ledCommand == "OFF") {
      digitalWrite(LED_BUILTIN_PIN, LOW);
      Serial.println("LED OFF");
    }
  }

  // --- Sensor Reading at 1-second intervals ---
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= sensorInterval) {
    previousMillis = currentMillis;

    int moistureValue = analogRead(MOISTURE_PIN);
    int waterLevel = analogRead(WATER_LEVEL_PIN);
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    // Validate DHT readings
    if (isnan(temperature) || isnan(humidity)) {
      Serial.println("Failed to read DHT sensor!");
      return; // skip upload
    }

    Serial.printf("Moisture: %d | Water Level: %d | Temp: %.2f | Humidity: %.2f\n",
                  moistureValue, waterLevel, temperature, humidity);

    // Upload to Firebase if ready
    if (Firebase.ready()) {
      Firebase.RTDB.setInt(&fbdo, "/ESP32/Moisture", moistureValue);
      Firebase.RTDB.setInt(&fbdo, "/ESP32/WaterLevel", waterLevel);
      Firebase.RTDB.setFloat(&fbdo, "/ESP32/Temperature", temperature);
      Firebase.RTDB.setFloat(&fbdo, "/ESP32/Humidity", humidity);
    }
  }
}