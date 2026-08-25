#pragma once
#include <Arduino.h>

// Wspólny format wyjścia testów bring-up, parsowany przez tools/run_bringup.py
inline void selfTestSuiteBegin(const char *suiteName) {
  Serial.print(F("===TEST_SUITE_START:"));
  Serial.print(suiteName);
  Serial.println(F("==="));
}

inline void selfTestReport(bool passed, const char *name) {
  Serial.print(passed ? F("[PASS] ") : F("[FAIL] "));
  Serial.println(name);
}

inline void selfTestSuiteEnd(bool allPassed) {
  Serial.print(F("===TEST_SUITE_END:RESULT="));
  Serial.print(allPassed ? F("PASS") : F("FAIL"));
  Serial.println(F("==="));
}
