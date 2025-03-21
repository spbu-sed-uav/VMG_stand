// #pragma once
// #include "adc_reading.h"
// #include <cstdlib>
// #include <cmath>
// class thermistor
// {
//     uint8_t _res;
//     uint8_t _pin;
//     uint16_t _beta;
//     uint8_t _tempBase;
//     float _baseDivRes;

// public:
//     thermistor() = default;

//     float NTC_compute(float analog, float baseDiv, uint16_t B, uint8_t t,
//     uint8_t res); float NTC_compute(float analog, uint32_t R, uint16_t B,
//     uint8_t t, uint32_t Rt, uint8_t res); void setup_thermistor(uint8_t pin,
//     uint32_t R, uint16_t B, uint8_t t = 25, uint32_t Rt = 10000, uint8_t res
//     = 10); void config(uint32_t R, uint16_t B, uint8_t t = 25, uint32_t Rt =
//     10000); void setPin(uint8_t pin, uint8_t res = 10); float getTemp();
//     float getTempAverage(uint8_t samples = 20);
//     float computeTemp(float analog, uint8_t res = 10);
// };