#include <IRremote.h>
//#include "SR04.h"
#include <Servo.h>

// PINs >>>

const int PIN_LED = 13; // Built in LED in Arduino board

// ultrasonic
const int ULTRASONIC_TRIG_PIN = A1;
const int ULTRASONIC_ECHO_PIN = A0;

// tracing
const int PIN_TRACING_RIGHT = A2;
const int PIN_TRACING_CENTER = 10;
const int PIN_TRACING_LEFT = A5;

// infrared
const int PIN_INFRARED = 11;

// wheels
int PIN_WHEELS_ENA = 5;
int PIN_WHEELS_ENB = 6;
int PIN_WHEELS_IN1 = A4;
int PIN_WHEELS_IN2 = A3;
int PIN_WHEELS_IN3 = 7;
int PIN_WHEELS_IN4 = 8;

// <<< PINs

// ultrasonic block >>>
//SR04 ultrasonic = SR04(ULTRASONIC_ECHO_PIN, ULTRASONIC_TRIG_PIN);
// <<< ultrasonic block

// infrared block >>>
IRrecv infrared(PIN_INFRARED);
decode_results infraredResults;
// <<< infrared block

// arm block >>>
Servo armServoMain;
Servo armServoRight;
Servo armServoLeft;
Servo armServoClaw;

int armPositionMain = 80; // 80 - center
int armPositionMainCorrection = -8;

int armPositionRight = 60; // 20 - поднята, 80 - опущена
int armPositionLeft = 140; // 0 - вытянута, 140 - втянута
int armPositionClaw = 0; // 0 - закрыто, 100 - открыто
// <<< arm block

String codeHold = "4294967295";
String one = "16753245";

String two = "16736925";
String three = "16769565";
String four = "16720605";
String five = "16712445";
String six = "16761405";
String seven = "16769055";
String eight = "16754775";
String nine = "16748655";
String zero = "16750695";

String star = "16738455";
String grid = "16756815";
String ok = "16726215";
String up = "16718055";
String down = "16730805";
String left = "16716015";
String right = "16734885";

String codeButtonPushed;
String lastButtonPushedSymbol;

int wheelsSpeed = 108; // from 0 to 255;


int oneMoveBothMs = 130;
int oneMoveSingleMs = 150;

// 0 - empty
// 1 - forward left
// 2 - forward
// 3 - forward right
// 7 - back left
// 8 - back
// 9 - back right
int lastFollowLineMoves[15] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

// Keeping the current state of the wheels >>>
unsigned long leftForwardStarted = 0;
unsigned long leftForwardStopped = 0;
bool leftCurrentlyMovingForward = false;

unsigned long leftBackStarted = 0;
unsigned long leftBackStopped = 0;
bool leftCurrentlyMovingBack = false;

unsigned long rightForwardStarted = 0;
unsigned long rightForwardStopped = 0;
bool rightCurrentlyMovingForward = false;

unsigned long rightBackStarted = 0;
unsigned long rightBackStopped = 0;
bool rightCurrentlyMovingBack = false;
// <<< Keeping the current state of the wheels

// 1 = manual
// 2 = follow the line
int mode = 1;

void setup() {
  Serial.begin(9600);  // speed for the console
  infrared.enableIRIn(); // Start the infrared receiver

  pinMode(PIN_TRACING_RIGHT, INPUT);
  pinMode(PIN_TRACING_CENTER, INPUT);
  pinMode(PIN_TRACING_LEFT, INPUT);

  pinMode(PIN_WHEELS_ENA, OUTPUT);
  pinMode(PIN_WHEELS_ENB, OUTPUT);
  analogWrite(PIN_WHEELS_ENA, wheelsSpeed);
  analogWrite(PIN_WHEELS_ENB, wheelsSpeed);

  pinMode(PIN_WHEELS_IN1, OUTPUT);
  pinMode(PIN_WHEELS_IN2, OUTPUT);
  pinMode(PIN_WHEELS_IN3, OUTPUT);
  pinMode(PIN_WHEELS_IN4, OUTPUT);

  armServoMain.attach(4); // attaches the servo on pin 9 to the servo object
  armServoRight.attach(2);
  armServoLeft.attach(9);
  armServoClaw.attach(3);

  armServoMain.write(armPositionMain + armPositionMainCorrection);
  delay(100);
  armServoLeft.write(armPositionLeft);
  delay(100);
  armServoRight.write(armPositionRight);
  delay(100);
  armServoClaw.write(armPositionClaw);
}

