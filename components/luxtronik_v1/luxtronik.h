/*
 * Copyright (c) 2024-2025 Jens-Uwe Rossbach
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

#include "luxtronik_sensor.h"
#include "simple_queue.h"
#include "simple_timer.h"

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

#include <cstdint>
#include <string>
#include <vector>


namespace esphome::luxtronik_v1
{
    class Luxtronik : public PollingComponent
    {
    public:
        enum OperationalModeType
        {
            HEATING   = 0,
            HOT_WATER = 1
        };

        struct HeatingCurves
        {
            bool hc_return_offset_avail;
            float hc_return_offset;
            bool hc_endpoint_avail;
            float hc_endpoint;
            bool hc_parallel_shift_avail;
            float hc_parallel_shift;
            bool hc_night_setback_avail;
            float hc_night_setback;
            bool hc_const_return_avail;
            float hc_const_return;
            bool mc1_endpoint_avail;
            float mc1_endpoint;
            bool mc1_parallel_shift_avail;
            float mc1_parallel_shift;
            bool mc1_night_setback_avail;
            float mc1_night_setback;
            bool mc1_const_flow_avail;
            float mc1_const_flow;
        };

        Luxtronik(
            uart::UARTComponent* uart,
            uint16_t request_delay,
            uint16_t response_timeout,
            uint16_t max_retries);
        virtual ~Luxtronik() = default;

        void update() override;
        void loop() override;
        void dump_config() override;

        void add_dataset(const char* code);
        void set_operational_mode(OperationalModeType type, uint8_t value);
        void set_hot_water_set_temperature(float value);
        void set_heating_curves(HeatingCurves& value);

#ifdef USE_SENSOR
        void set_sensor_flow_temperature(sensor::Sensor* sensor)                 { m_sensor_flow_temperature.set_sensor(sensor);                 }
        void set_sensor_return_temperature(sensor::Sensor* sensor)               { m_sensor_return_temperature.set_sensor(sensor);               }
        void set_sensor_return_set_temperature(sensor::Sensor* sensor)           { m_sensor_return_set_temperature.set_sensor(sensor);           }
        void set_sensor_hot_gas_temperature(sensor::Sensor* sensor)              { m_sensor_hot_gas_temperature.set_sensor(sensor);              }
        void set_sensor_outside_temperature(sensor::Sensor* sensor)              { m_sensor_outside_temperature.set_sensor(sensor);              }
        void set_sensor_hot_water_temperature(sensor::Sensor* sensor)            { m_sensor_hot_water_temperature.set_sensor(sensor);            }
        void set_sensor_hot_water_set_temperature(sensor::Sensor* sensor)        { m_sensor_hot_water_set_temperature.set_sensor(sensor);        }
        void set_sensor_heat_source_input_temperature(sensor::Sensor* sensor)    { m_sensor_heat_source_input_temperature.set_sensor(sensor);    }
        void set_sensor_heat_source_output_temperature(sensor::Sensor* sensor)   { m_sensor_heat_source_output_temperature.set_sensor(sensor);   }
        void set_sensor_mixed_circuit_1_temperature(sensor::Sensor* sensor)      { m_sensor_mixed_circuit_1_temperature.set_sensor(sensor);      }
        void set_sensor_mixed_circuit_1_set_temperature(sensor::Sensor* sensor)  { m_sensor_mixed_circuit_1_set_temperature.set_sensor(sensor);  }
        void set_sensor_remote_adjuster_temperature(sensor::Sensor* sensor)      { m_sensor_remote_adjuster_temperature.set_sensor(sensor);      }
        void set_sensor_heating_curve_hc_return_offset(sensor::Sensor* sensor)   { m_sensor_heating_curve_hc_return_offset.set_sensor(sensor);   }
        void set_sensor_heating_curve_hc_endpoint(sensor::Sensor* sensor)        { m_sensor_heating_curve_hc_endpoint.set_sensor(sensor);        }
        void set_sensor_heating_curve_hc_parallel_shift(sensor::Sensor* sensor)  { m_sensor_heating_curve_hc_parallel_shift.set_sensor(sensor);  }
        void set_sensor_heating_curve_hc_night_setback(sensor::Sensor* sensor)   { m_sensor_heating_curve_hc_night_setback.set_sensor(sensor);   }
        void set_sensor_heating_curve_hc_constant_return(sensor::Sensor* sensor) { m_sensor_heating_curve_hc_constant_return.set_sensor(sensor); }
        void set_sensor_heating_curve_mc1_endpoint(sensor::Sensor* sensor)       { m_sensor_heating_curve_mc1_endpoint.set_sensor(sensor);       }
        void set_sensor_heating_curve_mc1_parallel_shift(sensor::Sensor* sensor) { m_sensor_heating_curve_mc1_parallel_shift.set_sensor(sensor); }
        void set_sensor_heating_curve_mc1_night_setback(sensor::Sensor* sensor)  { m_sensor_heating_curve_mc1_night_setback.set_sensor(sensor);  }
        void set_sensor_heating_curve_mc1_constant_flow(sensor::Sensor* sensor)  { m_sensor_heating_curve_mc1_constant_flow.set_sensor(sensor);  }
        void set_sensor_impulses_compressor_1(sensor::Sensor* sensor)            { m_sensor_impulses_compressor_1.set_sensor(sensor);            }
        void set_sensor_impulses_compressor_2(sensor::Sensor* sensor)            { m_sensor_impulses_compressor_2.set_sensor(sensor);            }
#endif
#ifdef USE_BINARY_SENSOR
        void set_sensor_defrost_brine_flow(binary_sensor::BinarySensor* sensor)         { m_sensor_defrost_brine_flow.set_sensor(sensor);         }
        void set_sensor_power_provider_lock_period(binary_sensor::BinarySensor* sensor) { m_sensor_power_provider_lock_period.set_sensor(sensor); }
        void set_sensor_high_pressure_state(binary_sensor::BinarySensor* sensor)        { m_sensor_high_pressure_state.set_sensor(sensor);        }
        void set_sensor_engine_protection(binary_sensor::BinarySensor* sensor)          { m_sensor_engine_protection.set_sensor(sensor);          }
        void set_sensor_low_pressure_state(binary_sensor::BinarySensor* sensor)         { m_sensor_low_pressure_state.set_sensor(sensor);         }
        void set_sensor_external_power(binary_sensor::BinarySensor* sensor)             { m_sensor_external_power.set_sensor(sensor);             }
        void set_sensor_defrost_valve(binary_sensor::BinarySensor* sensor)              { m_sensor_defrost_valve.set_sensor(sensor);              }
        void set_sensor_hot_water_pump(binary_sensor::BinarySensor* sensor)             { m_sensor_hot_water_pump.set_sensor(sensor);             }
        void set_sensor_floor_heating_pump(binary_sensor::BinarySensor* sensor)         { m_sensor_floor_heating_pump.set_sensor(sensor);         }
        void set_sensor_heating_pump(binary_sensor::BinarySensor* sensor)               { m_sensor_heating_pump.set_sensor(sensor);               }
        void set_sensor_housing_ventilation(binary_sensor::BinarySensor* sensor)        { m_sensor_housing_ventilation.set_sensor(sensor);        }
        void set_sensor_ventilation_pump(binary_sensor::BinarySensor* sensor)           { m_sensor_ventilation_pump.set_sensor(sensor);           }
        void set_sensor_compressor_1(binary_sensor::BinarySensor* sensor)               { m_sensor_compressor_1.set_sensor(sensor);               }
        void set_sensor_compressor_2(binary_sensor::BinarySensor* sensor)               { m_sensor_compressor_2.set_sensor(sensor);               }
        void set_sensor_extra_pump(binary_sensor::BinarySensor* sensor)                 { m_sensor_extra_pump.set_sensor(sensor);                 }
        void set_sensor_secondary_heater_1(binary_sensor::BinarySensor* sensor)         { m_sensor_secondary_heater_1.set_sensor(sensor);         }
        void set_sensor_secondary_heater_2_failure(binary_sensor::BinarySensor* sensor) { m_sensor_secondary_heater_2_failure.set_sensor(sensor); }
        void set_sensor_device_communication(binary_sensor::BinarySensor* sensor)       { m_sensor_device_communication.set_sensor(sensor);       }
#endif
#ifdef USE_TEXT_SENSOR
        void set_sensor_mixer_1_state(text_sensor::TextSensor* sensor)                       { m_sensor_mixer_1_state.set_sensor(sensor);                       }
        void set_sensor_device_type(text_sensor::TextSensor* sensor)                         { m_sensor_device_type.set_sensor(sensor);                         }
        void set_sensor_firmware_version(text_sensor::TextSensor* sensor)                    { m_sensor_firmware_version.set_sensor(sensor);                    }
        void set_sensor_bivalence_level(text_sensor::TextSensor* sensor)                     { m_sensor_bivalence_level.set_sensor(sensor);                     }
        void set_sensor_operational_state(text_sensor::TextSensor* sensor)                   { m_sensor_operational_state.set_sensor(sensor);                   }
        void set_sensor_heating_mode(text_sensor::TextSensor* sensor)                        { m_sensor_heating_mode.set_sensor(sensor);                        }
        void set_sensor_hot_water_mode(text_sensor::TextSensor* sensor)                      { m_sensor_hot_water_mode.set_sensor(sensor);                      }
        void set_sensor_error_1_code(text_sensor::TextSensor* sensor)                        { m_sensor_error_1_code.set_sensor(sensor);                        }
        void set_sensor_error_2_code(text_sensor::TextSensor* sensor)                        { m_sensor_error_2_code.set_sensor(sensor);                        }
        void set_sensor_error_3_code(text_sensor::TextSensor* sensor)                        { m_sensor_error_3_code.set_sensor(sensor);                        }
        void set_sensor_error_4_code(text_sensor::TextSensor* sensor)                        { m_sensor_error_4_code.set_sensor(sensor);                        }
        void set_sensor_error_5_code(text_sensor::TextSensor* sensor)                        { m_sensor_error_5_code.set_sensor(sensor);                        }
        void set_sensor_error_1_time(text_sensor::TextSensor* sensor)                        { m_sensor_error_1_time.set_sensor(sensor);                        }
        void set_sensor_error_2_time(text_sensor::TextSensor* sensor)                        { m_sensor_error_2_time.set_sensor(sensor);                        }
        void set_sensor_error_3_time(text_sensor::TextSensor* sensor)                        { m_sensor_error_3_time.set_sensor(sensor);                        }
        void set_sensor_error_4_time(text_sensor::TextSensor* sensor)                        { m_sensor_error_4_time.set_sensor(sensor);                        }
        void set_sensor_error_5_time(text_sensor::TextSensor* sensor)                        { m_sensor_error_5_time.set_sensor(sensor);                        }
        void set_sensor_deactivation_1_code(text_sensor::TextSensor* sensor)                 { m_sensor_deactivation_1_code.set_sensor(sensor);                 }
        void set_sensor_deactivation_2_code(text_sensor::TextSensor* sensor)                 { m_sensor_deactivation_2_code.set_sensor(sensor);                 }
        void set_sensor_deactivation_3_code(text_sensor::TextSensor* sensor)                 { m_sensor_deactivation_3_code.set_sensor(sensor);                 }
        void set_sensor_deactivation_4_code(text_sensor::TextSensor* sensor)                 { m_sensor_deactivation_4_code.set_sensor(sensor);                 }
        void set_sensor_deactivation_5_code(text_sensor::TextSensor* sensor)                 { m_sensor_deactivation_5_code.set_sensor(sensor);                 }
        void set_sensor_deactivation_1_time(text_sensor::TextSensor* sensor)                 { m_sensor_deactivation_1_time.set_sensor(sensor);                 }
        void set_sensor_deactivation_2_time(text_sensor::TextSensor* sensor)                 { m_sensor_deactivation_2_time.set_sensor(sensor);                 }
        void set_sensor_deactivation_3_time(text_sensor::TextSensor* sensor)                 { m_sensor_deactivation_3_time.set_sensor(sensor);                 }
        void set_sensor_deactivation_4_time(text_sensor::TextSensor* sensor)                 { m_sensor_deactivation_4_time.set_sensor(sensor);                 }
        void set_sensor_deactivation_5_time(text_sensor::TextSensor* sensor)                 { m_sensor_deactivation_5_time.set_sensor(sensor);                 }

        void set_sensor_operating_hours_compressor_1(text_sensor::TextSensor* sensor, int32_t format)        { m_sensor_operating_hours_compressor_1.set_sensor(sensor, format);        }
        void set_sensor_average_operating_time_compressor_1(text_sensor::TextSensor* sensor, int32_t format) { m_sensor_average_operating_time_compressor_1.set_sensor(sensor, format); }
        void set_sensor_operating_hours_compressor_2(text_sensor::TextSensor* sensor, int32_t format)        { m_sensor_operating_hours_compressor_2.set_sensor(sensor, format);        }
        void set_sensor_average_operating_time_compressor_2(text_sensor::TextSensor* sensor, int32_t format) { m_sensor_average_operating_time_compressor_2.set_sensor(sensor, format); }
        void set_sensor_operating_hours_secondary_heater_1(text_sensor::TextSensor* sensor, int32_t format)  { m_sensor_operating_hours_secondary_heater_1.set_sensor(sensor, format);  }
        void set_sensor_operating_hours_secondary_heater_2(text_sensor::TextSensor* sensor, int32_t format)  { m_sensor_operating_hours_secondary_heater_2.set_sensor(sensor, format);  }
        void set_sensor_operating_hours_heat_pump(text_sensor::TextSensor* sensor, int32_t format)           { m_sensor_operating_hours_heat_pump.set_sensor(sensor, format);           }
#endif

    private:
        void next_dataset();
        void request_data();
        void send_request(const std::string& request);
        void parse_response(const std::string& response);
        void parse_temperatures(const std::string& response);
        void parse_inputs(const std::string& response);
        void parse_outputs(const std::string& response);
        void parse_operating_hours(const std::string& response);
        void parse_errors(const std::string& response);
        void parse_deactivations(const std::string& response);
        void parse_information(const std::string& response);
        void parse_heating_curves(const std::string& response);
        void parse_heating_mode(const std::string& response);
        void parse_hot_water_config(const std::string& response);
        void parse_hot_water_mode(const std::string& response);
        void handle_timeout();
        void clear_uart_buffer();
        void parse_slot(const std::string& slot, StringSensor& sensor_code, StringSensor& sensor_time);

        void accept_response()
        {
            m_timer.cancel();
            m_retry_count = 0;
        }

        static int32_t get_number(const std::string& input_str, uint16_t start, uint16_t end)       { return std::atoi(input_str.substr(start, end - start).c_str()); }
        static bool    get_binary_state(const std::string& input_str, uint16_t start, uint16_t end) { return input_str.substr(start, end - start) != "0";             }
        static bool    starts_with(const std::string& str, const std::string& with)                 { return (str.compare(0, with.length(), with) == 0);              }

    protected:
        uart::UARTDevice m_device;

        // temperatures
        TemperatureSensor m_sensor_flow_temperature;                 // 1100:2  - Ist-Temperatur Vorlauf Heizkreis
        TemperatureSensor m_sensor_return_temperature;               // 1100:3  - Ist-Temperatur Rücklauf Heizkreis
        TemperatureSensor m_sensor_return_set_temperature;           // 1100:4  - Soll-Temperatur Rücklauf Heizkreis
        TemperatureSensor m_sensor_hot_gas_temperature;              // 1100:5  - Temperatur Heißgas
        TemperatureSensor m_sensor_outside_temperature;              // 1100:6  - Außentemperatur
        TemperatureSensor m_sensor_hot_water_temperature;            // 1100:7  - Ist-Temperatur Brauchwarmwasser
        TemperatureSensor m_sensor_hot_water_set_temperature;        // 1100:8  - Soll-Temperatur Brauchwarmwasser
        TemperatureSensor m_sensor_heat_source_input_temperature;    // 1100:9  - Temperatur Wärmequelleneintritt
        TemperatureSensor m_sensor_heat_source_output_temperature;   // 1100:10 - Temperatur Wärmequellenaustritt
        TemperatureSensor m_sensor_mixed_circuit_1_temperature;      // 1100:11 - Ist-Temperatur Vorlauf Mischkreis 1
        TemperatureSensor m_sensor_mixed_circuit_1_set_temperature;  // 1100:12 - Soll-Temperatur Vorlauf Mischkreis 1
        TemperatureSensor m_sensor_remote_adjuster_temperature;      // 1100:13 - Temperatur Raumfernversteller

        // inputs
        BoolSensor m_sensor_defrost_brine_flow;          // 1200:2 - Abtau, Soledruck, Durchfluss
        BoolSensor m_sensor_power_provider_lock_period;  // 1200:3 - Sperrzeit EVU
        BoolSensor m_sensor_high_pressure_state;         // 1200:4 - Hodruckpressostat
        BoolSensor m_sensor_engine_protection;           // 1200:5 - Motorschutz
        BoolSensor m_sensor_low_pressure_state;          // 1200:6 - Niederdruckpressostat
        BoolSensor m_sensor_external_power;              // 1200:7 - Fremdstromanode

        // outputs
        BoolSensor   m_sensor_defrost_valve;               // 1300:2   - Abtauventil
        BoolSensor   m_sensor_hot_water_pump;              // 1300:3   - Brauchwarmwasserumwälzpumpe
        BoolSensor   m_sensor_floor_heating_pump;          // 1300:4   - Fußbodenheizungsumwälzpumpe
        BoolSensor   m_sensor_heating_pump;                // 1300:5   - Heizungsumwälzpumpe
        StringSensor m_sensor_mixer_1_state;               // 1300:6,7 - Mischer 1 fährt auf, Mischer 1 fährt zu
        BoolSensor   m_sensor_housing_ventilation;         // 1300:8   - Ventilation Wärmepumpengehäuse
        BoolSensor   m_sensor_ventilation_pump;            // 1300:9   - Ventilator, Brunnen- oder Soleumwälzpumpe
        BoolSensor   m_sensor_compressor_1;                // 1300:10  - Verdichter 1 in Wärmepumpe
        BoolSensor   m_sensor_compressor_2;                // 1300:11  - Verdichter 2 in Wärmepumpe
        BoolSensor   m_sensor_extra_pump;                  // 1300:12  - Zusatzumwälzpumpe - Zirkulationspumpe
        BoolSensor   m_sensor_secondary_heater_1;          // 1300:13  - Zweiter Wärmeerzeuger 1
        BoolSensor   m_sensor_secondary_heater_2_failure;  // 1300:14  - Zweiter Wärmeerzeuger 2 - Sammelstörung

        // information
        StringSensor m_sensor_device_type;        // 1700:2 - Wärmepumpentyp
        StringSensor m_sensor_firmware_version;   // 1700:3 - Software-Stand Firmware
        StringSensor m_sensor_bivalence_level;    // 1700:4 - Bivalenzstufe
        StringSensor m_sensor_operational_state;  // 1700:5 - Betriebszustand

        // heat curve
        TemperatureSensor m_sensor_heating_curve_hc_return_offset;    // 3400:2  - Heizkurve HK Abweichung Rücklauf
        TemperatureSensor m_sensor_heating_curve_hc_endpoint;         // 3400:3  - Heizkurve HK Endpunkt
        TemperatureSensor m_sensor_heating_curve_hc_parallel_shift;   // 3400:4  - Heizkurve HK Parallelverschiebung
        TemperatureSensor m_sensor_heating_curve_hc_night_setback;    // 3400:5  - Heizkurve HK Nachtabsenkung
        TemperatureSensor m_sensor_heating_curve_hc_constant_return;  // 3400:6  - Heizkurve HK Festwert Rücklauf
        TemperatureSensor m_sensor_heating_curve_mc1_endpoint;        // 3400:7  - Heizkurve MK1 Endpunkt
        TemperatureSensor m_sensor_heating_curve_mc1_parallel_shift;  // 3400:8  - Heizkurve MK1 Parallelverschiebung
        TemperatureSensor m_sensor_heating_curve_mc1_night_setback;   // 3400:9  - Heizkurve MK1 Nachtabsenkung
        TemperatureSensor m_sensor_heating_curve_mc1_constant_flow;   // 3400:10 - Heizkurve MK1 Festwert Vorlauf

        // modes
        StringSensor m_sensor_heating_mode;    // 3405:2 - Betriebsart Heizung
        StringSensor m_sensor_hot_water_mode;  // 3505:2 - Betriebsart Brauchwarmwasser

        // operating hours
        DurationSensor m_sensor_operating_hours_compressor_1;         // 1450:2  - Betriebsstunden Verdichter 1
        NumericSensor  m_sensor_impulses_compressor_1;                // 1450:3  - Impulse Verdichter 1
        DurationSensor m_sensor_average_operating_time_compressor_1;  // 1450:4  - Durchschnittliche Laufzeit Verdichter 1
        DurationSensor m_sensor_operating_hours_compressor_2;         // 1450:5  - Betriebsstunden Verdichter 2
        NumericSensor  m_sensor_impulses_compressor_2;                // 1450:6  - Impulse Verdichter 2
        DurationSensor m_sensor_average_operating_time_compressor_2;  // 1450:7  - Durchschnittliche Laufzeit Verdichter 2
        DurationSensor m_sensor_operating_hours_secondary_heater_1;   // 1450:8  - Betriebsstunden Zweiter Wärmeerzeuger 1
        DurationSensor m_sensor_operating_hours_secondary_heater_2;   // 1450:9  - Betriebsstunden Zweiter Wärmeerzeuger 2
        DurationSensor m_sensor_operating_hours_heat_pump;            // 1450:10 - Betriebsstunden Wärmepumpe

        // errors
        StringSensor m_sensor_error_1_code;  // 1500:1500:2   - Fehler 1 - Code
        StringSensor m_sensor_error_2_code;  // 1500:1501:2   - Fehler 2 - Code
        StringSensor m_sensor_error_3_code;  // 1500:1502:2   - Fehler 3 - Code
        StringSensor m_sensor_error_4_code;  // 1500:1503:2   - Fehler 4 - Code
        StringSensor m_sensor_error_5_code;  // 1500:1504:2   - Fehler 5 - Code
        StringSensor m_sensor_error_1_time;  // 1500:1500:2-6 - Fehler 1 - Datum/Uhrzeit
        StringSensor m_sensor_error_2_time;  // 1500:1501:2-6 - Fehler 2 - Datum/Uhrzeit
        StringSensor m_sensor_error_3_time;  // 1500:1502:2-6 - Fehler 3 - Datum/Uhrzeit
        StringSensor m_sensor_error_4_time;  // 1500:1503:2-6 - Fehler 4 - Datum/Uhrzeit
        StringSensor m_sensor_error_5_time;  // 1500:1504:2-6 - Fehler 5 - Datum/Uhrzeit

        // deactivations
        StringSensor m_sensor_deactivation_1_code;  // 1600:1600:2   - Abschaltung 1 - Code
        StringSensor m_sensor_deactivation_2_code;  // 1600:1601:2   - Abschaltung 2 - Code
        StringSensor m_sensor_deactivation_3_code;  // 1600:1602:2   - Abschaltung 3 - Code
        StringSensor m_sensor_deactivation_4_code;  // 1600:1603:2   - Abschaltung 4 - Code
        StringSensor m_sensor_deactivation_5_code;  // 1600:1604:2   - Abschaltung 5 - Code
        StringSensor m_sensor_deactivation_1_time;  // 1600:1600:2-6 - Abschaltung 1 - Datum/Uhrzeit
        StringSensor m_sensor_deactivation_2_time;  // 1600:1601:2-6 - Abschaltung 2 - Datum/Uhrzeit
        StringSensor m_sensor_deactivation_3_time;  // 1600:1602:2-6 - Abschaltung 3 - Datum/Uhrzeit
        StringSensor m_sensor_deactivation_4_time;  // 1600:1603:2-6 - Abschaltung 4 - Datum/Uhrzeit
        StringSensor m_sensor_deactivation_5_time;  // 1600:1604:2-6 - Abschaltung 5 - Datum/Uhrzeit

        // diagnostic
        BoolSensor m_sensor_device_communication;

        // configuration
        uint16_t m_request_delay;
        uint16_t m_response_timeout;
        uint16_t m_max_retries;

        // helpers
        char m_response_buffer[255];
        size_t m_cursor;
        bool m_lost_response;
        bool m_response_ready;
        bool m_slot_block;
        uint8_t m_config_response_state;
        uint16_t m_retry_count;
        std::string m_store_config_ack;
        std::vector<const char*> m_dataset_list;
        SimpleQueue<std::string> m_request_queue;
        SimpleTimer m_timer;
    };
}  // namespace esphome::luxtronik_v1
