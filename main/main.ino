#include <ESP32Servo.h>  // Servo lib

/************************************************************************************************
                                          CONSTANTS
************************************************************************************************/
//In this project, constants are in upper SNAKE_CASE

// Pins values
const int SWITCH_PIN = 2;
const int HAND_SERVO_PIN = 5;
const int BOX_SERVO_PIN = 6;

// delays values used between servo movement
// the larger the value, the longer it takes to perform servo's movement
const int SLOW = 25;
const int NORMAL = 15;
const int FAST = 5;

// Servos' range movements
const int BOX_CLOSED_POSITION = 70;   // servo angle for a fully closed box
const int BOX_OPENED_POSITION = 110;  // servo angle for a fully opend box

const int HAND_IN_BOX_POSITION = 180;          // servo angle for a fully rested hand
const int HAND_ON_SWITCH_POSITION = 24;        // servo angle for a fully stretched hand
const int HAND_JUST_BEFORE_TOUCH_SWITCH = 30;  // servo angle for the hand to touch the switch, without activating it

/************************************************************************************************
                                          VARIABLES
************************************************************************************************/
// In this project, variables and functions() are in camelCase

// Instancitating Servo objects
Servo handServo;  // Servo responsible for moving the hand
Servo boxServo;   // Servo responsible for opening and closing the box

int switchStatus = 0;  // status that tells if the machine was activated
int randomScene;       // used to select scene
int servoDelay;
bool isFirstMovement = true;  // used to check if it is the first time the machine is being activated

/************************************************************************************************
                                        SUB-ROUTINES
************************************************************************************************/

// generalize servo's movement depending the direction it's going and the delay between movement
void moveServo(Servo myServo, int startPosition, int endPosition, int myDelay) {
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

// open and closes box repeatedly
void boxShake(int startPosition, int endPosition, int cycleSize = 12, int myDelay = NORMAL) {
    for (int turn = 0; turn <= cycleSize; turn++) {
        moveServo(boxServo, startPosition, endPosition, FAST);
        boxServo.write(startPosition);
        delay(myDelay);
    }
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

// Similar with the basic one, but all speeds are random and includes delays between moveServo() functions
void allRandomMovement() {
    boxOpen(random(FAST, SLOW));  // open box
    delay(random(FAST, SLOW));

    handOpen(random(FAST, SLOW));  // hand moves to the switch
    delay(random(FAST, SLOW));

    handClose(random(FAST, SLOW));  // hand goes back to the box
    delay(random(FAST, SLOW));

    boxClose(random(FAST, SLOW));  // closes box
    delay(random(FAST, SLOW));
}

void angryBoxShaker() {
    boxOpen(NORMAL);
    boxShake(BOX_CLOSED_POSITION, BOX_OPENED_POSITION, random(8, 12), FAST);
    boxOpen(NORMAL);
    handOpen(SLOW);
    handClose(FAST);
    boxClose(FAST);
}

void deactivationPretender(int turns = 12, int positionA = 65, int positionB = 40) {
    boxOpen(NORMAL);
    moveServo(handServo, HAND_IN_BOX_POSITION, positionA, NORMAL);

    for (int turn = 0; turn <= turns; turn++) {
        moveServo(handServo, positionA, positionB, NORMAL);
        moveServo(handServo, positionB, positionA, NORMAL);
    }

    handOpen(NORMAL);
    handClose(NORMAL);
    boxClose(NORMAL);
}

/************************************************************************************************
                                            MAIN
************************************************************************************************/

// first function to run. Runs only once
void setup() {
    Serial.begin(9600);

    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    ESP32PWM::allocateTimer(2);
    ESP32PWM::allocateTimer(3);

    handServo.setPeriodHertz(50);
    boxServo.setPeriodHertz(50);

    boxServo.attach(BOX_SERVO_PIN, 1000, 2000);    // define which pin to use to control box servo
    handServo.attach(HAND_SERVO_PIN, 1000, 2000);  // define which pin to use to control hand servo

    boxServo.write(BOX_CLOSED_POSITION);    // initialize box servo at the starting position
    handServo.write(HAND_IN_BOX_POSITION);  // initialize hand servo at the starting position

    randomSeed(analogRead(0));  // seed for random number
}

// runs in infinity loop after setup()
void loop() {
    switchStatus = digitalRead(SWITCH_PIN);  // recieves the current status of the switch, returns HIGH or LOW

    if (switchStatus == LOW) {  // in case the switch was activated
        if (isFirstMovement) {  // make sure to always run the basic movement in the first time machine is activated
            Serial.println("First time nunning!");
            Serial.println("Basic Movement");
            basicMovement();
            isFirstMovement = false;
        } else {
            randomScene = random(1, 4);  // generate random number from 1 to 2

            if (randomScene == 1) {
                Serial.println("Basic Movement");
                basicMovement(random(FAST, SLOW));

            } else if (randomScene == 2) {
                Serial.println("All Random Movement");
                allRandomMovement();

            } else if (randomScene == 3) {
                Serial.println("Angry Box Shaker");
                angryBoxShaker();
            } else if (randomScene == 4) {
                Serial.println("Deactivation Pretender");
                deactivationPretender();
            }
        }
    }

    delay(200);
}
