
#include <IRremote.h> //LIB FOR REMOTE
#include <Servo.h>
#include <LedControl.h>

#define POTENTIOMETER A0
#define TEMP_PIN A1
#define LIGHT_PIN A2
#define IR_RECEIVER_PIN 2
#define SERVO_PIN 9

#define BLINKER_LINKS 7
#define BLINKER_RECHTS 8

#define ADC_VREF_mV 5000.0
#define ADC_RESOLUTION 1024.0

#define LIMIT_TWO 2
#define LIMIT_NINE 9
#define LIMIT_FIVE 5

Servo servo;

int widerstandslevel;
int servolevel;
int LEDPins[] = {3, 5, 6, 7, 8};
bool blinkerLON = false;
bool blinkerRON = false;

int segValues[] = {0, 0, 0, 0};
int cachedValues[] = {0, 0, 0, 0};
int currentSlot = 0;
int currentSegValue = 0;
bool valueChanged = false;
bool segIsOn = false;
bool calibrationActive = false;
LedControl segDisplay = LedControl(11, 13, 10, 1);
long msPassed = 0;
int secondsPassed = 0;

int tmpint = 0;

// put your setup code here, to run once:
void setup() {
  // LEDs
  pinMode(3, OUTPUT); // BREMSLICHT
  pinMode(5, OUTPUT); // FRONTLICHT
  pinMode(6, OUTPUT); // HUPE
  pinMode(BLINKER_LINKS, OUTPUT); // BLINKER LINKS
  pinMode(BLINKER_RECHTS, OUTPUT); // BLINKER RECHTS

  // TEMPSENSOR + IR
  Serial.begin(9600);
  IrReceiver.begin(IR_RECEIVER_PIN, ENABLE_LED_FEEDBACK);

  // MOTOR
  servo.attach(SERVO_PIN);

  // SEGMENT-DISPLAY -> LIGHT INTENSITY
  segDisplay.setIntensity(0, 15);
}

// put your main code here, to run repeatedly:
void loop() {
  if (!calibrationActive) {
    readRemote();
    checkTimer();
  } else {
    calibrateTime();
  }
  handleBlinkers();

  //startServo();
}

// THIS IS BECAUSE IM LAZY AND HAVE SHIT MEMORY
// FORCES USER TO USE CORRECT FORMAT FOR PARSER
void logger(
  const char* msgtype, const char* module, const char* type, 
  const char* subtype, const char* status, const int slot, 
  const float value, const char* mode, const char* modeinfo) {
    // TEMPLATE 
    // logger("msgtype", "module", "type", "suptype", "status", intSlot, floatValue, "mode", "modeinfo");
    Serial.print(msgtype);Serial.print(":");
    Serial.print(module);Serial.print(":");
    Serial.print(type);Serial.print(":");
    Serial.print(subtype);Serial.print(":");
    Serial.print(status);Serial.print(":");
    Serial.print(slot);Serial.print(":");
    Serial.print(value);Serial.print(":");
    Serial.print(mode);Serial.print(":");
    Serial.println(modeinfo);
}

// CHECK IF ONE SECOND HAS PASSED -> UPDATE SEG DISPLAY
void checkTimer() {
  if (millis() - msPassed >= 1000) {
    msPassed = millis();
    secondsPassed++;
    updateSegValues();
    if (segIsOn) {
      updateSeg(segValues);
    }
  }
}

void updateSegValues() {
  
  if (segValues[3] == LIMIT_NINE) {
    if (segValues[2] == LIMIT_FIVE) {
      if (segValues[1] == LIMIT_NINE) {

        // e.g. 23:59 -> 00:00
        if (segValues[0] == LIMIT_TWO) {
          //memset(segValues, 0, sizeof(segValues));
          segValues[0] = 0;
          segValues[1] = 0;
          segValues[2] = 0;
          segValues[3] = 0;
          return;
        }
        // e.g. 19:59
        tmpint = segValues[0];
        tmpint++;
        //Serial.println(tmpint);
        //memset(segValues, 0, sizeof(segValues));
        segValues[1] = 0;
        segValues[2] = 0;
        segValues[3] = 0;
        segValues[0] = tmpint;
        //Serial.println(segValues[0]);
        return;
      }
      // e.g. 11:59
      segValues[3] = 0;
      segValues[2] = 0;
      segValues[1]++;
      return;
    }
    // e.g. 11:19
    segValues[3] = 0;
    segValues[2]++;
    return;
  }
  // e.g. 11:11
  segValues[3]++;
  return;
}

