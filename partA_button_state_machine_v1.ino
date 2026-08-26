// A2 Part A v1: button driven state machine and non-blocking timing.
// This file runs by itself, but it is intended to be merged with Parts B/C.
// Integration hint: systemState, lastUpdate and sharedValue are deliberately generic.
// Before integrating this file, write down:
// - which button means "start" in the final system,
// - which button means "cancel" in the final system,
// - whether the button wiring is active-low or active-high,
// - which state numbers other parts are expecting,
// - which pin conflicts must be resolved,
// - which variables should be renamed,
// - which Serial messages are still useful,
// - and what evidence will prove this module works.
// sovunear[hwuabobv]

const int armButtonPin = 2;
const int cancelButtonPin = 4;
const int outputPin = 7;

int systemState = 1;          // 1 idle, 2 starting, 3 active, 4 stopping
int sharedValue = 0;          // other parts may want to use this name differently
unsigned long lastUpdate = 0; // reused as the state timer in this sketch

int previousArm = HIGH;
int previousCancel = HIGH;
unsigned long startWait = 4000;
unsigned long stopWait = 6000;

void setup() {
  pinMode(armButtonPin, INPUT_PULLUP);
  pinMode(cancelButtonPin, INPUT_PULLUP);
  pinMode(outputPin, OUTPUT);
  Serial.begin(9600);
  announceState("idle and waiting");
}

void loop() {
  readButtons();
  updateStateMachine();
  updateOutput();
  printHeartbeat();
}

void readButtons() {
  int armNow = digitalRead(armButtonPin);
  int cancelNow = digitalRead(cancelButtonPin);

  if (previousArm == HIGH && armNow == LOW && systemState == 1) {
    systemState = 2;
    lastUpdate = millis();
    announceState("starting");
  }

  if (previousCancel == HIGH && cancelNow == LOW) {
    systemState = 4;
    lastUpdate = millis();
    announceState("stopping");
  }

  previousArm = armNow;
  previousCancel = cancelNow;
}

void updateStateMachine() {
  if (systemState == 2 && millis() - lastUpdate >= startWait) {
    systemState = 3;
    sharedValue++;
    announceState("active");
  }

  if (systemState == 4 && millis() - lastUpdate >= stopWait) {
    systemState = 1;
    announceState("idle again");
  }
}

void updateOutput() {
  if (systemState == 1 || systemState == 4) {
    digitalWrite(outputPin, LOW);
  } else {
    digitalWrite(outputPin, HIGH);
  }
}

void announceState(String text) {
  Serial.print("Part A v1 state ");
  Serial.print(systemState);
  Serial.print(": ");
  Serial.println(text);
}

void printHeartbeat() {
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint >= 1000) {
    lastPrint = millis();
    Serial.print("A v1 sharedValue=");
    Serial.println(sharedValue);
  }
}

void printPartAChecklist() {
  Serial.println("Part A checklist:");
  Serial.println("1. Buttons have stable readings.");
  Serial.println("2. State changes are explained.");
  Serial.println("3. Timing uses mi