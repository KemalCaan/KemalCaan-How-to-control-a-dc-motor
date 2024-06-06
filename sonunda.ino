#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define ENCA 2 // YELLOW
#define ENCB 3 // WHITE
#define PWM 5
#define IN2 6
#define IN1 7
#define STBY 8 // Standby pin

volatile int posi = 0; 
long prevT = 0;
float eprev = 0;
float eintegral = 0;

LiquidCrystal_I2C lcd(0x27, 16, 2);  

void setup() {
  Serial.begin(9600);
  pinMode(ENCA, INPUT);
  pinMode(ENCB, INPUT);
  attachInterrupt(digitalPinToInterrupt(ENCA), readEncoder, RISING);
  
  pinMode(PWM, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(STBY, OUTPUT);
  
  digitalWrite(STBY, HIGH); // Standby pinini aktif hale getirme
  
  lcd.init(); // LCD'yi başlat
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Starting...");
  
  Serial.println("target pos");
}

void loop() {
  int target = 800; 

  // PID constants
  float kp = 0.65;
  float kd = 0.025;
  float ki = 0.0;

  // time difference
  long currT = micros();
  float deltaT = ((float)(currT - prevT)) / (1.0e6);
  prevT = currT;

  // Read the position
  int pos = 0;
  noInterrupts(); // disable interrupts temporarily while reading
  pos = posi;
  interrupts(); // turn interrupts back on

  // error
  int e = target - pos;

  // derivative
  float dedt = (e - eprev) / (deltaT);

  // integral
  eintegral = eintegral + e * deltaT;

  // control signal
  float u = kp * e + kd * dedt + ki * eintegral;

  // motor power
  float pwr = fabs(u);
  if (pwr > 255) {
    pwr = 255;
  }

  // motor direction
  int dir = 1;
  if (u < 0) {
    dir = -1;
  }

  // signal the motor
  setMotor(dir, pwr, PWM, IN1, IN2);

  // store previous error
  eprev = e;

  // Convert target to degrees
  float targetDegrees;
  if (target < 850) {
    targetDegrees = target * 0.45;
  } else {
    targetDegrees = target * 0.4;
  }

  // Convert position to degrees
  float posDegrees;
  if (pos < 850) {
    posDegrees = pos * 0.45;
  } else {
    posDegrees = pos * 0.4;
  }

  // Print the output
  Serial.print("Target: ");
  Serial.print(target);
  Serial.print(" (");
  Serial.print(targetDegrees);
  Serial.print(" degrees) | Position: ");
  Serial.print(pos);
  Serial.print(" (");
  Serial.print(posDegrees);
  Serial.print(" degrees) | Error: ");
  Serial.print(e);
  Serial.print(" | PWM: ");
  Serial.println(pwr);
  
  // Display on LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T: ");
  lcd.print(targetDegrees);
  lcd.print(" deg");
  lcd.setCursor(0, 1);
  lcd.print("P: ");
  lcd.print(posDegrees);
  lcd.print(" E: ");
  lcd.print(e);
}

void setMotor(int dir, int pwmVal, int pwm, int in1, int in2) {
  analogWrite(pwm, pwmVal);
  if (dir == 1) {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
  } else if (dir == -1) {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
  } else {
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
  }
}

void readEncoder() {
  int b = digitalRead(ENCB);
  if (b > 0) {
    posi++;
  } else {
    posi--;
  }
}