void loop() {
  //checkDistance();
  processIrButtons();
  processFollowLine();
  manageStateOfWheels();
}

void addMoveToLastMovesArray(int move) {
  if (lastFollowLineMoves[0] != move) { 
    lastFollowLineMoves[14] = lastFollowLineMoves[13];
    lastFollowLineMoves[13] = lastFollowLineMoves[12];
    lastFollowLineMoves[12] = lastFollowLineMoves[11];
    lastFollowLineMoves[11] = lastFollowLineMoves[10];
    lastFollowLineMoves[10] = lastFollowLineMoves[9];
    lastFollowLineMoves[9] = lastFollowLineMoves[8];
    lastFollowLineMoves[8] = lastFollowLineMoves[7];
    lastFollowLineMoves[7] = lastFollowLineMoves[6];
    lastFollowLineMoves[6] = lastFollowLineMoves[5];
    lastFollowLineMoves[5] = lastFollowLineMoves[4];
    lastFollowLineMoves[4] = lastFollowLineMoves[3];
    lastFollowLineMoves[3] = lastFollowLineMoves[2];
    lastFollowLineMoves[2] = lastFollowLineMoves[1];
    lastFollowLineMoves[1] = lastFollowLineMoves[0];
    lastFollowLineMoves[0] = move;

    Serial.println(lastFollowLineMoves[0]);
    Serial.println(lastFollowLineMoves[1]);
    Serial.println(lastFollowLineMoves[2]);
    Serial.println(lastFollowLineMoves[3]);
    Serial.println(lastFollowLineMoves[4]);
    Serial.println(lastFollowLineMoves[5]);
    Serial.println(lastFollowLineMoves[6]);
    Serial.println(lastFollowLineMoves[7]);
    Serial.println(lastFollowLineMoves[8]);
    Serial.println(lastFollowLineMoves[9]);
    
    
    Serial.println();
  }
}

//void checkDistance() {
//  long distance = ultrasonic.Distance();
//  Serial.print(distance);
//  Serial.println("cm");
//}

void followLineCheckAndStop() {
  if ((lastFollowLineMoves[0] == 2 && lastFollowLineMoves[1] == 8 && lastFollowLineMoves[2] == 2 && lastFollowLineMoves[3] == 8 && lastFollowLineMoves[4] == 2 && lastFollowLineMoves[5] == 8 && lastFollowLineMoves[6] == 2 && lastFollowLineMoves[7] == 8 && lastFollowLineMoves[8] == 2 && lastFollowLineMoves[9] == 8 && lastFollowLineMoves[10] == 2 && lastFollowLineMoves[11] == 8 && lastFollowLineMoves[12] == 2 && lastFollowLineMoves[13] == 8 && lastFollowLineMoves[14] == 2) || (lastFollowLineMoves[0] == 8 && lastFollowLineMoves[1] == 2 && lastFollowLineMoves[2] == 8 && lastFollowLineMoves[3] == 2 && lastFollowLineMoves[4] == 8 && lastFollowLineMoves[5] == 2 && lastFollowLineMoves[6] == 8 && lastFollowLineMoves[7] == 2 && lastFollowLineMoves[8] == 8 && lastFollowLineMoves[9] == 2 && lastFollowLineMoves[10] == 8 && lastFollowLineMoves[11] == 2 && lastFollowLineMoves[12] == 8 && lastFollowLineMoves[13] == 2 && lastFollowLineMoves[14] == 8)) {
    mode = 1;
    lastFollowLineMoves[0] = 0;
    lastFollowLineMoves[1] = 0;
    lastFollowLineMoves[2] = 0;
    lastFollowLineMoves[3] = 0;
    lastFollowLineMoves[4] = 0;
    lastFollowLineMoves[5] = 0;
    lastFollowLineMoves[6] = 0;
    lastFollowLineMoves[7] = 0;
    lastFollowLineMoves[8] = 0;
    lastFollowLineMoves[9] = 0;
    lastFollowLineMoves[10] = 0;
    lastFollowLineMoves[11] = 0;
    lastFollowLineMoves[12] = 0;
    lastFollowLineMoves[13] = 0;
    lastFollowLineMoves[14] = 0;
  }
}

