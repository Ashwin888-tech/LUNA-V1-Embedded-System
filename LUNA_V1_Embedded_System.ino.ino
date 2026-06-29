/*
------------------------------------------------------------

Project : LUNA V1 - Semi-Humanoid Robot

Embedded Systems Firmware

Maintainer:
Ashwin Bhat

Description:
This firmware implements the embedded control system for
LUNA V1, including servo control, motor control,
gesture routines, and serial command-based robot motion.

------------------------------------------------------------
*/

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// ----------------------
// PCA9685 Servo setup
// ----------------------
Adafruit_PWMServoDriver pca = Adafruit_PWMServoDriver(0x40); // Default I2C address

#define SERVOMIN  150 // Pulse length count for 0 degrees
#define SERVOMAX  600 // Pulse length count for 180 degrees

int servo1 = 0; // PCA9685 channel for servo1
//int servo2 = 1; // PCA9685 channel for servo2

// Convert angle (0-180) to pulse width count
int angle_to_ticks(int angle) {
  return SERVOMIN + (angle * (SERVOMAX - SERVOMIN)) / 180;
}

// ----------------------
// Motor driver pins (L298N)
// ----------------------
const int RPWM_L= 8;
const int LPWM_L= 9;
const int REN_L= 7;
const int LEN_L= 6;
const int RPWM_R= 10;
const int LPWM_R= 11;
const int REN_R= 12;
const int LEN_R= 13;

// ----------------------
// Motor control functions
// ----------------------
void moveForward() {
  analogWrite(RPWM_L, 50);
  analogWrite(LPWM_L, 0);
  analogWrite(RPWM_R, 0);
  analogWrite(LPWM_R, 50);
  
}

void moveBackward() {
  analogWrite(RPWM_L, 0);
  analogWrite(LPWM_L, 50);
  analogWrite(RPWM_R, 50);
  analogWrite(LPWM_R, 0);
}

void stopMotors() {
  analogWrite(RPWM_L, 0);
  analogWrite(LPWM_L, 0);
  analogWrite(RPWM_R, 0);
  analogWrite(LPWM_R, 0);
}

void turnRight() {
  analogWrite(RPWM_L, 50);
  analogWrite(LPWM_L, 0);
  analogWrite(RPWM_R, 50);
  analogWrite(LPWM_R, 0);
}

void turnLeft() {
  analogWrite(RPWM_L, 0);
  analogWrite(LPWM_L, 50);
  analogWrite(RPWM_R, 0);
  analogWrite(LPWM_R, 50);
}

// ----------------------
// Handshake function (two servos)
// ----------------------
void handshake() {
  Serial.println("Performing handshake...");

  // Arm up motion (0° to 90°)
  for (int ang = 0; ang <= 90; ang += 5) {
    int pulse = angle_to_ticks(ang);
    pca.setPWM(servo1, 0, pulse);
    //pca.setPWM(servo2, 0, pulse);
    delay(50);
  }

  delay(2000); // Hold at 90° for 2 seconds

  // Arm down motion (90° to 0°)
  for (int ang = 90; ang >= 0; ang -= 5) {
    int pulse = angle_to_ticks(ang);
    pca.setPWM(servo1, 0, pulse);
    //pca.setPWM(servo2, 0, pulse);
    delay(50);
  }

  delay(1000); // Hold at 0° for 1 second

  Serial.println("Handshake complete.");
}

// ----------------------
// Arduino setup
// ----------------------
void setup() {
  Serial.begin(9600);
  Serial.println("Robot + Handshake Bot Starting...");

  // Motor pins setup
  pinMode(RPWM_L, OUTPUT);
  pinMode(LPWM_L, OUTPUT);
  pinMode(REN_L, OUTPUT);
  pinMode(LEN_L, OUTPUT);
  digitalWrite(REN_L, HIGH);
  digitalWrite(LEN_L, HIGH);

  pinMode(RPWM_R, OUTPUT);
  pinMode(LPWM_R, OUTPUT);
  pinMode(REN_R, OUTPUT);
  pinMode(LEN_R, OUTPUT);
  digitalWrite(REN_R, HIGH);
  digitalWrite(LEN_R, HIGH);

  // Stop motors initially
  stopMotors();

  // PCA9685 initialization
  pca.begin();
  pca.setPWMFreq(50); // 50Hz for analog servo

  delay(500);

  Serial.println("Arduino ready");
  Serial.println("Available commands: front, back, left, right, stop, handshake");
}

// ----------------------
// Arduino main loop
// ----------------------
void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    Serial.print("Received: ");
    Serial.println(command);

    if (command == "front") {
      moveForward();
    } else if (command == "back") {
      moveBackward();
    } else if (command == "left") {
      turnLeft();
    } else if (command == "right") {
      turnRight();
    } else if (command == "stop") {
      stopMotors();
    } else if (command == "handshake") {
      handshake();
    } else {
      Serial.println("Unknown command. Available: front, back, left, right, stop, handshake");
    }
  }
}
