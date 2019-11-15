//// STATES ////
#define sDontWalk 0
#define sWalk 2
#define sDontWalkBlink 3
#define sWalkBlink 4
#define sDontWalkSolid 5
#define sWalkSolid 6
#define sWalkAndDontSolid 7
#define sRandom 8
#define sOff 9

int State = sDontWalk;

//// PIN NAMES ////
const int DontWalkLight = 0;
const int WalkLight = 1;
const int ModeButton = 3;
const int PowerSW = 4;

//// VARS ////

//// Steady on Vars ////
const long DontWalkLength = 10000;
const long WalkLength = 10000;
long DontWalkTime = DontWalkLength;
long WalkTime = WalkLength;
unsigned long CurrentMillis;
unsigned long PreviousMillis;

//// Flash Vars ////
long PrevLightMillis = 0;
long LightInt1 = 1000;
long LightInt2 = 100;
int NumLight = 2; // Less 1
int LightNum = 0;
bool LightChanged = true;
const long FlashInterval = 500;
int DontWalkLightState = LOW;
int WalkLightState = LOW;

//// Random Flash Vars ////
int NumberOfLights = 2;
long NextFlash[2];
int LightPINs[] = {DontWalkLight, WalkLight};
int LightState[2];

//// Button Debounce Vars ////
int ButtonState;
int LastButtonState = HIGH;
long LastDebounceTime = 0;
long DebounceDelay = 40;
bool ButtonToggle = false;

void setup() {
  //// PIN MODE ////
  pinMode(DontWalkLight, OUTPUT);
  pinMode(WalkLight, OUTPUT);
  pinMode(ModeButton, INPUT);
  pinMode(PowerSW, INPUT);

  // Turn the lights off on boot
  digitalWrite(DontWalkLight, LOW);
  digitalWrite(WalkLight, LOW);

  // Enable internal resistor
  digitalWrite(ModeButton, HIGH);
  digitalWrite(PowerSW, HIGH);

  CurrentMillis = millis();
}

void loop() {
  switch (State) {
    case sDontWalk:
      fDontWalk();
      break;

    case sWalk:
      fWalk();
      break;

    case sDontWalkBlink:
      fDontWalkBlink();
      break;

    case sWalkBlink:
      fWalkBlink();
      break;

    case sDontWalkSolid:
      fDontWalkSolid();
      break;

    case sWalkSolid:
      fWalkSolid();
      break;

    case sWalkAndDontSolid:
      fWalkAndDontSolid();
      break;

    case sRandom:
      fRandom();
      break;

    case sOff:
      fOff();
      break;
  }
}

//// FUNCTIONS ////

void fDontWalk() {
  digitalWrite(WalkLight, LOW);
  digitalWrite (DontWalkLight, HIGH);

  if (millis() - CurrentMillis > 1000) {
    DontWalkTime = DontWalkTime - 1000;
    CurrentMillis = millis();
  }

  if (DontWalkTime == 0) {
    DontWalkTime = DontWalkLength;
    State = sWalk;
  }

  fButtonPress();

  if (ButtonToggle == true) {
    DontWalkTime = DontWalkLength;
    ButtonToggle = false;
    State = sDontWalkBlink;
  }

  if (digitalRead (PowerSW) == LOW) {
    State = sOff;
  }
}

void fWalk() {
  digitalWrite(DontWalkLight, LOW);
  digitalWrite(WalkLight, HIGH);

  if (millis() - CurrentMillis > 1000) {
    WalkTime = WalkTime - 1000;
    CurrentMillis = millis();
  }

  if (WalkTime == 0) {
    WalkTime = WalkLength;
    State = sDontWalk;
  }

  fButtonPress();

  if (ButtonToggle == true) {
    WalkTime = WalkLength;
    ButtonToggle = false;
    State = sDontWalkBlink;
  }

  if (digitalRead (PowerSW) == LOW) {
    State = sOff;
  }
}

void fDontWalkBlink() {
  digitalWrite(WalkLight, LOW);

  CurrentMillis = millis();

  if (CurrentMillis - PreviousMillis >= FlashInterval) {
    PreviousMillis = CurrentMillis;

    if (DontWalkLightState == LOW) {
      DontWalkLightState = HIGH;
    } else {
      DontWalkLightState = LOW;
    }
    digitalWrite(DontWalkLight, DontWalkLightState);
  }

  fButtonPress();

  if (ButtonToggle == true) {
    ButtonToggle = false;
    State = sWalkBlink;
  }

  if (digitalRead (PowerSW) == LOW) {
    State = sOff;
  }
}

void fWalkBlink() {
  digitalWrite (DontWalkLight, LOW);

  CurrentMillis = millis();

  if (CurrentMillis - PreviousMillis >= FlashInterval) {
    PreviousMillis = CurrentMillis;

    if (WalkLightState == LOW) {
      WalkLightState = HIGH;
    } else {
      WalkLightState = LOW;
    }
    digitalWrite(WalkLight, WalkLightState);
  }

  fButtonPress();

  if (ButtonToggle == true) {
    ButtonToggle = false;
    State = sDontWalkSolid;
  }

  if (digitalRead (PowerSW) == LOW) {
    State = sOff;
  }
}

void fDontWalkSolid() {
  digitalWrite (WalkLight, LOW);
  digitalWrite (DontWalkLight, HIGH);

  fButtonPress();

  if (ButtonToggle == true) {
    ButtonToggle = false;
    State = sWalkSolid;
  }

  if (digitalRead (PowerSW) == LOW) {
    State = sOff;
  }
}

void fWalkSolid() {
  digitalWrite (WalkLight, HIGH);
  digitalWrite (DontWalkLight, LOW);

  fButtonPress();

  if (ButtonToggle == true) {
    ButtonToggle = false;
    State = sWalkAndDontSolid;
  }

  if (digitalRead (PowerSW) == LOW) {
    State = sOff;
  }
}

void fWalkAndDontSolid() {
  digitalWrite (WalkLight, HIGH);
  digitalWrite (DontWalkLight, HIGH);

  fButtonPress();

  if (ButtonToggle == true) {
    ButtonToggle = false;
    State = sRandom;
  }

  if (digitalRead (PowerSW) == LOW) {
    State = sOff;
  }
}

void fRandom() {
  for (int i = 0; i < NumberOfLights; i++) {
    if (millis() > NextFlash[i]) {
      if (LightState[i] == LOW) LightState[i] = HIGH; else LightState[i] = LOW;
      digitalWrite(LightPINs[i], LightState[i]);
      NextFlash[i] = millis() + random(50, 1500) ; // next toggle random time
    }
  }

  fButtonPress();

  if (ButtonToggle == true) {
    ButtonToggle = false;
    State = sDontWalk;
  }

  if (digitalRead (PowerSW) == LOW) {
    State = sOff;
  }
}

void fOff() {
  digitalWrite (DontWalkLight, LOW);
  digitalWrite(WalkLight, LOW);

  if (digitalRead (PowerSW) == HIGH) {
    DontWalkTime = DontWalkLength;
    WalkTime = WalkLength;
    State = sDontWalk;
  }
}

void fButtonPress() {
  int Reading = digitalRead(ModeButton) == LOW;

  if (Reading != LastButtonState) {
    LastDebounceTime = millis();
  }

  if ((millis() - LastDebounceTime) > DebounceDelay) {
    if (Reading != ButtonState) {
      ButtonState = Reading;
      if (ButtonState == LOW) { // Logic is inverted since we're using the internal resistors. When button is pressed the PIN goes LOW, when not pressed it's HIGH.
        ButtonToggle = true;
      }
    }
  }
  LastButtonState = Reading;
}
