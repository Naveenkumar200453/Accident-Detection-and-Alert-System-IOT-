#include "Wire.h"        
#include "MPU6050.h"
#include <SoftwareSerial.h>
#include <TinyGPS++.h>

MPU6050 mpu;
int16_t ax, ay, az;
int16_t gx, gy, gz;

SoftwareSerial gpsSerial(9, 8);
SoftwareSerial b(5, 4); // tx , rx
TinyGPSPlus gps;

float latitude, longitude;
const int gasSensorPin = A0; // MQ-2 sensor connected to A0
int gasLevel;
const int gasThreshold = 400; // Adjust threshold as needed

void setup() {
  Serial.begin(9600);
  gpsSerial.begin(9600);
  Wire.begin();
  mpu.initialize();

  pinMode(gasSensorPin, INPUT);
  pinMode(12, OUTPUT); // LED
  pinMode(11, OUTPUT); // Buzzer
  b.begin(9600); // Bluetooth
Serial.println("Accident Detected!");
  b.println("Accident Detected!");

  float defaultLat = 10.416753;
  float defaultLon = 77.901203;

  Serial.println("Sending Location...");
  Serial.print("Latitude: "); Serial.println(defaultLat);
  Serial.print("Longitude: "); Serial.println(defaultLon);

  b.println("Sending  Location...");
  b.print("Latitude: "); b.println(defaultLat);
  b.print("Longitude: "); b.println(defaultLon);
}

void loop() {
  // Read MPU data
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  byte x = map(ax, -17000, 17000, 0, 255);
  byte y = map(ay, -17000, 17000, 0, 255);
  byte z = map(az, -17000, 17000, 0, 255);

  gasLevel = analogRead(gasSensorPin);

  // Gas Detection
  if (gasLevel > gasThreshold) {
    Serial.println("Gas Leakage Detected!");
    b.println("Gas Leakage Detected!");
    digitalWrite(12, HIGH);
    digitalWrite(11, HIGH);
    fetchAndSendLocation();
    delay(3000);
  } else {
    digitalWrite(12, LOW);
    digitalWrite(11, LOW);
  }

  // Accident Detection
  if (x < 15 || y > 240 || z < 10 || z > 240) {
    delay(3000); // Confirm reading
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    x = map(ax, -17000, 17000, 0, 255);
    y = map(ay, -17000, 17000, 0, 255);
    z = map(az, -17000, 17000, 0, 255);

    if (x < 15 || y > 240 || z < 10 || z > 240) {
      Serial.println("Accident Detected!");
      b.println("Accident Detected!");
      digitalWrite(12, HIGH);
      digitalWrite(11, HIGH);
      fetchAndSendLocation();
      delay(3000);
    }
  } else {
    digitalWrite(12, LOW);
    digitalWrite(11, LOW);
  }
}

void fetchAndSendLocation() {
  bool locationFound = false;
  unsigned long start = millis();
  
  while (millis() - start < 3000) { // try for 3 seconds
    if (gpsSerial.available()) {
      char c = gpsSerial.read();
      gps.encode(c);

      if (gps.location.isUpdated() && gps.location.isValid()) {
        latitude = gps.location.lat();
        longitude = gps.location.lng();
        locationFound = true;
        break;
      }
    }
  }

  if (!locationFound) {
    gpsFallback(); // Use fixed location
  } else {
    Serial.print("Latitude: "); Serial.println(latitude);
    Serial.print("Longitude: "); Serial.println(longitude);
    b.print("Latitude: "); b.println(latitude);
    b.print("Longitude: "); b.println(longitude);
  }
}

void gpsFallback() {
 
  serial.println("Live GPS not found, sending fallback location...");
  Serial.print("Latitude: "); Serial.println(latitude);
  Serial.print("Longitude: "); Serial.println(longitude);
  b.print("Latitude: "); b.println(latitude);
  b.print("Longitude: "); b.println(longitude);
}