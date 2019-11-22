/*
  Ported to TfLiteMicroArduino ESP32/M5Stack IMU
  from https://github.com/arduino/ArduinoTensorFlowLiteTutorials/

  IMU Classifier
  This example uses the on-board IMU to start reading acceleration and gyroscope
  data from on-board IMU, once enough samples are read, it then uses a
  TensorFlow Lite (Micro) model to try to classify the movement as a known gesture.
  Note: The direct use of C/C++ pointers, namespaces, and dynamic memory is generally
        discouraged in Arduino examples, and in the future the TensorFlowLite library
        might change to make the sketch simpler.

  Created by Don Coleman, Sandeep Mistry
  Modified by Dominic Pajak, Sandeep Mistry

  This example code is in the public domain.
*/

#include "TfLiteMicroArduino.h"

#include <M5StickC.h>
#include <utility/MPU6886.h>

#include "model.h"

const float accelerationThreshold = 2.5; // threshold of significant in G's
const int numSamples = 119;

int samplesRead = numSamples;

// array to map gesture index to a name
const char* GESTURES[] = {
  "maru",
  "batsu"
};

#define NUM_GESTURES (sizeof(GESTURES) / sizeof(GESTURES[0]))


void setup() {
  Serial.begin(115200);
  
  M5.begin();

  // initialize the IMU
  M5.IMU.Init();
  M5.Mpu6886.SetAccelFsr(M5.Mpu6886.AFS_4G); // AFS_4G
  M5.Mpu6886.SetGyroFsr(M5.Mpu6886.GFS_2000DPS); // GFS_2000DPS

  TfLiteMicro.begin(model, 8*1024);
}

void loop() {
  if (TfLiteMicro.failed()) {
    Serial.print("TfLiteMicro error:");
    Serial.println(TfLiteMicro.error());
    return;
  }

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
      // read the acceleration and gyroscope data
      M5.IMU.getAccelData(&aX,&aY,&aZ);
      M5.IMU.getGyroData(&gX,&gY,&gZ);
      
      // normalize the IMU data between 0 to 1 and store in the model's
      // input tensor
      
      TfLiteMicro.inputFloat(0)[samplesRead * 6 + 0] = (aX + 4.0) / 8.0;
      TfLiteMicro.inputFloat(0)[samplesRead * 6 + 1] = (aY + 4.0) / 8.0;
      TfLiteMicro.inputFloat(0)[samplesRead * 6 + 2] = (aZ + 4.0) / 8.0;
      TfLiteMicro.inputFloat(0)[samplesRead * 6 + 3] = (gX + 2000.0) / 4000.0;
      TfLiteMicro.inputFloat(0)[samplesRead * 6 + 4] = (gY + 2000.0) / 4000.0;
      TfLiteMicro.inputFloat(0)[samplesRead * 6 + 5] = (gZ + 2000.0) / 4000.0;

      delay(10);

      samplesRead++;

      if (samplesRead == numSamples) {
        // Run inferencing
        TfLiteMicro.invoke();

        if (TfLiteMicro.outputFloat(0)[0] > 0.8) {
          Serial.println("🙆(まる)");
        }

        if (TfLiteMicro.outputFloat(0)[1] > 0.8) {
          Serial.println("🙅(ばつ)");
        }
      }
  }
}