void processFollowLine() {
  if (mode == 2) {
    // найти черную линию
    // проехать вперед долю секунды

    int center = digitalRead(PIN_TRACING_CENTER);

    if (center == HIGH) {
      addMoveToLastMovesArray(2);
      rightForwardStart();
      leftForwardStart();
    } else {
      bothStop();

      int right = digitalRead(PIN_TRACING_RIGHT);
      int left = digitalRead(PIN_TRACING_LEFT);

      if (right == HIGH && left == LOW) {
        addMoveToLastMovesArray(1);
        leftForwardStart();
        delay(40);
      } else if (left == HIGH && right == LOW) {
        addMoveToLastMovesArray(3);
        rightForwardStart();
        delay(40);
      } else if (left == LOW && right == LOW) {
        addMoveToLastMovesArray(8);
        leftBackStart();
        rightBackStart();
        delay(50);
      } else {
        return;
      }
    }

    followLineCheckAndStop();
  }
}

void processIrButtons() {
  if (infrared.decode( & infraredResults)) {
    codeButtonPushed = String(infraredResults.value);

    Serial.println(codeButtonPushed);

    if (mode == 1 && codeButtonPushed == one) {
      lastButtonPushedSymbol = one;

      rightForwardStart();
    } else if (mode == 1 && codeButtonPushed == two) {
      lastButtonPushedSymbol = two;

      rightForwardStart();
      leftForwardStart();
    } else if (mode == 1 && codeButtonPushed == three) {
      lastButtonPushedSymbol = three;

      leftForwardStart();
    } else if (codeButtonPushed == five) {
      lastButtonPushedSymbol = five;

      if (mode == 1) {
        mode = 2;
      } else {
        mode = 1;
      }

      Serial.println("mode " + String(mode));
    } else if (mode == 1 && codeButtonPushed == seven) {
      lastButtonPushedSymbol = seven;

      rightBackStart();
    } else if (mode == 1 && codeButtonPushed == eight) {
      lastButtonPushedSymbol = eight;

      rightBackStart();
      leftBackStart();
    } else if (mode == 1 && codeButtonPushed == nine) {
      lastButtonPushedSymbol = nine;

      leftBackStart();
    } else if (mode == 1 && codeButtonPushed == left) {
      lastButtonPushedSymbol = left;

      armTurnLeft();
    } else if (mode == 1 && codeButtonPushed == right) {
      lastButtonPushedSymbol = right;

      armTurnRight();
    } else if (mode == 1 && codeButtonPushed == up) {
      lastButtonPushedSymbol = up;

      armForward();
    } else if (mode == 1 && codeButtonPushed == down) {
      lastButtonPushedSymbol = down;

      armBack();
    } else if (mode == 1 && codeButtonPushed == star) {
      lastButtonPushedSymbol = star;

      armUp();
    } else if (mode == 1 && codeButtonPushed == grid) {
      lastButtonPushedSymbol = grid;

      armDown();
    } else if (mode == 1 && codeButtonPushed == ok) {
      lastButtonPushedSymbol = ok;

      if (armPositionClaw == 0) {
        openClaw();
      } else {
        closeClaw();
      }
    }

    if (codeButtonPushed == codeHold) {
      Serial.println(String(millis()) + ": hold " + lastButtonPushedSymbol);

      if (lastButtonPushedSymbol == two) {
        rightForwardStart();
        leftForwardStart();
      } else if (lastButtonPushedSymbol == eight) {
        rightBackStart();
        leftBackStart();
      } else if (lastButtonPushedSymbol == one) {
        rightForwardStart();
      } else if (lastButtonPushedSymbol == three) {
        leftForwardStart();
      } else if (lastButtonPushedSymbol == seven) {
        rightBackStart();
      } else if (lastButtonPushedSymbol == nine) {
        leftBackStart();
      }
    } else {
      Serial.println(String(millis()) + ": pushed " + lastButtonPushedSymbol);
    }

    infrared.resume(); // Receive the next value
  }
}

