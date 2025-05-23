#pragma once
#include <cmath>
#include <cstdlib>

#include "adc_reading.hpp"
class thermistor {
  uint8_t _res;       // adc resolution
  uint8_t _pin;       // pin
  uint16_t _beta;     // just a coeff, defined by a manufacturer
  uint8_t _tempBase;  // base thermistor temperature, defined by a manufacturer
  float _baseDivRes;  // base resistance, defined by manufacturer
  uint8_t _thermistor_bit;

 public:
  thermistor() = default;

  float NTC_compute(float analog, float baseDiv, uint16_t B, uint8_t t,
                    uint8_t res);
  float NTC_compute(float analog, uint32_t R, uint16_t B, uint8_t t,
                    uint32_t Rt, uint8_t res);
  void setup_thermistor(uint8_t pin, uint32_t R, uint16_t B,
                        uint8_t t = 25, uint32_t Rt = 10000, uint8_t res = 10,
                        uint8_t thermistor_bit = 3);
  void config(uint32_t R, uint16_t B, uint8_t t = 25, uint32_t Rt = 10000,
              uint8_t thermistor_bit = 3);
  void setPin(uint8_t pin, uint8_t res = 10, uint8_t bit = 3);
  float getTemp();
  float getTempAverage(uint8_t samples = 20);
  float computeTemp(float analog, uint8_t res = 10);
  uint8_t
  getBit()
  {
    return _thermistor_bit;
  }
};
extern thermistor regulator;
extern thermistor motor;
void temperature_task(void* arg);