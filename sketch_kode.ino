#include <ESP8266WiFi.h>
#include <Servo.h> // Tambahkan pustaka servo

const int irSensor1 = D1;
const int irSensor2 = D2;
const int irSensor3 = D5;
const int metalProximitySensor1 = D8;
const int metalProximitySensor2 = D4; // Sensor metal tambahan
const int motorIn1 = D6;
const int motorIn2 = D7;

Servo myServo; // Objek servo
const int servoPin = D0;

enum MotorState { STOPPED, MOVING_RIGHT, MOVING_LEFT };
MotorState motorState = STOPPED;

unsigned long ir1DetectTime = 0;
bool ir1Detected = false;
bool servoActive = false;

void setup() {
  pinMode(irSensor1, INPUT);
  pinMode(irSensor2, INPUT);
  pinMode(irSensor3, INPUT);
  pinMode(metalProximitySensor1, INPUT);
  pinMode(metalProximitySensor2, INPUT);
  pinMode(motorIn1, OUTPUT);
  pinMode(motorIn2, OUTPUT);

  digitalWrite(motorIn1, LOW);
  digitalWrite(motorIn2, LOW);

  myServo.attach(servoPin); // Menghubungkan servo ke pin
  myServo.write(180); // Posisi awal servo di 0°

  Serial.begin(115200);
  Serial.println("Program dimulai...");
}

void loop() {
  int sensor1State = digitalRead(irSensor1);
  int sensor2State = digitalRead(irSensor2);
  int sensor3State = digitalRead(irSensor3);
  int proximityState1 = digitalRead(metalProximitySensor1);
  int proximityState2 = digitalRead(metalProximitySensor2);

  // Logika motor berdasarkan sensor IR dan proximity
  if (sensor3State == LOW) {
    motorState = STOPPED;
    stopMotor();
  } else if (sensor2State == LOW) {
    motorState = STOPPED;
    stopMotor();
  } else if (proximityState1 == LOW || proximityState2 == LOW) {
    if (motorState != MOVING_LEFT || motorState != STOPPED) {
      motorState = MOVING_LEFT;
      moveMotorLeft();
    }
  } else if (sensor1State == LOW) {
    if (motorState != MOVING_RIGHT || motorState != STOPPED) {
      motorState = MOVING_RIGHT;
      moveMotorRight();
    }
  }

  // Logika penundaan 2 detik sebelum servo membuka
  if (sensor1State == LOW && !ir1Detected) {
    ir1DetectTime = millis(); // Catat waktu deteksi pertama
    ir1Detected = true;
    servoActive = false;
  }

  if (ir1Detected && millis() - ir1DetectTime >= 2000 && !servoActive) {
    // Setelah 2 detik, buka servo
    myServo.write(0);
    Serial.println("Servo membuka ke 0");
    servoActive = true; // Tandai servo telah aktif
  }

  // Logika untuk mengembalikan servo ke posisi awal
  if (servoActive && millis() - ir1DetectTime >= 4000) {
    myServo.write(180); // Kembali ke 0° setelah 2 detik membuka
    Serial.println("Servo kembali ke 180°");
    ir1Detected = false; // Reset deteksi IR 1
    servoActive = false; // Reset status servo
  }
}

void moveMotorRight() {
  digitalWrite(motorIn1, HIGH);
  digitalWrite(motorIn2, LOW);
}

void moveMotorLeft() {
  digitalWrite(motorIn1, LOW);
  digitalWrite(motorIn2, HIGH);
}

void stopMotor() {
  digitalWrite(motorIn1, LOW);
  digitalWrite(motorIn2, LOW);
}