void manageStateOfWheels() {
  unsigned long currentMillis = millis();

  // if the wheel is moving more than $oneMoveMs ms
  if (leftCurrentlyMovingForward && rightCurrentlyMovingForward && (currentMillis - leftForwardStarted) > oneMoveBothMs && (currentMillis - rightForwardStarted) > oneMoveBothMs) {
    bothStop();
  } else if (leftCurrentlyMovingBack && rightCurrentlyMovingBack && (currentMillis - leftBackStarted) > oneMoveBothMs && (currentMillis - rightBackStarted) > oneMoveBothMs) {
    bothStop();
  } else if (leftCurrentlyMovingForward && (currentMillis - leftForwardStarted) > oneMoveSingleMs) {
    Serial.println(String(currentMillis) + ": leftCurrentlyMovingForward && leftForwardStarted " + String(leftForwardStarted) + "; oneMoveMs " + String(oneMoveSingleMs) + "; working for " + String(currentMillis - leftForwardStarted));

    leftStop();
  } else if (rightCurrentlyMovingForward && (currentMillis - rightForwardStarted) > oneMoveSingleMs) {
    Serial.println(String(currentMillis) + ": rightCurrentlyMovingForward && rightForwardStarted " + String(rightForwardStarted) + "; oneMoveMs " + String(oneMoveSingleMs) + "; working for " + String(currentMillis - rightForwardStarted));

    rightStop();
  } else if (leftCurrentlyMovingBack && (currentMillis - leftBackStarted) > oneMoveSingleMs) {
    Serial.println(String(currentMillis) + ": leftCurrentlyMovingBack && leftBackStarted " + String(leftBackStarted) + "; oneMoveMs " + String(oneMoveSingleMs) + "; working for " + String(currentMillis - leftBackStarted));

    leftStop();
  } else if (rightCurrentlyMovingBack && (currentMillis - rightBackStarted) > oneMoveSingleMs) {
    Serial.println(String(currentMillis) + ": rightCurrentlyMovingBack && rightBackStarted " + String(rightBackStarted) + "; oneMoveMs " + String(oneMoveSingleMs) + "; working for " + String(currentMillis - rightBackStarted));

    rightStop();
  }
}

// Wheels left/right start/stop functions >>>
void rightForwardStart() {
  unsigned long currentMillis = millis();

  Serial.println(String(currentMillis) + ": Right forward start");
  rightForwardStarted = currentMillis;
  rightForwardStopped = 0;
  rightCurrentlyMovingForward = true;
  rightCurrentlyMovingBack = false;

  digitalWrite(PIN_WHEELS_IN4, LOW);
  digitalWrite(PIN_WHEELS_IN3, HIGH);
}

void rightBackStart() {
  unsigned long currentMillis = millis();

  Serial.println(String(currentMillis) + ": Right back start");
  rightBackStarted = currentMillis;
  rightBackStopped = 0;
  rightCurrentlyMovingForward = false;
  rightCurrentlyMovingBack = true;

  digitalWrite(PIN_WHEELS_IN3, LOW);
  digitalWrite(PIN_WHEELS_IN4, HIGH);
}

void leftForwardStart() {
  unsigned long currentMillis = millis();

  Serial.println(String(currentMillis) + ": Left forward start");
  leftForwardStarted = currentMillis;
  leftForwardStopped = 0;
  leftCurrentlyMovingForward = true;

  digitalWrite(PIN_WHEELS_IN2, LOW);
  digitalWrite(PIN_WHEELS_IN1, HIGH);
}

void leftBackStart() {
  unsigned long currentMillis = millis();

  Serial.println(String(currentMillis) + ": Left back start");
  leftBackStarted = currentMillis;
  leftBackStopped = 0;
  leftCurrentlyMovingForward = false;
  leftCurrentlyMovingBack = true;

  digitalWrite(PIN_WHEELS_IN1, LOW);
  digitalWrite(PIN_WHEELS_IN2, HIGH);
}

void leftStop() {
  leftCurrentlyMovingForward = false;
  leftCurrentlyMovingBack = false;

  digitalWrite(PIN_WHEELS_IN1, LOW);
  digitalWrite(PIN_WHEELS_IN2, LOW);

  Serial.println(String(millis()) + ": Left stopped");
}

void rightStop() {
  rightCurrentlyMovingForward = false;
  rightCurrentlyMovingBack = false;

  digitalWrite(PIN_WHEELS_IN3, LOW);
  digitalWrite(PIN_WHEELS_IN4, LOW);

  Serial.println(String(millis()) + ": Right stopped");
}

void bothStop() {
  digitalWrite(PIN_WHEELS_IN1, LOW);
  digitalWrite(PIN_WHEELS_IN2, LOW);
  digitalWrite(PIN_WHEELS_IN3, LOW);
  digitalWrite(PIN_WHEELS_IN4, LOW);

  rightCurrentlyMovingForward = false;
  rightCurrentlyMovingBack = false;
  leftCurrentlyMovingForward = false;
  leftCurrentlyMovingBack = false;

  Serial.println(String(millis()) + ": Both stopped");
}

// <<< Wheels left/right start/stop functions

// Arm functions >>>

