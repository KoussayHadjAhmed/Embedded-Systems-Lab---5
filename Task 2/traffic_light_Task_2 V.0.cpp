#define Redlight 0
#define Yellowlight 1
#define Greenlight 2
#define YellowtoRedlight 3
#define PedGreen 4
#define PedRed 5

int red = 3;
int yellow = 6;
int green = 7;
int pred = 4;
int pgreen = 5;
int pbutton =2;

int state = Redlight;
int pstate = PedGreen;
int time;
volatile int buttonstate;
int pressedflag = 0;
int togreenflag = 0;
void changeLED();
  
void setup() {
    pinMode(red, OUTPUT);
    pinMode(yellow, OUTPUT);
    pinMode(green, OUTPUT);
    pinMode(pgreen, OUTPUT);
    pinMode(pred, OUTPUT);
    pinMode(pbutton, INPUT);
    attachInterrupt(digitalPinToInterrupt(pbutton),buttoninterrupt,RISING);
  Serial.begin(9600);
}

void loop() {
    switch(state) {
        case Redlight:
            changeLED(red,10000);
            pstate=PedRed;
      		togreenflag = 1;
            state = Yellowlight;// Move to yellow
            break;
			
        case Yellowlight:
            changeLED(yellow,5000);
            if(togreenflag == 1){
            	state = Greenlight;
            }else{
              	pstate=PedGreen;
            	state = Redlight;
            }
//            state = Greenlight; // Move to green
            break;
			
        case Greenlight:
      		togreenflag = 0;
			if(pressedflag == 1){
				changeLED(green,5000);
              	pressedflag = 0;
			}else{
				changeLED(green,10000);        
			}
            state = Yellowlight; // Move to yellow
            break;
			
        //case YellowtoRedlight:
        //    changeLED(yellow,5000);
        //    pstate=PedGreen;            
        //    state = Redlight; // Move from yellow to red
        //    break;
    }
  	 switch(pstate){
		case PedRed:
		digitalWrite(pgreen, LOW);
		digitalWrite(pred, HIGH);    
		break;

		case PedGreen:
		digitalWrite(pgreen, HIGH);
		digitalWrite(pred, LOW);
		break;
}
}
// function to set LEDs to high then back to low
void changeLED(int LED, int time) {
    digitalWrite(LED, HIGH);
    delay(time);
    digitalWrite(LED, LOW);
}

void buttoninterrupt() { //ISR
    
 buttonstate = digitalRead(pbutton);
  if(buttonstate == HIGH){
    pressedflag = 1;
     Serial.println("Button Pressed");
    }
  
}