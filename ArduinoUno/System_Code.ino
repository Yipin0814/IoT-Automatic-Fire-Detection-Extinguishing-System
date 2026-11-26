#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_PWMServoDriver.h>
#include <DHT.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

// Website control
SoftwareSerial espSerial(12, 3); 
bool manualMode = false;

// LCD and PCA9685
LiquidCrystal_I2C lcd(0x27, 16, 2);
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// DHT11 Sensor Settings
#define DHTPIN 4       // Connect DHT11 DATA to Digital Pin 4
#define DHTTYPE DHT11  // Define sensor type
DHT dht(DHTPIN, DHTTYPE);

// PCA9685 channels
#define SERVO0 0   // mechanical arm aiming base
#define SERVO1 1   // robotic arm joint 1 (Static Hold)
#define SERVO2 2   // robotic arm joint 2 (Static Hold)
#define SERVO3 3   // flame sensor scanning servo

// Pins
const int MQ2_PIN = A0;
const int FLAME_PIN = 2;
const int FLAME_AO = A1;

const int BUZZER_PIN = 7;
const int RELAY_PIN = 8;

const int RED_PIN = 9;
const int GREEN_PIN = 10;
const int BLUE_PIN = 11;

const int TRIG_PIN = 5;
const int ECHO_PIN = 6;

const int LDR_PIN = A2;
int ldrThreshold = 800;

// Custom Checkmark Symbol
byte checkChar[8] = {
  0b00000,
  0b00001,
  0b00011,
  0b10110,
  0b11100,
  0b01000,
  0b00000,
  0b00000
};

// Thresholds
int smokeThreshold = 400;
const int waterThreshold = 4;
int heatThreshold = 35; // Temperature trigger level

// Relay logic
const int PUMP_ON = HIGH;
const int PUMP_OFF = LOW;

// RGB common cathode
const int LED_ON = HIGH;
const int LED_OFF = LOW;

// Flame sensor logic
const int FLAME_DETECTED_LEVEL = LOW;

// Ultrasonic
long duration;
int distance;

// Servo 3 scanning settings
int leftPos = 150;
int rightPos = 530;
int servo3Pos = leftPos;
int servoDirection = 1;

int servoStep = 5;
unsigned long servoDelay = 35;
unsigned long lastServoMove = 0;

bool stopServo3 = false;

// Timing Variables
unsigned long lastSensorRead = 0;
unsigned long sensorInterval = 500; // Main loop: 0.5 seconds

unsigned long lastDHTRead = 0;
unsigned long dhtInterval = 2000;   // DHT loop: 2.0 seconds
int currentTemp = 0;                // Store the latest temperature

void setup()
{
    Serial.begin(9600);

    pinMode(FLAME_PIN, INPUT);

    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(RELAY_PIN, OUTPUT);

    pinMode(RED_PIN, OUTPUT);
    pinMode(GREEN_PIN, OUTPUT);
    pinMode(BLUE_PIN, OUTPUT);

    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);

    noTone(BUZZER_PIN);
    digitalWrite(RELAY_PIN, PUMP_OFF);

    setRGB(false, true, false);

    Wire.begin();

    // Start DHT sensor
    dht.begin(); 

    // Start the LCD and load the custom character
    lcd.init();
    lcd.backlight();
    lcd.createChar(0, checkChar); 

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Fire System");
    lcd.setCursor(0, 1);
    lcd.print("Starting...");

    pwm.begin();
    pwm.setPWMFreq(50);
    delay(1000);

    // --- SERVO STARTING POSITIONS ---
    pwm.setPWM(SERVO3, 0, servo3Pos); // Radar sweep starts at leftPos
    pwm.setPWM(SERVO0, 0, 340);       // Arm base points forward
    
    // Lock the holding joints into place
    pwm.setPWM(SERVO1, 0, 340);       // Hold joint 1
    pwm.setPWM(SERVO2, 0, 600);       // Hold joint 2

    delay(1000);
    lcd.clear();

    Serial.println("Fire Detection Started");
    espSerial.begin(9600);
}