void armTurnRight() {
  Serial.println("Turn right\n");

  int minPos = 35;
  int targetPos = armPositionMain + armPositionMainCorrection - 45;

  if (targetPos < minPos) {
    targetPos = minPos;
  }

  for (armPositionMain; armPositionMain > targetPos; armPositionMain--) {
    armServoMain.write(armPositionMain);
    delay(5); // delay 5ms（used to adjust the servo speed）
  }

  delay(100);
}

void armTurnLeft() {
  Serial.println("Turn left\n");

  int maxPos = 125;
  int targetPos = armPositionMain + armPositionMainCorrection + 45;

  if (targetPos > maxPos) {
    targetPos = maxPos;
  }

  for (armPositionMain; armPositionMain < targetPos; armPositionMain++) {
    armServoMain.write(armPositionMain);
    delay(5);
  }

  delay(100);
}

void openClaw() {
  Serial.println("Open claw\n");

  for (armPositionClaw; armPositionClaw < 50; armPositionClaw++) {
    armServoClaw.write(armPositionClaw);
  }

  delay(100);
}

void closeClaw() {
  Serial.println("Close claw\n");

  for (armPositionClaw; armPositionClaw > 0; armPositionClaw--) {
    armServoClaw.write(armPositionClaw);
  }

  delay(100);
}

void liftUp() {
  Serial.println("Lift up\n");

  for (armPositionLeft; armPositionLeft < 120; armPositionLeft++) {
    armServoLeft.write(armPositionLeft);
    delay(5);
  }

  delay(1000);
}

void liftDown() {
  Serial.println("Lift down\n");

  for (armPositionLeft; armPositionLeft > 50; armPositionLeft--) {
    armServoLeft.write(armPositionLeft);
    delay(5);
  }

  delay(1000);
}

void leftServo5() {
  // left servo rotates to 5 degrees
  Serial.println("left servo rotates to 5 degrees\n");

  for (armPositionLeft; armPositionLeft > 50; armPositionLeft--) {
    armServoLeft.write(armPositionLeft);
    delay(5);
  }

  delay(1500);
}

void rightServo100() {
  // right servo rotates to 100 degrees
  for (armPositionRight; armPositionRight > 50; armPositionRight--) {
    armServoRight.write(armPositionRight);
    delay(5);
  }
}

void leftServo120() {
  // left servo rotates to100 degrees, rocker arm lifts.
  Serial.println("left servo rotates to100 degrees, rocker arm lifts\n");
  for (armPositionLeft; armPositionLeft < 120; armPositionLeft++) {
    armServoLeft.write(armPositionLeft);
    delay(5);
  }

  delay(1000);
}

// armPositionLeft: 0 - вытянута, 140 - втянута
void armForward() {
  Serial.println("Arm forward\n");

  int minPos = 0;
  int targetPos = armPositionLeft - 46;

  if (targetPos < minPos) {
    targetPos = minPos;
  }

  for (armPositionLeft; armPositionLeft > targetPos; armPositionLeft--) {
    armServoLeft.write(armPositionLeft);
    delay(5); // delay 5ms（used to adjust the servo speed）
  }

  delay(100);
}

void armBack() {
  Serial.println("Arm back\n");

  int maxPos = 140;
  int targetPos = armPositionLeft + 46;

  if (targetPos > maxPos) {
    targetPos = maxPos;
  }

  for (armPositionLeft; armPositionLeft < targetPos; armPositionLeft++) {
    armServoLeft.write(armPositionLeft);
    delay(5); // delay 5ms（used to adjust the servo speed）
  }

  delay(100);
}

// armPositionRight: 20 - поднята, 80 - опущена
void armUp() {
  Serial.println("Arm up\n");

  int minPos = 20;
  int targetPos = armPositionRight - 40;

  if (targetPos < minPos) {
    targetPos = minPos;
  }

  Serial.println("Arm up to " + String(targetPos));

  for (armPositionRight; armPositionRight > targetPos; armPositionRight--) {
    Serial.println("Arm right " + String(armPositionRight));
    armServoRight.write(armPositionRight);
    delay(5); // delay 5ms（used to adjust the servo speed）
  }

  delay(100);
}

void armDown() {
  Serial.println("Arm down\n");

  int maxPos = 80;
  int targetPos = armPositionRight + 40;

  if (targetPos > maxPos) {
    targetPos = maxPos;
  }

  Serial.println("Arm down to " + String(targetPos));

  for (armPositionRight; armPositionRight < targetPos; armPositionRight++) {
    Serial.println("Arm right " + String(armPositionRight));
    armServoRight.write(armPositionRight);
    delay(5); // delay 5ms（used to adjust the servo speed）
  }

  delay(100);
}

// <<< Arm functions
