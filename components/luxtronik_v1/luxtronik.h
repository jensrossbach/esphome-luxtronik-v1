/*
 * Copyright (c) 2024 Jens-Uwe Rossbach
 *
 * This code is licensed under the MIT License.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


#pragma once

#include "esphome/core/defines.h"
#include "esphome/core/component.h"
#ifdef USE_SENSOR
#include "esphome/components/sensor/sensor.h"
#endif
#ifdef USE_BINARY_SENSOR
#include "esphome/components/binary_sensor/binary_sensor.h"
#endif
#ifdef USE_TEXT_SENSOR
#include "esphome/components/text_sensor/text_sensor.h"
#endif
#include "esphome/components/uart/uart.h"
#include "esphome/core/hal.h"

#include <string>


namespace esphome::luxtronik_v1
{
    class Luxtronik : public PollingComponent
    {
    public:
        Luxtronik(uart::UARTComponent* uart);
        virtual ~Luxtronik() = default;

        void loop() override;
        void update() override;

        void dump_config() override;

#ifdef USE_SENSOR
        void set_sensor_flow_temperature(sensor::Sensor* sensor)                        { m_sensor_flow_temperature                = sensor; }
        void set_sensor_return_temperature(sensor::Sensor* sensor)                      { m_sensor_return_temperature              = sensor; }
        void set_sensor_return_set_temperature(sensor::Sensor* sensor)                  { m_sensor_return_set_temperature          = sensor; }
        void set_sensor_hot_gas_temperature(sensor::Sensor* sensor)                     { m_sensor_hot_gas_temperature             = sensor; }
        void set_sensor_outside_temperature(sensor::Sensor* sensor)                     { m_sensor_outside_temperature             = sensor; }
        void set_sensor_hot_water_temperature(sensor::Sensor* sensor)                   { m_sensor_hot_water_temperature           = sensor; }
        void set_sensor_hot_water_set_temperature(sensor::Sensor* sensor)               { m_sensor_hot_water_set_temperature       = sensor; }
        void set_sensor_heat_source_input_temperature(sensor::Sensor* sensor)           { m_sensor_heat_source_input_temperature   = sensor; }
        void set_sensor_heat_source_output_temperature(sensor::Sensor* sensor)          { m_sensor_heat_source_output_temperature  = sensor; }
        void set_sensor_mixed_circuit_1_temperature(sensor::Sensor* sensor)             { m_sensor_mixed_circuit_1_temperature     = sensor; }
        void set_sensor_mixed_circuit_1_set_temperature(sensor::Sensor* sensor)         { m_sensor_mixed_circuit_1_set_temperature = sensor; }
        void set_sensor_remote_adjuster_temperature(sensor::Sensor* sensor)             { m_sensor_remote_adjuster_temperature     = sensor; }
#endif
#ifdef USE_BINARY_SENSOR
        void set_sensor_defrost_brine_flow(binary_sensor::BinarySensor* sensor)         { m_sensor_defrost_brine_flow              = sensor; }
        void set_sensor_power_provider_lock_period(binary_sensor::BinarySensor* sensor) { m_sensor_power_provider_lock_period      = sensor; }
        void set_sensor_high_pressure_state(binary_sensor::BinarySensor* sensor)        { m_sensor_high_pressure_state             = sensor; }
        void set_sensor_engine_protection(binary_sensor::BinarySensor* sensor)          { m_sensor_engine_protection               = sensor; }
        void set_sensor_low_pressure_state(binary_sensor::BinarySensor* sensor)         { m_sensor_low_pressure_state              = sensor; }
        void set_sensor_external_power(binary_sensor::BinarySensor* sensor)             { m_sensor_external_power                  = sensor; }
        void set_sensor_defrost_valve(binary_sensor::BinarySensor* sensor)              { m_sensor_defrost_valve                   = sensor; }
        void set_sensor_hot_water_pump(binary_sensor::BinarySensor* sensor)             { m_sensor_hot_water_pump                  = sensor; }
        void set_sensor_floor_heating_pump(binary_sensor::BinarySensor* sensor)         { m_sensor_floor_heating_pump              = sensor; }
        void set_sensor_heating_pump(binary_sensor::BinarySensor* sensor)               { m_sensor_heating_pump                    = sensor; }
        void set_sensor_housing_ventilation(binary_sensor::BinarySensor* sensor)        { m_sensor_housing_ventilation             = sensor; }
        void set_sensor_ventilation_pump(binary_sensor::BinarySensor* sensor)           { m_sensor_ventilation_pump                = sensor; }
        void set_sensor_compressor_1(binary_sensor::BinarySensor* sensor)               { m_sensor_compressor_1                    = sensor; }
        void set_sensor_compressor_2(binary_sensor::BinarySensor* sensor)               { m_sensor_compressor_2                    = sensor; }
        void set_sensor_extra_pump(binary_sensor::BinarySensor* sensor)                 { m_sensor_extra_pump                      = sensor; }
        void set_sensor_secondary_heater_1(binary_sensor::BinarySensor* sensor)         { m_sensor_secondary_heater_1              = sensor; }
        void set_sensor_secondary_heater_2_failure(binary_sensor::BinarySensor* sensor) { m_sensor_secondary_heater_2_failure      = sensor; }
        void set_sensor_device_communication(binary_sensor::BinarySensor* sensor)       { m_sensor_device_communication            = sensor; }
#endif
#ifdef USE_TEXT_SENSOR
        void set_sensor_mixer_1_state(text_sensor::TextSensor* sensor)                  { m_sensor_mixer_1_state                   = sensor; }
        void set_sensor_device_type(text_sensor::TextSensor* sensor)                    { m_sensor_device_type                     = sensor; }
        void set_sensor_firmware_version(text_sensor::TextSensor* sensor)               { m_sensor_firmware_version                = sensor; }
        void set_sensor_bivalence_level(text_sensor::TextSensor* sensor)                { m_sensor_bivalence_level                 = sensor; }
        void set_sensor_operational_state(text_sensor::TextSensor* sensor)              { m_sensor_operational_state               = sensor; }
        void set_sensor_heating_mode(text_sensor::TextSensor* sensor)                   { m_sensor_heating_mode                    = sensor; }
        void set_sensor_hot_water_mode(text_sensor::TextSensor* sensor)                 { m_sensor_hot_water_mode                  = sensor; }
        void set_sensor_error_1_code(text_sensor::TextSensor* sensor)                   { m_sensor_error_1_code                    = sensor; }
        void set_sensor_error_2_code(text_sensor::TextSensor* sensor)                   { m_sensor_error_2_code                    = sensor; }
        void set_sensor_error_3_code(text_sensor::TextSensor* sensor)                   { m_sensor_error_3_code                    = sensor; }
        void set_sensor_error_4_code(text_sensor::TextSensor* sensor)                   { m_sensor_error_4_code                    = sensor; }
        void set_sensor_error_5_code(text_sensor::TextSensor* sensor)                   { m_sensor_error_5_code                    = sensor; }
        void set_sensor_error_1_time(text_sensor::TextSensor* sensor)                   { m_sensor_error_1_time                    = sensor; }
        void set_sensor_error_2_time(text_sensor::TextSensor* sensor)                   { m_sensor_error_2_time                    = sensor; }
        void set_sensor_error_3_time(text_sensor::TextSensor* sensor)                   { m_sensor_error_3_time                    = sensor; }
        void set_sensor_error_4_time(text_sensor::TextSensor* sensor)                   { m_sensor_error_4_time                    = sensor; }
        void set_sensor_error_5_time(text_sensor::TextSensor* sensor)                   { m_sensor_error_5_time                    = sensor; }
        void set_sensor_deactivation_1_code(text_sensor::TextSensor* sensor)            { m_sensor_deactivation_1_code             = sensor; }
        void set_sensor_deactivation_2_code(text_sensor::TextSensor* sensor)            { m_sensor_deactivation_2_code             = sensor; }
        void set_sensor_deactivation_3_code(text_sensor::TextSensor* sensor)            { m_sensor_deactivation_3_code             = sensor; }
        void set_sensor_deactivation_4_code(text_sensor::TextSensor* sensor)            { m_sensor_deactivation_4_code             = sensor; }
        void set_sensor_deactivation_5_code(text_sensor::TextSensor* sensor)            { m_sensor_deactivation_5_code             = sensor; }
        void set_sensor_deactivation_1_time(text_sensor::TextSensor* sensor)            { m_sensor_deactivation_1_time             = sensor; }
        void set_sensor_deactivation_2_time(text_sensor::TextSensor* sensor)            { m_sensor_deactivation_2_time             = sensor; }
        void set_sensor_deactivation_3_time(text_sensor::TextSensor* sensor)            { m_sensor_deactivation_3_time             = sensor; }
        void set_sensor_deactivation_4_time(text_sensor::TextSensor* sensor)            { m_sensor_deactivation_4_time             = sensor; }
        void set_sensor_deactivation_5_time(text_sensor::TextSensor* sensor)            { m_sensor_deactivation_5_time             = sensor; }
#endif

    private:
        void send_request(const char* req);
        void parse_response(const std::string& response);
        void clear_uart_buffer();
#ifdef USE_TEXT_SENSOR
        void parse_slot(const std::string& slot, text_sensor::TextSensor* sensor_code, text_sensor::TextSensor* sensor_time);
#endif

        static float get_temperature(const std::string& input_str)               { return std::atof(input_str.c_str()) / 10;          }
        static float get_number(const std::string& input_str)                    { return std::atoi(input_str.c_str());               }
        static bool get_binary_state(const std::string& input_str)               { return input_str != "0";                           }
        static bool starts_with(const std::string& str, const std::string& with) { return (str.compare(0, with.length(), with) == 0); }

    protected:
        uart::UARTDevice m_device;

#ifdef USE_SENSOR
        // Temperatures
        sensor::Sensor* m_sensor_flow_temperature;                 // 1100:2  - Ist-Temperatur Vorlauf Heizkreis
        sensor::Sensor* m_sensor_return_temperature;               // 1100:3  - Ist-Temperatur Rücklauf Heizkreis
        sensor::Sensor* m_sensor_return_set_temperature;           // 1100:4  - Soll-Temperatur Rücklauf Heizkreis
        sensor::Sensor* m_sensor_hot_gas_temperature;              // 1100:5  - Temperatur Heißgas
        sensor::Sensor* m_sensor_outside_temperature;              // 1100:6  - Außentemperatur
        sensor::Sensor* m_sensor_hot_water_temperature;            // 1100:7  - Ist-Temperatur Brauchwarmwasser
        sensor::Sensor* m_sensor_hot_water_set_temperature;        // 1100:8  - Soll-Temperatur Brauchwarmwasser
        sensor::Sensor* m_sensor_heat_source_input_temperature;    // 1100:9  - Temperatur Wärmequelleneintritt
        sensor::Sensor* m_sensor_heat_source_output_temperature;   // 1100:10 - Temperatur Wärmequellenaustritt
        sensor::Sensor* m_sensor_mixed_circuit_1_temperature;      // 1100:11 - Ist-Temperatur Vorlauf Mischkreis 1
        sensor::Sensor* m_sensor_mixed_circuit_1_set_temperature;  // 1100:12 - Soll-Temperatur Vorlauf Mischkreis 1
        sensor::Sensor* m_sensor_remote_adjuster_temperature;      // 1100:13 - Temperatur Raumfernversteller
#endif

#ifdef USE_BINARY_SENSOR
        // Inputs
        binary_sensor::BinarySensor* m_sensor_defrost_brine_flow;          // 1200:2 - Abtau, Soledruck, Durchfluss
        binary_sensor::BinarySensor* m_sensor_power_provider_lock_period;  // 1200:3 - Sperrzeit EVU
        binary_sensor::BinarySensor* m_sensor_high_pressure_state;         // 1200:4 - Hodruckpressostat
        binary_sensor::BinarySensor* m_sensor_engine_protection;           // 1200:5 - Motorschutz
        binary_sensor::BinarySensor* m_sensor_low_pressure_state;          // 1200:6 - Niederdruckpressostat
        binary_sensor::BinarySensor* m_sensor_external_power;              // 1200:7 - Fremdstromanode

        // Outputs
        binary_sensor::BinarySensor* m_sensor_defrost_valve;               // 1300:2   - Abtauventil
        binary_sensor::BinarySensor* m_sensor_hot_water_pump;              // 1300:3   - Brauchwarmwasserumwälzpumpe
        binary_sensor::BinarySensor* m_sensor_floor_heating_pump;          // 1300:4   - Fußbodenheizungsumwälzpumpe
        binary_sensor::BinarySensor* m_sensor_heating_pump;                // 1300:5   - Heizungsumwälzpumpe
        binary_sensor::BinarySensor* m_sensor_housing_ventilation;         // 1300:8   - Ventilation Wärmepumpengehäuse
        binary_sensor::BinarySensor* m_sensor_ventilation_pump;            // 1300:9   - Ventilator, Brunnen- oder Soleumwälzpumpe
        binary_sensor::BinarySensor* m_sensor_compressor_1;                // 1300:10  - Verdichter 1 in Wärmepumpe
        binary_sensor::BinarySensor* m_sensor_compressor_2;                // 1300:11  - Verdichter 2 in Wärmepumpe
        binary_sensor::BinarySensor* m_sensor_extra_pump;                  // 1300:12  - Zusatzumwälzpumpe - Zirkulationspumpe
        binary_sensor::BinarySensor* m_sensor_secondary_heater_1;          // 1300:13  - Zweiter Wärmeerzeuger 1
        binary_sensor::BinarySensor* m_sensor_secondary_heater_2_failure;  // 1300:14  - Zweiter Wärmeerzeuger 2 - Sammelstörung

        // Diagnostic
        binary_sensor::BinarySensor* m_sensor_device_communication;
#endif

#ifdef USE_TEXT_SENSOR
        // Outputs
        text_sensor::TextSensor* m_sensor_mixer_1_state;  // 1300:6,7 - Mischer 1 fährt auf, Mischer 1 fährt zu

        // Information
        text_sensor::TextSensor* m_sensor_device_type;        // 1700:2 - Wärmepumpentyp
        text_sensor::TextSensor* m_sensor_firmware_version;   // 1700:3 - Software-Stand Firmware
        text_sensor::TextSensor* m_sensor_bivalence_level;    // 1700:4 - Bivalenzstufe
        text_sensor::TextSensor* m_sensor_operational_state;  // 1700:5 - Betriebszustand

        // Modes
        text_sensor::TextSensor* m_sensor_heating_mode;    // 3405:2 - Betriebsart Heizung
        text_sensor::TextSensor* m_sensor_hot_water_mode;  // 3505:2 - Betriebsart Brauchwarmwasser

        // Errors
        text_sensor::TextSensor* m_sensor_error_1_code;  // 1500:1500:2   - Fehler 1 - Code
        text_sensor::TextSensor* m_sensor_error_2_code;  // 1500:1501:2   - Fehler 2 - Code
        text_sensor::TextSensor* m_sensor_error_3_code;  // 1500:1502:2   - Fehler 3 - Code
        text_sensor::TextSensor* m_sensor_error_4_code;  // 1500:1503:2   - Fehler 4 - Code
        text_sensor::TextSensor* m_sensor_error_5_code;  // 1500:1504:2   - Fehler 5 - Code
        text_sensor::TextSensor* m_sensor_error_1_time;  // 1500:1500:2-6 - Fehler 1 - Datum/Uhrzeit
        text_sensor::TextSensor* m_sensor_error_2_time;  // 1500:1501:2-6 - Fehler 2 - Datum/Uhrzeit
        text_sensor::TextSensor* m_sensor_error_3_time;  // 1500:1502:2-6 - Fehler 3 - Datum/Uhrzeit
        text_sensor::TextSensor* m_sensor_error_4_time;  // 1500:1503:2-6 - Fehler 4 - Datum/Uhrzeit
        text_sensor::TextSensor* m_sensor_error_5_time;  // 1500:1504:2-6 - Fehler 5 - Datum/Uhrzeit

        // Deactivations
        text_sensor::TextSensor* m_sensor_deactivation_1_code;  // 1600:1600:2   - Abschaltung 1 - Code
        text_sensor::TextSensor* m_sensor_deactivation_2_code;  // 1600:1601:2   - Abschaltung 2 - Code
        text_sensor::TextSensor* m_sensor_deactivation_3_code;  // 1600:1602:2   - Abschaltung 3 - Code
        text_sensor::TextSensor* m_sensor_deactivation_4_code;  // 1600:1603:2   - Abschaltung 4 - Code
        text_sensor::TextSensor* m_sensor_deactivation_5_code;  // 1600:1604:2   - Abschaltung 5 - Code
        text_sensor::TextSensor* m_sensor_deactivation_1_time;  // 1600:1600:2-6 - Abschaltung 1 - Datum/Uhrzeit
        text_sensor::TextSensor* m_sensor_deactivation_2_time;  // 1600:1601:2-6 - Abschaltung 2 - Datum/Uhrzeit
        text_sensor::TextSensor* m_sensor_deactivation_3_time;  // 1600:1602:2-6 - Abschaltung 3 - Datum/Uhrzeit
        text_sensor::TextSensor* m_sensor_deactivation_4_time;  // 1600:1603:2-6 - Abschaltung 4 - Datum/Uhrzeit
        text_sensor::TextSensor* m_sensor_deactivation_5_time;  // 1600:1604:2-6 - Abschaltung 5 - Datum/Uhrzeit
#endif

        char m_response_buffer[255];
        size_t m_cursor;
        uint16_t m_received_responses;
        bool m_response_ready;
        bool m_slot_block;
    };
}  // namespace esphome::luxtronik_v1
