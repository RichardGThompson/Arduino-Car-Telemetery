#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>

#include <Wire.h>
#include<MPU6050.h>



char userInput;

//Calibration Stuff.
#define CALIBRATION_BUTTON 2l
#define CALIBRATION_SAMPLES 200

//Stuff for RPM.
#define UPDATE_INTERVAL 50
unsigned long lastUpdateTime = 0;
#define LED_COUNT 9
#define TACH_SIGNAL 3
#define CYLINDERS 4
#define MAX_RPM 7500
#define MIN_RPM 4000
const int SPARK_PER_REV = CYLINDERS / 2;
const int shiftLight[] = {23, 24, 25, 26, 27, 28, 29, 30, 31};
volatile int sparkCount = 0;
float currentRpm = 0;
int prevRpm = 0;

//Stuff for Speed.
#define SPEED_SIGNAL 18
const float rpmPerKmh = 10.62f;
volatile int speedCount = 0;
float currentSpeed = 0;
int prevSpeed = 0;

//Stuff for Throttle.
#define THROTTLE_PIN A0
int throttleVal;

//Stuff for G-Force.
MPU6050 mpu;
int countsPerG;
float currentYForce, currentXForce, currentZForce;
float accelArray[3];
float yOffset, xOffset, zOffset;

//Stuff for Radio.
float dataArray[6];

void InitLeds(){
  for(int i = 0; i < LED_COUNT; i++){
    pinMode(shiftLight[i], OUTPUT);
    digitalWrite(shiftLight[i], HIGH);
    delay(25);
    digitalWrite(shiftLight[i], LOW);
  }
}

void SensorCalibration(){
  Serial.println("Calibrating Sensors...");
  Vector rawAccel = mpu.readRawAccel();
  float mpuCalibrationArray[CALIBRATION_SAMPLES][3];
  for(int i = 0; i < CALIBRATION_SAMPLES; i++){
    mpuCalibrationArray[i][0] = rawAccel.YAxis/countsPerG;
    mpuCalibrationArray[i][1] = rawAccel.XAxis/countsPerG;
    mpuCalibrationArray[i][2] = rawAccel.ZAxis/countsPerG;
  }
  for(int i = 0; i < CALIBRATION_SAMPLES; i++){
    yOffset += mpuCalibrationArray[i][0];
    xOffset += mpuCalibrationArray[i][1];
    zOffset += mpuCalibrationArray[i][2];
  }
  yOffset = yOffset / CALIBRATION_SAMPLES;
  xOffset = xOffset / CALIBRATION_SAMPLES;
  zOffset = zOffset / CALIBRATION_SAMPLES;
  Serial.println(yOffset);
  Serial.println(xOffset);
  Serial.println(zOffset);
}



void StartUpSeq(){
  InitLeds();
  //delay(3000);
  SensorCalibration();
  
}

void MpuSettings(){
  //Setting the Counts Per G based on the G_Scale Selected.
  switch(mpu.getRange()){
    case MPU6050_RANGE_16G:
      countsPerG = 2048;
      break;
    case MPU6050_RANGE_8G:
      countsPerG = 4096;
      break;
    case MPU6050_RANGE_4G:
      countsPerG = 8192;
      break;
    case MPU6050_RANGE_2G:
      countsPerG = 16384;
      break;
  }
}

float getThrottlePos(){
  //897 CLOSED
  //228 OPEN
  return map(analogRead(THROTTLE_PIN), 228, 897, 100, 0);
  //Serial.println(throttleVal);
}

void incrementSpeedCount(){
  speedCount++;
}

void incrementSparkCount(){
  sparkCount++;
}

void setup() {
  // put your setup code here, to run once:
  pinMode(CALIBRATION_BUTTON, INPUT_PULLUP);
  Serial.begin(115200);
  while(!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_16G)){
    //Serial.println("Could not find a valid MPU sensor!");
    delay(500);
  }
  pinMode(SPEED_SIGNAL, INPUT_PULLUP);
  pinMode(TACH_SIGNAL, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(SPEED_SIGNAL), incrementSpeedCount, FALLING);
  attachInterrupt(digitalPinToInterrupt(TACH_SIGNAL), incrementSparkCount, FALLING);
  MpuSettings();
  StartUpSeq();
  
  
}

void loop() {
  
  // put your main code here, to run repeatedly:
  Vector rawAccel = mpu.readRawAccel();
  
  

  
  float currentInterval = millis() - lastUpdateTime;
  if(currentInterval > UPDATE_INTERVAL){
    currentSpeed = (speedCount * (1000.0f / currentInterval)) / rpmPerKmh;
    dataArray[3] = currentSpeed;
    currentRpm = (sparkCount * (1000.0f / currentInterval) * 60) / SPARK_PER_REV;
    dataArray[4] = currentRpm;
    speedCount = 0;
    sparkCount = 0;
    //Serial.println(currentSpeed);
    //Serial.println(currentRpm);
    lastUpdateTime = millis();
  }
  
  dataArray[0] = (rawAccel.YAxis/countsPerG) - yOffset;
  dataArray[1] = (rawAccel.XAxis/countsPerG) - xOffset;
  dataArray[2] = (rawAccel.ZAxis/countsPerG) - zOffset;
  dataArray[5] = getThrottlePos();

  for(int i = 0; i < 6; i++){
    Serial.println(dataArray[i]);
  }
  delay(100);
}
