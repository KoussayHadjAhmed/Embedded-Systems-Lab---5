// Master.ino
#include <Wire.h>

#define Redlight 0
#define Yellowlight 1
#define Greenlight 2
#define PedGreen 4
#define PedRed 5

// Vehicle LED pins 
const int trafficRedPin = 8;
const int trafficYellowPin = 9;
const int trafficGreenPin = 10;

// Pedestrian button pin (interrupt)
const int pbutton = 2;

// I2C slave address for pedestrian controller
const uint8_t PED_SLAVE_ADDR = 9;

// State variables (preserve original names/logic)
int state = Redlight;
int pstate = PedRed;
volatile int pressedflag = 0;
int togreenflag = 0;

// For debug
unsigned long lastSerialMillis = 0;

void setup() {
  // Vehicle LEDs
  pinMode(trafficRedPin, OUTPUT);
  pinMode(trafficYellowPin, OUTPUT);
  pinMode(trafficGreenPin, OUTPUT);

  // Pedestrian button input 
  pinMode(pbutton, INPUT); 

  // Attach interrupt on the button pin 
  attachInterrupt(digitalPinToInterrupt(pbutton), buttoninterrupt, RISING);

  // I2C Master
  Wire.begin(); // Master

  Serial.begin(9600);

  // Start state to match original behavior: begin with Redlight in loop logic
  state = Redlight;
  pstate = PedRed;
  togreenflag = 0;
  pressedflag = 0;

  // Ensure vehicle LEDs start off; loop will set the first state visuals
  digitalWrite(trafficRedPin, LOW);
  digitalWrite(trafficYellowPin, LOW);
  digitalWrite(trafficGreenPin, LOW);

  // Ensure slave starts in PedRed by sending 0 at startup
  sendPedCommand(0);
}

void loop() {
  switch(state) {
    case Redlight:
      // In original: changeLED(red,3000); pstate=PedRed; togreenflag = 1; state = Yellowlight;
      // Set vehicle red on for 3000ms then off
      Serial.println("STATE: Redlight (3000 ms)");
      setVehicleLED(trafficRedPin);
      // Set pedestrian state to PedRed and send to slave
      pstate = PedRed;
      sendPedCommand(0);
      togreenflag = 1;
      delay(3000);
      clearVehicleLED(trafficRedPin);
      state = Yellowlight;
      break;

    case Yellowlight:
      // changeLED(yellow,3000);
      Serial.println("STATE: Yellowlight (3000 ms)");
      setVehicleLED(trafficYellowPin);
      delay(3000);
      clearVehicleLED(trafficYellowPin);

      // If togreenflag==1 go to Green, else go to Red and set PedGreen
      if (togreenflag == 1) {
        state = Greenlight;
      } else {
        pstate = PedGreen;
        // Inform slave PedGreen
        sendPedCommand(1);
        state = Redlight;
      }
      break;

    case Greenlight:
      // togreenflag = 0;
      // if pressedflag==1 changeLED(green,3000) else changeLED(green,6000)
      // pressedflag cleared after use
      togreenflag = 0;
      if (pressedflag == 1) {
        Serial.println("STATE: Greenlight (short due to button) 3000 ms");
        setVehicleLED(trafficGreenPin);
        delay(3000);
        clearVehicleLED(trafficGreenPin);
        pressedflag = 0;
      } else {
        Serial.println("STATE: Greenlight (normal) 6000 ms");
        setVehicleLED(trafficGreenPin);
        delay(6000);
        clearVehicleLED(trafficGreenPin);
      }
      state = Yellowlight;
      break;

    default:
      // Fallback: go to Redlight
      state = Redlight;
      break;
  }

  // In the original code the pedestrian outputs were set each loop iteration based on pstate.
  // Here we keep the Slave as the authoritative ped output; we already send commands when pstate changes
  // (we sent in Redlight and in Yellow->Red transition). No continuous polling required.
}

// Helper: set a vehicle LED ON (others OFF)
void setVehicleLED(int ledPin) {
  digitalWrite(trafficRedPin, LOW);
  digitalWrite(trafficYellowPin, LOW);
  digitalWrite(trafficGreenPin, LOW);
  digitalWrite(ledPin, HIGH);
}

void clearVehicleLED(int ledPin) {
  digitalWrite(ledPin, LOW);
}

// Send pedestrian command to slave (1 = PedGreen, 0 = PedRed)
void sendPedCommand(uint8_t cmd) {
  // We only send when needed, but calling repeatedly is harmless; slave will ignore if same state.
  Wire.beginTransmission(PED_SLAVE_ADDR);
  Wire.write(cmd);
  Wire.endTransmission();
  if (millis() - lastSerialMillis > 200) {
    if (cmd == 1) Serial.println("Master -> Slave: Pedestrian GREEN (1)");
    else Serial.println("Master -> Slave: Pedestrian RED (0)");
    lastSerialMillis = millis();
  }
}

// ISR for button (identical behavior to your original ISR)
void buttoninterrupt() {
  // Read pin (same as original)
  int buttonstate = digitalRead(pbutton);
  if (buttonstate == HIGH) {
    pressedflag = 1;
    Serial.println("Master ISR: Button Pressed (pressedflag set)");
  }
}