void loop()
{
    pwm.setPWM(SERVO1, 0, 340);       
    pwm.setPWM(SERVO2, 0, 600);
    updateServo3();

    // DHT11 Timer Loop
    if (millis() - lastDHTRead >= dhtInterval) 
    {
        lastDHTRead = millis();
        float t = dht.readTemperature();
        
        if (!isnan(t)) {
            currentTemp = (int)t; 
        }
    }

    // Main Sensor Timer Loop
    if (millis() - lastSensorRead >= sensorInterval)
    {
        lastSensorRead = millis();

        int smokeValue = analogRead(MQ2_PIN);
        int flameValue = digitalRead(FLAME_PIN);
        int flameAnalog = analogRead(FLAME_AO);

        distance = getDistance();

        bool flameDetected = (flameValue == FLAME_DETECTED_LEVEL);
        bool smokeDetected = (smokeValue > smokeThreshold);
        bool heatDetected = (currentTemp > heatThreshold); 
        bool waterLow = (distance == -1 || distance > waterThreshold);

        String statusText;

        if (flameDetected && smokeDetected)
        {
            statusText = "DANGER";
            stopServo3 = true;
            dangerMode();
        }
        else if (flameDetected)
        {
            statusText = "FLAME"; 
            stopServo3 = true;
            flameMode();
        }
        else if (smokeDetected)
        {
            statusText = "SMOKE";
            stopServo3 = false;
            smokeMode();
        }
        else if (heatDetected) 
        {
            statusText = "HEAT";
            stopServo3 = false;
            heatMode();
        }
        else if (analogRead(LDR_PIN) > ldrThreshold && !flameDetected)
        {
            statusText = "LIGHT";
            stopServo3 = false;
            lightMode();
        }
        else
        {
            statusText = "NORMAL";
            stopServo3 = false;
            normalMode();
        }

        updateLCD(statusText, smokeValue, waterLow, currentTemp);
        printSerial(smokeValue, flameValue, flameAnalog, distance, statusText, waterLow, currentTemp);
    }
    readFromESP();
}

void updateServo3()
{
    if (stopServo3 == true)
    {
        pwm.setPWM(SERVO3, 0, servo3Pos);
        return;
    }

    if (millis() - lastServoMove >= servoDelay)
    {
        lastServoMove = millis();

        servo3Pos += servoDirection * servoStep;

        if (servo3Pos >= rightPos)
        {
            servo3Pos = rightPos;
            servoDirection = -1;
        }
        else if (servo3Pos <= leftPos)
        {
            servo3Pos = leftPos;
            servoDirection = 1;
        }

        pwm.setPWM(SERVO3, 0, servo3Pos);
    }
}

void normalMode()
{
    setRGB(false, true, false);
    if (!manualMode) {
        noTone(BUZZER_PIN);
        digitalWrite(RELAY_PIN, PUMP_OFF);
    }
}

void smokeMode()
{
    setRGB(false, false, true);
    if (!manualMode) {
        tone(BUZZER_PIN, 500);
        digitalWrite(RELAY_PIN, PUMP_OFF);
    }
}

void heatMode()
{
    setRGB(false, false, true);
    if (!manualMode) {
        tone(BUZZER_PIN, 250);
        digitalWrite(RELAY_PIN, PUMP_OFF);
    }
}

void lightMode()
{
    setRGB(true, true, false);
    if (!manualMode) {
        tone(BUZZER_PIN, 300);
        digitalWrite(RELAY_PIN, PUMP_OFF);
    }
}

void flameMode()
{
    setRGB(true, false, false);
    stopAndAimAtFlame();
    if (!manualMode) {
        tone(BUZZER_PIN, 1000);
        digitalWrite(RELAY_PIN, PUMP_ON);
    }
}

void dangerMode()
{
    setRGB(true, false, false);
    stopAndAimAtFlame();
    if (!manualMode) {
        tone(BUZZER_PIN, 1000);
        digitalWrite(RELAY_PIN, PUMP_ON);
    }
}

