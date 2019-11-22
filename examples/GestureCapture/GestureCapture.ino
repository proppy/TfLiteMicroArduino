/*
  Ported to ESP32/M5Stack IMU from https://github.com/arduino/ArduinoTensorFlowLiteTutorials/

  IMU Capture
  This example uses the on-board IMU to start reading acceleration and gyroscope
  data from on-board IMU and prints it to the Serial Monitor for one second
  when the significant motion is detected.
  You can also use the Serial Plotter to graph the data.

  Created by Don Coleman, Sandeep Mistry
  Modified by Dominic Pajak, Sandeep Mistry

  This example code is in the public domain.
*/

#include <M5StickC.h>
#include <utility/MPU6886.h>

const float accelerationThreshold = 2.5; // threshold of significant in G's
const int numSamples = 119;

int samplesRead = numSamples;

void setup() {
  M5.begin();
    
  Serial.begin(9600);
  while (!Serial);

  M5.IMU.Init();
  M5.Mpu6886.SetAccelFsr(M5.Mpu6886.AFS_4G);
  M5.Mpu6886.SetGyroFsr(M5.Mpu6886.GFS_2000DPS);
  
  // print the header
  Serial.println("aX,aY,aZ,gX,gY,gZ");
}

float buf[6*numSamples];

void loop() {
  float aX, aY, aZ, gX, gY, gZ;

  // wait for significant motion
  while (samplesRead == numSamples) {
      // read the acceleration data
      M5.IMU.getAccelData(&aX,&aY,&aZ);
      // sum up the absolutes
      float aSum = fabs(aX) + fabs(aY) + fabs(aZ);

      // check if it's above the threshold
      if (aSum >= accelerationThreshold) {
        // reset the sample read count
        samplesRead = 0;
        break;
      }
  }

  // check if the all the required samples have been read since
  // the last time the significant motion was detected
  while (samplesRead < numSamples) {
    // check if both new acceleration and gyroscope data is
    // available
      // read the acceleration and gyroscope data
      M5.IMU.getAccelData(&aX,&aY,&aZ);
      M5.IMU.getGyroData(&gX,&gY,&gZ);

      buf[samplesRead*6+0] = aX;
      buf[samplesRead*6+1] = aY;
      buf[samplesRead*6+2] = aZ;
      buf[samplesRead*6+3] = gX;
      buf[samplesRead*6+4] = gY;
      buf[samplesRead*6+5] = gZ;

      delay(10);

      samplesRead++;
      
      if (samplesRead == numSamples) {
        for (int i = 0; i < (6*numSamples); i+=6) {
            // print the data in CSV format
            Serial.print(buf[i], 3);
            Serial.print(',');
            Serial.print(buf[i+1], 3);
            Serial.print(',');
            Serial.print(buf[i+2], 3);
            Serial.print(',');
            Serial.print(buf[i+3], 3);
            Serial.print(',');
            Serial.print(buf[i+4], 3);
            Serial.print(',');
            Serial.print(buf[i+5], 3);
            Serial.println();
        }
        // add an empty line if it's the last sample
        Serial.println();
      }
  }
}
