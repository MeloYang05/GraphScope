/**
 * Copyright 2021 Alibaba Group Holding Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "jna/native.h"

namespace DB_NAMESPACE {

class TestResult {
public:
  TestResult(bool successful, const std::stringstream& logger)
    : successful_(successful), info_(std::move(logger.str())) {}
  ~TestResult() = default;

  bool GetResult() const { return successful_; }
  const char* GetInfo() const { return info_.c_str(); }

private:
  bool successful_;
  std::string info_;
};

extern "C" DLL_EXPORT TestResult* runLocalTests();
extern "C" DLL_EXPORT bool getTestResultFlag(const TestResult* r);
extern "C" DLL_EXPORT const char* getTestResultInfo(const TestResult* r);
extern "C" DLL_EXPORT void freeTestResult(TestResult* r);

}  // namespace DB_NAMESPACE
