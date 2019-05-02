#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;
int countsPerG;
float currentYForce, currentXForce, currentZForce;
float accelArray[3];
char userInput;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  //Serial.println("Init. MPU Sensor");
  while(!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_16G)){
    //Serial.println("Could not find a valid MPU sensor!");
    delay(500);
  }
  MpuSettings();
  //Serial.println("FOUND MPU SENSOR!");
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


void loop() {
  
  Vector rawAccel = mpu.readRawAccel();
  Vector normAccel = mpu.readNormalizeAccel();
  
  
  if(Serial.available() > 0){
    userInput = Serial.read();
    if(userInput == 'g'){
      accelArray[0] = rawAccel.YAxis/countsPerG;
      accelArray[1] = rawAccel.XAxis/countsPerG;
      accelArray[2] = rawAccel.ZAxis/countsPerG;
      for(int i = 0; i < 3; i++){
        digitalWrite(13, HIGH);
        Serial.println(accelArray[i]);
        digitalWrite(13, LOW);
      }
    }
  }
}
