#pragma once
#include <iostream>

constexpr uint8_t WEIGHTS_MAX_AMOUNT{8};
constexpr uint8_t WEIGHTS_MAX_DRIVERS{8};



class Weights_Interface
{

};

struct Weight_Sensor
{
    uint32_t AVG_amount_samples;
    uint32_t weight; // maybe add more
    uint32_t sensor_id;
};
class AP_Weight_Backend
{
public:

    uint32_t get_weight(void){return weights[primary].weight;};
    uint32_t get_weight(uint8_t instance){return weights[instance].weight;};

    uint32_t get_id(void){return weights[primary].sensor_id;};
    uint32_t get_id(uint8_t instance){return weights[instance].sensor_id;};

    uint32_t get_avg_sample(void){return weights[primary].AVG_amount_samples;};
    uint32_t get_avg_sample(uint8_t instance){return weights[instance].AVG_amount_samples;};

    void set_avg_sample(uint8_t number){weights[primary].AVG_amount_samples=number;}
    void set_avg_sample(uint8_t number, uint8_t instance){weights[instance].AVG_amount_samples=number;}

    // logs
    void log_weight();

    void log_weight(uint64_t ct_time, uint8_t weight_sensor);

private:
    const char *WEIGHT_TAG{"WEIGHTS"};

    uint8_t amount_of_sensors;
    uint8_t primary; // primary sensor

    // backend readings
    Weights_Interface backend[WEIGHTS_MAX_DRIVERS];

    // actual sensors
    Weight_Sensor weights[WEIGHTS_MAX_AMOUNT];
};