void updateSeg(int currVals[]) {
  // SLOT WITH CLOCK VALUE SEPARATOR (1) -> DP LAMP NEEDED
  for (int i = 0; i < 4; i++) {
    if (i == 1) {
      segDisplay.setDigit(0, i, currVals[i], true);
    } else {
      segDisplay.setDigit(0, i, currVals[i], false);
    }
  }
}

void calibrateTime() {
  if (IrReceiver.decode()) {
    // NO REPEAT READS
    if (!(IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT)) {
      switch (IrReceiver.decodedIRData.command) {
        case 68:
          // GO BACK ONE SLOT; CONFIGURE PREV SLOT
          if (currentSlot > 0) {
            currentSlot --;
            logger("INFO", "CLOCK", "DISPLAY", "", "ON", currentSlot, -1, "CALIBRATION_MODE", "BACK");
            /* Serial.print("INFO:CLOCK:CALIBRATION_MODE:");
            Serial.println(currentSlot); */

          // Current slot already AT MIN
          } else {
            logger("ERROR", "CLOCK", "DISPLAY", "", "ON", currentSlot, -1, "CALIBRATION_MODE", "MIN");
            //Serial.println("ERROR:CLOCK:CALIBRATION_MODE:SLOT:MIN"); 
          }
          break;

        // CONFIRM -> OK BUTTON (CLOCK GOT CALIBRATED)
        case 64:
          calibrationActive = false;
          delay(900);
          logger("INFO", "CLOCK", "DISPLAY", "", "ON", currentSlot, -1, "NORMAL", "CONFIRMED");
          break;

        // GO FORWARD ONE SLOT; CONFIGURE NEXT SLOT
        case 67:
          if (currentSlot < 3) {
            currentSlot ++;
            logger("INFO", "CLOCK", "DISPLAY", "", "ON", currentSlot, -1, "CALIBRATION_MODE", "FORWARD");
            /* Serial.print("INFO:CLOCK:CALIBRATION_MODE:SLOT:");
            Serial.println(currentSlot); */

          // Current slot already END
          } else {
            logger("ERROR", "CLOCK", "DISPLAY", "", "ON", currentSlot, -1, "CALIBRATION_MODE", "MAX");
            //Serial.println("ERROR:CLOCK:CALIBRATION_MODE:SLOT:MAX"); 
          }
          break;

        // SET CURRENT SLOT TO VALUE 1
        case 22:
          currentSegValue = 1;
          valueChanged = true;
          break;

        // SET CURRENT SLOT TO VALUE 2
        case 25:
          currentSegValue = 2;
          valueChanged = true;
          break;

        case 13:
          currentSegValue = 3;
          valueChanged = true;
          break;

        case 12:
          currentSegValue = 4;
          valueChanged = true;
          break;

        case 24:
          currentSegValue = 5;
          valueChanged = true;
          break;

        case 94:
          currentSegValue = 6;
          valueChanged = true;
          break;

        case 8:
          currentSegValue = 7;
          valueChanged = true;
          break;

        case 28:
          currentSegValue = 8;
          valueChanged = true;
          break;

        case 90:
          currentSegValue = 9;
          valueChanged = true;
          break;
        
        case 82:
          currentSegValue = 0;
          valueChanged = true;
          break;

        // CANCEL CLOCK CALIBRATION -> RESET VALUES TO CACHED ARRAY
        case 66:
          logger("ERROR", "CLOCK", "DISPLAY", "", "ON", currentSlot, -1, "CALIBRATION_MODE", "CANCELLED");
          //Serial.println("INFO:CLOCK:CALIBRATION_MODE:CANCELLED");
          //Serial.println("INFO: Clock reset to previous state")
          calibrationActive = false;
          
          for (int i = 0; i < 4; i++) {
            segValues[i] = cachedValues[i];
          }
          updateSeg(cachedValues);
          break;

        // TODO: remove break; in every number key and have additional
        //       case for all number keys to set value flag valueChanged to true

        default:
        logger("WARNING", "REMOTE", "KEY", "UNKOWN", "", -1, -1, "CLOCK_CALIBRATION", "");
          //Serial.println("WARNING:REMOTE:UNKOWN"); // UNKNWON REMOTE KEY
          break;
      }
      //Serial.println(IrReceiver.decodedIRData.command); // spams; dont uncomment
    }
    IrReceiver.resume();
  }

  // UPDATE VALUE OF CURRENT SLOT
  if (valueChanged) {

    // SLOT WITH CLOCK VALUE SEPARATOR (1) -> DP LAMP NEEDED
    if (currentSlot == 1) segDisplay.setDigit(0, currentSlot, currentSegValue, true);
    else segDisplay.setDigit(0, currentSlot, currentSegValue, false);

    // UPDATE ARRAY AND RESET VALUE FLAG
    segValues[currentSlot] = currentSegValue;
    valueChanged = false;
  }
}

