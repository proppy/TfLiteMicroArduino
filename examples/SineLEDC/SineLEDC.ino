/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include "TfLiteMicroArduino.h"
#include "sine_model_data.h"

#define LEDC_CHANNEL_0     0
#define LEDC_TIMER_13_BIT  13
#define LEDC_BASE_FREQ     5000

float angle = 0.0f;

void setup() {
  Serial.begin(9600);
  TfLiteMicro.begin(sine_model_data_tflite);
  ledcSetup(LEDC_CHANNEL_0, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);
  ledcAttachPin(G10, LEDC_CHANNEL_0);
}

void loop() {
  if (TfLiteMicro.failed()) {
    Serial.print("TfLiteMicro error:");
    Serial.println(TfLiteMicro.error());
    return;
  }
  TfLiteMicro.inputFloat(0)[0] = angle;
  TfLiteMicro.invoke();
  float y = TfLiteMicro.outputFloat(0)[0];
  ledcWrite(LEDC_CHANNEL_0, 8191 * (y + 1.0f) / 2.0f);
  angle += 0.1f;
  if (angle > 2 * M_PI) {
    angle = 0.0f;
  }
  delay(30);
}
