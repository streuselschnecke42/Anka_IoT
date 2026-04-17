
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

Servo servo;

int widerstandslevel;
int servolevel;
int LEDPins[] = {3, 5, 6, 7, 8};
bool blinkerLON = false;
bool blinkerRON = false;

LedControl segDisplay = LedControl(11, 13, 10, 1);

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

  // SHIFTREGISTER + SEGMENT-DISPLAY
  segDisplay.shutdown(0, false);
  segDisplay.setIntensity(0, 12);
  segDisplay.clearDisplay(0);
}

// put your main code here, to run repeatedly:
void loop() {
  readRemote();
  handleBlinkers();

  //startServo();

  /* digitalWrite(6, HIGH);
  delay(4000);
  digitalWrite(6, LOW);
  delay(1000); */

  segDisplay.setDigit(0, 0, 8, false);
  segDisplay.setDigit(0, 1, 8, true);
  segDisplay.setDigit(0, 2, 8, false);
  segDisplay.setDigit(0, 3, 8, false);
}

void geTime() {
  
}

void readRemote() {
  // FERNBEDIENUNG
  if (IrReceiver.decode()) {
    // NO REPEAT READS
    if (!(IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT)) {
      switch (IrReceiver.decodedIRData.command) {
        case 70:
          Serial.println("UP");
          break;

        case 68:
          Serial.println("LEFT");
          blinkerRON = false;
          blinkerLON = !blinkerLON;
          break;

        // INITIATE ALL
        case 64:
          Serial.println("OK");
          if (!blinkerLON && !blinkerRON) {
            wave();
          }
          break;

        case 67:
          Serial.println("RIGHT");
          blinkerLON = false;
          blinkerRON = !blinkerRON;
          break;

        case 21:
          Serial.println("DOWN");
          break;

        case 22:
          Serial.println("1");
          Serial.print("Temperature: ");
          Serial.print(measureTemp());
          Serial.println(" °C");
          break;

        case 25:
          Serial.println("2");
          Serial.print("Lightlevel: ");
          Serial.println(measureLight());
          break;

        case 13:
          Serial.println("3");
          break;

        case 12:
          Serial.println("4");
          break;

        case 24:
          Serial.println("5");
          break;

        case 94:
          Serial.println("6");
          break;

        case 8:
          Serial.println("7");
          break;

        case 28:
          Serial.println("8");
          break;

        case 90:
          Serial.println("9");
          break;

        case 66:
          Serial.println("*");
          break;

        case 82:
          Serial.println("0");
          break;

        case 74:
          Serial.println("#");
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
    Serial.println("both blinkers off");
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
    delay(100);
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

  Serial.print("Resistor level: ");
  Serial.println(widerstandslevel);

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
