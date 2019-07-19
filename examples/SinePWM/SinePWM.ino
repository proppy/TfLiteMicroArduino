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

float angle = 0.0f;

void setup() {
  Serial.begin(9600);
  TfLiteMicro.begin(g_sine_model_data);
  pinMode(PB9, PWM);
}

void loop() {
  if (TfLiteMicro.failed()) {
    Serial.print("TfLiteMicro error:");
    Serial.println(TfLiteMicro.error());
  }
  TfLiteMicro.inputFloat(0)[0] = angle;
  TfLiteMicro.invoke();
  float y = TfLiteMicro.outputFloat(0)[0];
  pwmWrite(PB9, 65535 * (y + 1.0f) / 2.0f);
  angle += 0.01f;
  if (angle > 2 * M_PI) {
    angle = 0.0f;
  }
}
