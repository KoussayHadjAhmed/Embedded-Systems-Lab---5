// Slave.ino
#include <Wire.h>

// Pedestrian Light Pins (as in your simplified slave)
const int pedestrianGreenPin = 7;
const int pedestrianRedPin = 6;

// I2C address for this slave
const uint8_t SLAVE_ADDR = 9;

void setup() {
  Wire.begin(SLAVE_ADDR);  // Initialize as Slave with address 9
  Wire.onReceive(receiveTrafficCommand);  // Register event to receive commands from Master

  pinMode(pedestrianGreenPin, OUTPUT);
  pinMode(pedestrianRedPin, OUTPUT);

  // Start with pedestrian light red
  digitalWrite(pedestrianGreenPin, LOW);
  digitalWrite(pedestrianRedPin, HIGH);

  Serial.begin(9600);
  Serial.println("Pedestrian Slave started. Waiting for master commands...");
}

void loop() {
  // Nothing required in loop; update happens in receiveTrafficCommand
  // Could add a timeout/heartbeat to enter fail-safe (blinking) if desired.
  delay(50);
}

void receiveTrafficCommand(int numBytes) {
  // Receive command from Master
  while (Wire.available()) {
    int command = Wire.read();
    if (command == 1) {
      // Pedestrians allowed to cross
      digitalWrite(pedestrianRedPin, LOW);
      digitalWrite(pedestrianGreenPin, HIGH);
      Serial.println("Slave: Pedestrian light GREEN (cross).");
    } else if (command == 0) {
      // Pedestrian crossing end
      digitalWrite(pedestrianGreenPin, LOW);
      digitalWrite(pedestrianRedPin, HIGH);
      Serial.println("Slave: Pedestrian light RED (stop).");
    } else {
      // Unknown command: ignore
      Serial.print("Slave: Unknown command received: ");
      Serial.println(command);
    }
  }
}
