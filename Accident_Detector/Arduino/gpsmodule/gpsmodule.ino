#include <SoftwareSerial.h>

// Define RX and TX pins
SoftwareSerial sim800l(2, 3); // RX, TX

void setup() {
  // Initialize Serial Monitor
  Serial.begin(9600);
  
  // Initialize SIM800L
  sim800l.begin(9600);
  delay(1000); // Give the module some time to boot
  
  // Set SMS mode to text
  sendCommand("AT+CMGF=1", 1000);
  
  // Send the SMS
  sendSMS("+918144404230", "Hello from Arduino and SIM800L!");
}

void loop() {
  // Do nothing here
}

// Function to send AT commands
void sendCommand(String command, int timeout) {
  sim800l.println(command);
  delay(timeout);
  while (sim800l.available()) {
    Serial.write(sim800l.read());
  }
}

// Function to send SMS
void sendSMS(String phoneNumber, String message) {
  sim800l.print("AT+CMGS=\"");
  sim800l.print("+918144404230");
  sim800l.println("\"");
  delay(1000);
  sim800l.println(message);
  delay(100);
  sim800l.write(26); // ASCII code of CTRL+Z to send the SMS
  delay(1000);
}
