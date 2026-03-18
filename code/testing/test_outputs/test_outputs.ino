
#include <IRremote.h> //LIB FOR REMOTE
#include <Servo.h>

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

// put your setup code here, to run once:
void setup() {
  // LEDs
  pinMode(3, OUTPUT); // BREMSLICHT
  pinMode(5, OUTPUT); // FRONTLICHT
  pinMode(BLINKER_LINKS, OUTPUT); // HUPE
  pinMode(BLINKER_RECHTS, OUTPUT); // BLINKER LINKS
  pinMode(8, OUTPUT); // BLINKER RECHTS

  // TEMPSENSOR + IR
  Serial.begin(9600);
  IrReceiver.begin(IR_RECEIVER_PIN, ENABLE_LED_FEEDBACK);

  // MOTOR
  servo.attach(SERVO_PIN);
}

// put your main code here, to run repeatedly:
void loop() {
  readRemote();
  /* digitalWrite(6, HIGH);
  delay(4000);
  digitalWrite(8, HIGH);
  delay(1000); */
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
          if (blinkerON) {
            blinkerON = false;
          } else {
            blink(BLINKER_LINKS)
            blinkerON = true;
          }
          break;

        // INITIATE ALL
        case 64:
          Serial.println("OK");
          wave();
          break;

        case 67:
          Serial.println("RIGHT");
          if (blinkerRON) {
            blinkerON = false;
          } else if (!blinkerROn && !blinkerLON) {
            blink(BLINKER_RECHTS)
            blinkerRON = true;
          }
          break;

        case 21:
          Serial.println("DOWN");
          break;

        case 22:
          Serial.println("1");
          break;

        case 25:
          Serial.println("2");
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
          Serial.println("WARNING: undefined key:");
          break;
      }
      //Serial.println(IrReceiver.decodedIRData.command);
    }
    IrReceiver.resume();
  }
}

void blink(int LEDPin) {
  digitalWrite(LEDPin, HIGH);
  delay(500);
  digitalWrite(LEDPin, LOW);
  delay(500);
}

void wave() {
  // WAVE AROUND THE FAKE CAR
  for (int i = 0; i < sizeof(LEDPins); i++) {
    digitalWrite(LEDPins[i], HIGH);
    delay(100);
    digitalWrite(LEDPins[i], LOW);
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

  //Serial.print("Resistor level: ");
  //Serial.println(widerstandslevel);

  delay(20);
}

float measureTemp() {
  // TEMPSENSOR
  int adcVal = analogRead(TEMP_PIN);
  float adcInmV= adcVal * (ADC_VREF_mV / ADC_RESOLUTION);
  float tempC = adcInmV / 10;

  //Serial.print("Temperature: ");
  //Serial.print(tempC);
  //Serial.println(" °C");

  return tempC;
}

int measureLight() {
  // LICHTSENSOR
  int lightlevel = analogRead(LIGHT_PIN);

  //Serial.print("Lightlevel: ");
  //Serial.println(lightlevel);

  /* if(lightlevel < 300)
    Serial.println("It's nighttime.");
  else
    Serial.println("It's daytime"); */
  // TODO make frontlights brigter depnding on light level

  return lightlevel;
}
