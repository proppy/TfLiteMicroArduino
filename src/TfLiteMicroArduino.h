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

#ifndef TFLITE_ARDUINO_H
#define TFLITE_ARDUINO_H

#include <TensorFlowLite.h>

#include "tensorflow/lite/experimental/micro/kernels/all_ops_resolver.h"
#include "tensorflow/lite/experimental/micro/micro_error_reporter.h"
#include "tensorflow/lite/experimental/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"

class TfLiteMicroArduino {
 private:
  tflite::ops::micro::AllOpsResolver resolver_;
  tflite::ErrorReporter* error_reporter_;
  uint8_t* tensor_arena_;
  tflite::SimpleTensorAllocator* tensor_allocator_;
  tflite::MicroInterpreter* interpreter_;
  TfLiteStatus status_ = kTfLiteError;
  String error_ = "NO_INTERPRETER";
 public:
  void begin(const uint8_t* model_data, int tensor_arena_size = 2 * 1024) {
    error_reporter_ = new tflite::MicroErrorReporter();
    const tflite::Model* model = ::tflite::GetModel(model_data);
    if (model->version() != TFLITE_SCHEMA_VERSION) {
      error_reporter_->Report(
          "Model provided is schema version %d not equal "
          "to supported version %d.\n",
          model->version(), TFLITE_SCHEMA_VERSION);
      error_ = "BAD_MODEL_VERSION";
      status_ = kTfLiteError;
      return;
    }
    tensor_arena_ = new uint8_t[tensor_arena_size];
    tensor_allocator_ = new tflite::SimpleTensorAllocator(tensor_arena_, tensor_arena_size);
    interpreter_ = new tflite::MicroInterpreter(model, resolver_, tensor_allocator_, error_reporter_);
    status_ = kTfLiteOk;
  }
  bool failed() {
    return status_ != kTfLiteOk;
  }
  const String& error() {
    return error_;
  }
  float* inputFloat(int n) {
    return interpreter_->input(n)->data.f;
  }
  uint8_t* inputUInt8(int n) {
    return interpreter_->input(n)->data.uint8;
  }
  float* outputFloat(int n) {
    return interpreter_->output(n)->data.f;
  }
  uint8_t* outputUInt8(int n) {
    return interpreter_->output(n)->data.uint8;
  }
  void invoke() {
    status_ = interpreter_->Invoke();
    if (status_ != kTfLiteOk) {
      error_reporter_->Report("Inference failed: %d\n",  status_);
      error_ = "INVOKE_FAILED";
    }
  }
  void end() {
    if (interpreter_) {
      delete(interpreter_);
      interpreter_ = nullptr;
    }
    if (tensor_allocator_) {
      delete(tensor_allocator_);
      tensor_allocator_ = nullptr;
    }
    if (tensor_arena_) {
      delete(tensor_arena_);
      tensor_arena_ = nullptr;
    }
    if (error_reporter_) {
      delete(error_reporter_);
      error_reporter_ = nullptr;
    }
  }
};
TfLiteMicroArduino TfLiteMicro;

#endif // TFLITE_ARDUINO_H