// SEGMENT DISPLAY ON; NO MATTER WHAT SegValues
void segON() {
  segDisplay.shutdown(0, false);
  segDisplay.clearDisplay(0);
  updateSeg(segValues);
  segIsOn = true;
}

// SEGMENT DISPLAY OFF
void segOFF() {
  segDisplay.shutdown(0, true);
  segIsOn = false;
}

void readRemote() {
  // FERNBEDIENUNG
  if (IrReceiver.decode()) {
    // NO REPEAT READS
    if (!(IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT)) {
      switch (IrReceiver.decodedIRData.command) {
        case 70:
          // TODO: FRONTLIGHT
          logger("WARNING", "REMOTE", "KEY", "UP", "UNCONFIGURED", -1, -1, "MAIN", "");
          //Serial.println("WARNING:REMOTE:UP:UNCONFIGURED");
          break;

        case 68:
          //Serial.println("LEFT");
          blinkerRON = false;
          blinkerLON = !blinkerLON;

          if (blinkerLON) {
            logger("INFO", "REMOTE", "KEY", "UP", "UNCONFIGURED", -1, -1, "MAIN", "");
            //Serial.println("INFO:BLINKER:LEFT:ON");
          } else {
            logger("INFO", "LED", "BLINKER", "LEFT", "OFF", -1, -1, "", "");
            //Serial.println("INFO:BLINKER:LEFT:OFF");
          }

          break;

        // INITIATE ALL
        case 64:
          if (!blinkerLON && !blinkerRON) {
            logger("INFO", "LED", "ALL", "ALL", "TEST", -1, -1, "BLINKERTEST", "ACTIVE");
            //Serial.println("INFO:LED:TEST:ACTIVE");
            wave();
          }
          break;

        case 67:
          //Serial.println("RIGHT");
          blinkerLON = false;
          blinkerRON = !blinkerRON;

          if (blinkerRON) {
            logger("INFO", "LED", "BLINKER", "RIGHT", "ON", -1, -1, "", "");
            //Serial.println("INFO:BLINKER:RIGHT:ON");
          } else {
            logger("INFO", "LED", "BLINKER", "RIGHT", "OFF", -1, -1, "", "");
            //Serial.println("INFO:BLINKER:RIGHT:OFF");
          }

          break;

        case 21:
          // TODO: BREAKLIGHT
          logger("WARNING", "REMOTE", "KEY", "DOWN", "UNCONFIGURED", -1, -1, "", "");
          //Serial.println("WARNING:REMOTE:DOWN:UNCONFIGURED");
          break;

        case 22:
          //Serial.println("1");
          logger("INFO", "SENSOR", "TEMPERATURE", "", "MEASURING", -1, measureTemp(), "", "");
          /* Serial.print("INFO:SENSOR:TEMPERATURE:");
          Serial.println(measureTemp()); */
          //Serial.println(" °C");
          break;

        case 25:
          //Serial.println("2");
          logger("INFO", "SENSOR", "LIGHTLEVEL", "", "MEASURING", -1, measureLight(), "", "");
          /* Serial.print("INFO:LIGHTLEVEL:");
          Serial.println(measureLight()); */
          break;

        case 13:
          logger("WARNING", "REMOTE", "KEY", "3", "UNCONFIGURED", -1, -1, "", "");
          //Serial.println("WARNING:REMOTE:3:UNCONFIGURED");
          break;

        case 12:
          logger("WARNING", "REMOTE", "KEY", "4", "UNCONFIGURED", -1, -1, "", "");
          //Serial.println("WARNING:REMOTE:4:UNCONFIGURED");
          break;

        case 24:
          logger("WARNING", "REMOTE", "KEY", "5", "UNCONFIGURED", -1, -1, "", "");
          //Serial.println("WARNING:REMOTE:5:UNCONFIGURED");
          break;

        case 94:
          logger("WARNING", "REMOTE", "KEY", "6", "UNCONFIGURED", -1, -1, "", "");
          //Serial.println("WARNING:REMOTE:6:UNCONFIGURED");
          break;

        case 8:
          logger("WARNING", "REMOTE", "KEY", "7", "UNCONFIGURED", -1, -1, "", "");
          //Serial.println("WARNING:REMOTE:7:UNCONFIGURED");
          break;

        case 28:
          logger("WARNING", "REMOTE", "KEY", "8", "UNCONFIGURED", -1, -1, "", "");
          //Serial.println("WARNING:REMOTE:8:UNCONFIGURED");
          break;

        case 90:
          logger("WARNING", "REMOTE", "KEY", "9", "UNCONFIGURED", -1, -1, "", "");
          //Serial.println("WARNING:REMOTE:9:UNCONFIGURED");
          break;

        case 66:
          // CACHE VALUES & CONFIGURE SEG DISPLAY
          if (segIsOn) {
            logger("INFO", "CLOCK", "DISPLAY", "", "ON", 0, -1, "CALIBRATION_MODE", "ACTIVATED");
            //Serial.println("INFO:CLOCK:CALIBRATION_MODE:ACTIVE");
            calibrationActive = true;
            for (int i = 0; i < 4; i++) {
              cachedValues[i] = segValues[i];
            }
          
          // CLOCK INACTIVE -> NO VISUAL CONFIGURATION POSSIBLE
          } else {
            logger("ERROR", "CLOCK", "DISPLAY", "", "OFF", -1, -1, "", "");
            //Serial.println("ERROR:CLOCK:DISPLAY:OFF");
          }
          break;

        case 82:
          if (!segIsOn) {
            logger("INFO", "CLOCK", "DISPLAY", "", "ON", 0, -1, "NORMAL", "");
            //Serial.println("INFO:CLOCK:ON");
            segON();
          } else {
            logger("INFO", "CLOCK", "DISPLAY", "", "OFF", -1, -1, "", "");
            //Serial.println("INFO:CLOCK:OFF");
            segOFF();
          }
          break;

        case 74:
          logger("INFO", "CLOCK", "DISPLAY", "", "ON", 0, -1, "NORMAL", "RESET");
          //Serial.println("INFO:CLOCK:RESET");
          memset(segValues, 0, sizeof(segValues));
          updateSeg(segValues);
          break;

        default:
          //Serial.println("WARNING: undefined key:");
          break;
      }
      //Serial.println(IrReceiver.decodedIRData.command);
    }
    IrReceiver.resume();
  }
}

