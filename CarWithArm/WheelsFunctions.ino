void manageStateOfWheels() {
  unsigned long currentMillis = millis();

  // if the wheel is moving more than $oneMoveMs ms
  if (leftCurrentlyMovingForward && rightCurrentlyMovingForward && (currentMillis - leftForwardStarted) > oneMoveBothMs && (currentMillis - rightForwardStarted) > oneMoveBothMs) {
    bothStop();
  } else if (leftCurrentlyMovingBack && rightCurrentlyMovingBack && (currentMillis - leftBackStarted) > oneMoveBothMs && (currentMillis - rightBackStarted) > oneMoveBothMs) {
    bothStop();
  } else if (leftCurrentlyMovingForward && (currentMillis - leftForwardStarted) > oneMoveSingleMs) {
    //Serial.println(String(currentMillis) + ": leftCurrentlyMovingForward && leftForwardStarted " + String(leftForwardStarted) + "; oneMoveMs " + String(oneMoveSingleMs) + "; working for " + String(currentMillis - leftForwardStarted));

    leftStop();
  } else if (rightCurrentlyMovingForward && (currentMillis - rightForwardStarted) > oneMoveSingleMs) {
    //Serial.println(String(currentMillis) + ": rightCurrentlyMovingForward && rightForwardStarted " + String(rightForwardStarted) + "; oneMoveMs " + String(oneMoveSingleMs) + "; working for " + String(currentMillis - rightForwardStarted));

    rightStop();
  } else if (leftCurrentlyMovingBack && (currentMillis - leftBackStarted) > oneMoveSingleMs) {
    //Serial.println(String(currentMillis) + ": leftCurrentlyMovingBack && leftBackStarted " + String(leftBackStarted) + "; oneMoveMs " + String(oneMoveSingleMs) + "; working for " + String(currentMillis - leftBackStarted));

    leftStop();
  } else if (rightCurrentlyMovingBack && (currentMillis - rightBackStarted) > oneMoveSingleMs) {
    //Serial.println(String(currentMillis) + ": rightCurrentlyMovingBack && rightBackStarted " + String(rightBackStarted) + "; oneMoveMs " + String(oneMoveSingleMs) + "; working for " + String(currentMillis - rightBackStarted));

    rightStop();
  }
}

// Wheels left/right start/stop functions >>>
void rightForwardStart() {
  unsigned long currentMillis = millis();

  //Serial.println(String(currentMillis) + ": Right forward start");
  rightForwardStarted = currentMillis;
  rightForwardStopped = 0;
  rightCurrentlyMovingForward = true;
  rightCurrentlyMovingBack = false;

  digitalWrite(PIN_WHEELS_IN4, LOW);
  digitalWrite(PIN_WHEELS_IN3, HIGH);
}

void rightBackStart() {
  unsigned long currentMillis = millis();

  //Serial.println(String(currentMillis) + ": Right back start");
  rightBackStarted = currentMillis;
  rightBackStopped = 0;
  rightCurrentlyMovingForward = false;
  rightCurrentlyMovingBack = true;

  digitalWrite(PIN_WHEELS_IN3, LOW);
  digitalWrite(PIN_WHEELS_IN4, HIGH);
}

void leftForwardStart() {
  unsigned long currentMillis = millis();

  //Serial.println(String(currentMillis) + ": Left forward start");
  leftForwardStarted = currentMillis;
  leftForwardStopped = 0;
  leftCurrentlyMovingForward = true;

  digitalWrite(PIN_WHEELS_IN2, LOW);
  digitalWrite(PIN_WHEELS_IN1, HIGH);
}

void leftBackStart() {
  unsigned long currentMillis = millis();

  //Serial.println(String(currentMillis) + ": Left back start");
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

  //Serial.println(String(millis()) + ": Left stopped");
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

  //Serial.println(String(millis()) + ": Both stopped");
}

// <<< Wheels left/right start/stop functions