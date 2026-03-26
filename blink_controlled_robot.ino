// ===== BLINK CONTROLLED ROBOT =====

const int ecgPin = A0;

// L298N motor pins
const int IN1 = 5;
const int IN2 = 6;
const int IN3 = 9;
const int IN4 = 10;

// Signal processing
int raw = 0;
int smooth = 0;
int baseline = 0;
int spikeThreshold = 80;

// Blink logic
int blinkCount = 0;
unsigned long firstBlinkTime = 0;
unsigned long lastBlinkTime = 0;

// Motor timing
unsigned long motorStartTime = 0;
unsigned long motorRunTime = 0;
bool motorActive = false;

void setup() {
  Serial.begin(9600);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  stopCar();

  // Baseline calibration
  long sum = 0;
  for (int i = 0; i < 100; i++) {
    sum += analogRead(ecgPin);
    delay(10);
  }
  baseline = sum / 100;
}

void loop() {

  raw = analogRead(ecgPin);
  smooth = (smooth * 7 + raw) / 8;
  baseline = (baseline * 99 + smooth) / 100;

  int diff = abs(smooth - baseline);

  // Debug
  Serial.print("Diff: ");
  Serial.println(diff);

  // Blink detection
  if (diff > spikeThreshold && millis() - lastBlinkTime > 600) {

    Serial.println("Blink detected!");

    if (blinkCount == 0) {
      firstBlinkTime = millis();
    }

    blinkCount++;
    lastBlinkTime = millis();
  }

  // Decision logic
  if (blinkCount > 0 && millis() - firstBlinkTime > 2000) {

    Serial.print("Blink Count: ");
    Serial.println(blinkCount);

    if (blinkCount == 1) {
      moveForward();
      motorRunTime = 1200;
    }
    else if (blinkCount == 2) {
      moveBackward();
      motorRunTime = 1200;
    }
    else if (blinkCount == 3) {
      turnRight();
      motorRunTime = 800;
    }
    else if (blinkCount == 4) {
      turnLeft();
      motorRunTime = 800;
    }
    else if (blinkCount >= 5) {
      stopCar();
      motorActive = false;
    }

    motorStartTime = millis();
    motorActive = true;
    blinkCount = 0;
  }

  // Auto stop
  if (motorActive && millis() - motorStartTime > motorRunTime) {
    stopCar();
    motorActive = false;
  }

  delay(20);
}

// Motor functions
void moveForward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void moveBackward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void turnRight() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void turnLeft() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void stopCar() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