void handleBlinkers() {
  if (blinkerLON) {
    digitalWrite(BLINKER_RECHTS, LOW);
    blink(BLINKER_LINKS);

  } else if (blinkerRON) {
    digitalWrite(BLINKER_LINKS, LOW);
    blink(BLINKER_RECHTS);
  }
  /* if (!blinkerRON && !blinkerLON) {
    Serial.println("both blinkers off"); // spams; dont uncomment
  } */
}

void blink(int LEDPin) {
  digitalWrite(LEDPin, HIGH);
  delay(500);
  digitalWrite(LEDPin, LOW);
  delay(500);
}

void wave() {
  // WAVE AROUND THE FAKE CAR
  for (int pin : LEDPins) {
    digitalWrite(pin, HIGH);
    delay(100); // uncomment for wave
    digitalWrite(pin, LOW);
  }
}

int readPot() {
  int sum = 0;
  for (int i = 0; i < 5; i++) {
    sum += analogRead(POTENTIOMETER);
    delay(2);
  }
  return sum / 5;
}

void startServo() {
  // MOTOR + POT
  widerstandslevel = readPot();
  servolevel = map(widerstandslevel, 0, 1023, 5, 175);
  servo.write(servolevel);

  logger("INFO", "SERVO", "RESISTORLEVEL", "", "ON", -1, widerstandslevel, "", "");
  /* Serial.print("Resistor level: ");
  Serial.println(widerstandslevel); */

  delay(20);
}

float measureTemp() {
  // TEMPSENSOR
  int adcVal = analogRead(TEMP_PIN);
  float adcInmV= adcVal * (ADC_VREF_mV / ADC_RESOLUTION);
  float tempC = adcInmV / 10;

  return tempC;
}

int measureLight() {
  // LICHTSENSOR
  int lightlevel = analogRead(LIGHT_PIN);

  /* if(lightlevel < 300)
    Serial.println("It's nighttime.");
  else
    Serial.println("It's daytime"); */
  // TODO make frontlights brigter depnding on light level

  return lightlevel;
}