void stopAndAimAtFlame()
{
    pwm.setPWM(SERVO3, 0, servo3Pos);
    pwm.setPWM(SERVO0, 0, servo3Pos-50);
}

int getDistance()
{
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);

    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    duration = pulseIn(ECHO_PIN, HIGH, 30000);

    if (duration == 0)
    {
        return -1;
    }

    int cm = duration * 0.034 / 2;
    return cm;
}

void updateLCD(String statusText, int smokeValue, bool waterLow, int temp)
{
    lcd.setCursor(0, 0);
    String line0 = "Status: " + statusText;
    while(line0.length() < 16) { 
        line0 += " "; 
    } 
    lcd.print(line0);

    lcd.setCursor(0, 1);
    lcd.print("T:");
    lcd.print(temp);
    lcd.print(" S:");
    lcd.print(smokeValue);
    lcd.print(" W:");
    
    if (waterLow) {
        lcd.print("X");
    } else {
        lcd.write(byte(0)); 
    }
    lcd.print("   "); 
}

void printSerial(int smokeValue, int flameValue, int flameAnalog, int distance, String statusText, bool waterLow, int temp)
{
    Serial.print("Temp:");
    Serial.print(temp);
    Serial.print("C");

    Serial.print(" | Smoke:");
    Serial.print(smokeValue);

    Serial.print(" | Flame DO:");
    Serial.print(flameValue);

    Serial.print(" | Flame AO:");
    Serial.print(flameAnalog);

    Serial.print(" | Distance:");
    Serial.print(distance);
    Serial.print("cm");

    Serial.print(" | Water:");
    Serial.print(waterLow ? "FILL WATER" : "OK");

    Serial.print(" | Servo3:");
    Serial.print(servo3Pos);

    Serial.print(" | Status:");
    Serial.println(statusText);

    int waterPercent = (distance == -1) ? 0 : constrain(map(distance, 20, 0, 0, 100), 0, 100);
    espSerial.print("{\"flame\":");
    espSerial.print(flameValue == FLAME_DETECTED_LEVEL ? 1 : 0);
    espSerial.print(",\"smoke\":");
    espSerial.print(smokeValue);
    espSerial.print(",\"temp\":");
    espSerial.print(temp);
    espSerial.print(",\"humidity\":0");
    espSerial.print(",\"waterLevel\":");
    espSerial.print(waterPercent);
    espSerial.print(",\"pir\":0,\"ldr\":");
    espSerial.print(analogRead(LDR_PIN));
    espSerial.println("}");
}

void setRGB(bool red, bool green, bool blue)
{
    digitalWrite(RED_PIN, red ? LED_ON : LED_OFF);
    digitalWrite(GREEN_PIN, green ? LED_ON : LED_OFF);
    digitalWrite(BLUE_PIN, blue ? LED_ON : LED_OFF);
}

void readFromESP() {
  static String buf = "";
  while (espSerial.available()) {
    char c = espSerial.read();
    if (c == '\n') {
      StaticJsonDocument<64> doc;
      if (!deserializeJson(doc, buf)) {
        if (doc.containsKey("mode")) {
          manualMode = false;
          stopServo3 = false;
        } else {
          manualMode = true;
          if (doc.containsKey("pump"))   digitalWrite(RELAY_PIN, doc["pump"] ? PUMP_ON : PUMP_OFF);
          if (doc.containsKey("buzzer")) { if (doc["buzzer"]) tone(BUZZER_PIN, 1000); else noTone(BUZZER_PIN); }
          if (doc.containsKey("servo")) {
            int pwmVal = map((int)doc["servo"], 0, 180, 150, 530);
            servo3Pos = pwmVal;
            pwm.setPWM(SERVO0, 0, pwmVal);
            stopServo3 = true;
          }
        }
      }
      buf = "";
    } else {
      buf += c;
      if (buf.length() > 200) buf = "";
    }
  }
}