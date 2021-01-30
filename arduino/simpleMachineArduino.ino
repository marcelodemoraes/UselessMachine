#include <Servo.h>  // Servo lib

/************************************************************************************************
                                          CONSTANTS
************************************************************************************************/
//In this project, constants are in upper SNAKE_CASE

// Pins values
const int SWITCH_PIN = 2;
const int HAND_SERVO_PIN = 5;
const int BOX_SERVO_PIN = 6;

// const int LED_PIN = 2;

// delays values used between servo movement
// the larger the value, the longer it takes to perform servo's movement
const int SLOW = 25;
const int NORMAL = 15;
const int FAST = 6;

// Servos' range movements
const int BOX_CLOSED_POSITION = 75;   // servo angle for a fully closed box
const int BOX_OPENED_POSITION = 113;  // servo angle for a fully opend box

const int HAND_IN_BOX_POSITION = 0;          // servo angle for a fully rested hand
const int HAND_ON_SWITCH_POSITION = 141;        // servo angle for a fully stretched hand
const int HAND_JUST_BEFORE_TOUCH_SWITCH = 30;  // servo angle for the hand to touch the switch, without activating it

/************************************************************************************************
                                          VARIABLES
************************************************************************************************/
// In this project, variables and functions() are in camelCase

// Instancitating Servo objects
Servo handServo;  // Servo responsible for moving the hand
Servo boxServo;   // Servo responsible for opening and closing the box

int switchStatus = 0;  // status that tells if the machine was activated

/************************************************************************************************
                                        SUB-ROUTINES
************************************************************************************************/

// generalize servo's movement depending the direction it's going and the delay between movement
void moveServo(Servo myServo, int startPosition, int endPosition, int myDelay = NORMAL) {
    // in case it's an increasing position movement
    if (startPosition <= endPosition) {
        // position is the current servo's shaft angle
        for (int position = startPosition; position <= endPosition; position++) {
            myServo.write(position);
            delay(myDelay);
        }
    }
    // in case it's a decreasing position movement
    else {
        for (int position = startPosition; position >= endPosition; position--) {
            myServo.write(position);
            delay(myDelay);
        }
    }
}

void boxOpen(int myDelay = NORMAL) {
    moveServo(boxServo, boxServo.read(), BOX_OPENED_POSITION, myDelay);
}

void boxClose(int myDelay = NORMAL) {
    moveServo(boxServo, boxServo.read(), BOX_CLOSED_POSITION, myDelay);
}

void handOpen(int myDelay = NORMAL) {
    moveServo(handServo, handServo.read(), HAND_ON_SWITCH_POSITION, myDelay);
}

void handClose(int myDelay = NORMAL) {
    moveServo(handServo, handServo.read(), HAND_IN_BOX_POSITION, myDelay);
}

/************************************************************************************************
                                            SCENES
************************************************************************************************/

// Standard movement: open box, box's hand deactivates the switch
// box's hand goes back to resting position and finally box closes
// first time machine is activated, always normal speed, later is random
void basicMovement(int myDelay = NORMAL) {
    boxOpen(myDelay);    // open box
    handOpen(myDelay);   // hand moves to the switch
    handClose(myDelay);  // hand goes back to the box
    boxClose(myDelay);   // closes box
}

/************************************************************************************************
                                            MAIN
************************************************************************************************/

// first function to run. Runs only once
void setup() {
    Serial.begin(9600);

    boxServo.attach(BOX_SERVO_PIN);    // define which pin to use to control box servo
    handServo.attach(HAND_SERVO_PIN);  // define which pin to use to control hand servo
  
    boxServo.write(BOX_CLOSED_POSITION);    // initialize box servo at the starting position
    handServo.write(HAND_IN_BOX_POSITION);  // initialize hand servo at the starting position

}

// runs in infinity loop after setup()
void loop() {
    switchStatus = digitalRead(SWITCH_PIN);  // recieves the current status of the switch, returns HIGH or LOW

    if (switchStatus == HIGH) {  // in case the switch was activated
        digitalWrite(LED_BUILTIN, HIGH);
        Serial.println("Basic Movement");
        basicMovement(FAST);
        digitalWrite(LED_BUILTIN, LOW);
    }

    delay(200);
}
