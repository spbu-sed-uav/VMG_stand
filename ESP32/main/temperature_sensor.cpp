#include "temperature_sensor.h"
// сигнал АЦП, (R резистора / R термистора), B термистора, t термистора,
// разрешение АЦП
thermistor motor;
thermistor regulator;

float
thermistor::NTC_compute(float analog, float baseDiv, uint16_t B, uint8_t t,
                        uint8_t res)
{
  if (analog <= 0) {  // add smth about overflow
    return INFINITY;
  }
  analog = baseDiv / ((float)((1 << res) - 1) / analog - 1.0f);
  analog = (std::log(analog) / B) + 1.0f / (t + 273.15f);
  return (1.0f / analog - 273.15f);
}

// сигнал АЦП, R резистора, B термистора, t термистора, R термистора,разрешение
// АЦП
float
thermistor::NTC_compute(float analog, uint32_t R, uint16_t B, uint8_t t,
                        uint32_t Rt, uint8_t res)
{
  return NTC_compute(analog, (float)R / Rt, B, t, res);
}

// пин, R резистора, B термистора, t термистора, R термистора, разрешение АЦП
void
thermistor::setup_thermistor(uint8_t pin, uint32_t R, uint16_t B, uint8_t t,
                             uint32_t Rt, uint8_t res, uint8_t thermistor_bit)
{
  config(R, B, t, Rt);
  setPin(pin, res);
}

// настроить термистор: R резистора, B термистора, t термистора, термистора
void
thermistor::config(uint32_t R, uint16_t B, uint8_t t, uint32_t Rt,
                   uint8_t thermistor_bit)
{
  _beta           = B;
  _tempBase       = t;
  _thermistor_bit = thermistor_bit;
  _baseDivRes     = (float)R / Rt;
}

// настроить пин и разрешение АЦП
void
thermistor::setPin(uint8_t pin, uint8_t res, uint8_t bit)
{
  _pin            = pin;
  _res            = res;
  _thermistor_bit = bit;
}

// прочитать температуру с пина
float
thermistor::getTemp()
{
  int reading;
  analogue_reader.read_adc(_thermistor_bit, reading);
  return computeTemp(reading);
}

// прочитать усреднённую температуру с пина, можно указать к-во усреднений
float
thermistor::getTempAverage(uint8_t samples)
{
  uint32_t aver = 0;
  int reading;
  for (uint8_t i = 0; i < samples; i++) {
    analogue_reader.read_adc(_thermistor_bit, reading);
    aver += reading;
  }
  return computeTemp((float)aver / samples, _res);
}

// получить температуру из сигнала АЦП, можно указать разрешение АЦП
float
thermistor::computeTemp(float analog, uint8_t res)
{
  return NTC_compute(analog, _baseDivRes, _beta, _tempBase, res);
}

void
temperature_task(void* arg)
{
  regulator.config(1, 1);
  motor.config(1, 1);
  thermistor* sensor = &motor;
  auto sensor_bit    = *static_cast<int*>(arg);
  if (sensor_bit == regulator.getBit()) {
    sensor = &regulator;
  }
  for (;;) {
    uint32_t avg_temperature =
        static_cast<uint32_t>(roundf(sensor->getTempAverage(10)));
    ulTaskNotifyTake(0, pdMS_TO_TICKS(ONE_SECOND_MS));
    analogue_reader.adc_set(sensor_bit, avg_temperature);
    vTaskDelay(40);
  }
}