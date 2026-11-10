int red = 3;
int yellow = 2;
int green = 1;
int state = 0;

void setup() {
    pinMode(red, OUTPUT);
    pinMode(yellow, OUTPUT);
    pinMode(green, OUTPUT);
}

void loop() {
    switch(state) {
        case 0:
            changeLED(red);
            state = 1; // Move to yellow
            break;
        case 1:
            changeLED(yellow);
            state = 2; // Move to green
            break;
        case 2:
            changeLED(green);
            state = 3; // Move to yellow
            break;
        case 3:
            changeLED(yellow);
            state = 0; // move to red
            break;
    }
}

// function to set LEDs to high then back to low
void changeLED(int LED) {
    digitalWrite(LED, HIGH);
    delay(5000);
    digitalWrite(LED, LOW);
}
