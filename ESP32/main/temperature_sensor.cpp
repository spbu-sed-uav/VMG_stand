// #include "temperature_sensor.h"
// // сигнал АЦП, (R резистора / R термистора), B термистора, t термистора,
// разрешение АЦП float thermistor::NTC_compute(float analog, float baseDiv,
// uint16_t B, uint8_t t, uint8_t res) {
//     if (analog <= 0 || isnan(analog)) return INFINITY;
//     analog = baseDiv / ((float)((1 << res) - 1) / analog - 1.0f);
//     analog = (std::log(analog) / B) + 1.0f / (t + 273.15f);
//     return (1.0f / analog - 273.15f);
// }

// // сигнал АЦП, R резистора, B термистора, t термистора, R термистора,
// разрешение АЦП float thermistor::NTC_compute(float analog, uint32_t R,
// uint16_t B, uint8_t t, uint32_t Rt, uint8_t res) {
//     return NTC_compute(analog, (float)R / Rt, B, t, res);
// }

//     // пин, R резистора, B термистора, t термистора, R термистора, разрешение
//     АЦП thermistor::setup_thermistor(uint8_t pin, uint32_t R, uint16_t B,
//     uint8_t t = 25, uint32_t Rt = 10000, uint8_t res = 10)
//     {
//         config(R, B, t, Rt);
//         setPin(pin, res);
//     }

//     // настроить термистор: R резистора, B термистора, t термистора, R
//     термистора void thermistor::config(uint32_t R, uint16_t B, uint8_t t =
//     25, uint32_t Rt = 10000)
//     {
//         _beta = B;
//         _tempBase = t;
//         _baseDivRes = (float)R / Rt;
//     }

//     // настроить пин и разрешение АЦП
//     void thermistor::setPin(uint8_t pin, uint8_t res = 10)
//     {
//         _pin = pin;
//         _res = res;
//     }

//     // прочитать температуру с пина
//     float thermistor::getTemp()
//     {
//         return computeTemp(analogRead(_pin), _res);
//     }

//     // прочитать усреднённую температуру с пина, можно указать к-во
//     усреднений float thermistor::getTempAverage(uint8_t samples = 20)
//     {
//         uint16_t aver = 0;
//         for (uint8_t i = 0; i < samples; i++)
//             aver += analogRead(_pin);
//         return computeTemp((float)aver / samples, _res);
//     }

//     // получить температуру из сигнала АЦП, можно указать разрешение АЦП
//     float thermistor::computeTemp(float analog, uint8_t res = 10)
//     {
//         return NTC_compute(analog, _baseDivRes, _beta, _tempBase, res);
//     }
