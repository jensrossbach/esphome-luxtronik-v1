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


#include "luxtronik.h"
#include "esphome/core/log.h"

#include <chrono>
#include <ctime>
#include <cmath>


namespace esphome::luxtronik_v1
{
    static constexpr const char *const TAG = "luxtronik";

    static constexpr const char* const TYPE_TEMPERATURES          = "1100";
    static constexpr const char* const TYPE_INPUTS                = "1200";
    static constexpr const char* const TYPE_OUTPUTS               = "1300";
    static constexpr const char* const TYPE_OPERATING_HOURS       = "1450";
    static constexpr const char* const TYPE_ERRORS                = "1500";
    static constexpr const char* const TYPE_ERRORS_SLOT           = "1500;150";
    static constexpr const char* const TYPE_DEACTIVATIONS         = "1600";
    static constexpr const char* const TYPE_DEACTIVATIONS_SLOT    = "1600;160";
    static constexpr const char* const TYPE_INFORMATION           = "1700";
    static constexpr const char* const TYPE_ALL                   = "1800";
    static constexpr const char* const TYPE_HEATING_CURVES        = "3400";
    static constexpr const char* const TYPE_HEATING_CURVES_CONFIG = "3401";
    static constexpr const char* const TYPE_HEATING_MODE          = "3405";
    static constexpr const char* const TYPE_HOT_WATER_CONFIG      = "3501";
    static constexpr const char* const TYPE_HOT_WATER_MODE        = "3505";

    static constexpr const char* const TYPE_STORE_CONFIG      = "999";
    static constexpr const char* const TYPE_STORE_CONFIG_ACK  = "993";
    static constexpr const char* const TYPE_STORE_CONFIG_SEQ  = "999993993999";
    static constexpr const char* const TYPE_NACK_778          = "778";
    static constexpr const char* const TYPE_NACK_779          = "779";

    static constexpr const char ASCII_CR      = 0x0D;  // cariage return ('\r')
    static constexpr const char ASCII_LF      = 0x0A;  // line feed ('\n')
    static constexpr const char MIN_PRINTABLE = 0x20;
    static constexpr const char MAX_PRINTABLE = 0x7E;
    static constexpr const char DELIMITER     = ';';

    static constexpr const char SLOT_1_ID = '0';
    static constexpr const char SLOT_2_ID = '1';
    static constexpr const char SLOT_3_ID = '2';
    static constexpr const char SLOT_4_ID = '3';
    static constexpr const char SLOT_5_ID = '4';

    static constexpr const size_t MIN_RESPONSE_LENGTH =  5;
    static constexpr const size_t MIN_SLOT_LENGTH     = 10;

    static constexpr const size_t INDEX_RESPONSE_START =  5;
    static constexpr const size_t INDEX_NACK_START     =  4;
    static constexpr const size_t INDEX_SLOT_ID        =  8;
    static constexpr const size_t INDEX_SLOT_START     = 10;

    static constexpr const uint16_t STORE_SEQ_TIMEOUT = 5000;

    enum class DeviceType : uint8_t
    {
        ERC =  0,
        SW1 =  1,
        SW2 =  2,
        WW1 =  3,
        WW2 =  4,
        L1I =  5,
        L2I =  6,
        L1A =  7,
        L2A =  8,
        KSW =  9,
        KLW = 10,
        SWC = 11,
        LWC = 12,
        L2G = 13,
        WZS = 14
    };

    enum class BivalenceLevel : uint8_t
    {
        SINGLE_COMPRESSOR = 1,
        DUAL_COMPRESSOR   = 2,
        ADDITIONAL_HEATER = 3
    };

    enum class OperationalState : uint8_t
    {
        HEAT          = 0,
        HOT_WATER     = 1,
        SWIMMING_POOL = 2,
        PROVIDER_LOCK = 3,
        DEFROST       = 4,
        STANDBY       = 5
    };

    enum class OperationalMode : uint8_t
    {
        AUTO          = 0,
        SECOND_HEATER = 1,
        PARTY         = 2,
        VACATION      = 3,
        OFF           = 4
    };

    Luxtronik::Luxtronik(
                    uart::UARTComponent* uart,
                    uint16_t request_delay,
                    uint16_t response_timeout,
                    uint16_t max_retries)
        : PollingComponent()
        , m_device(uart)
        , m_sensor_flow_temperature()
        , m_sensor_return_temperature()
        , m_sensor_return_set_temperature()
        , m_sensor_hot_gas_temperature()
        , m_sensor_outside_temperature()
        , m_sensor_hot_water_temperature()
        , m_sensor_hot_water_set_temperature()
        , m_sensor_heat_source_input_temperature()
        , m_sensor_heat_source_output_temperature()
        , m_sensor_mixed_circuit_1_temperature()
        , m_sensor_mixed_circuit_1_set_temperature()
        , m_sensor_remote_adjuster_temperature()
        , m_sensor_defrost_brine_flow()
        , m_sensor_power_provider_lock_period()
        , m_sensor_high_pressure_state()
        , m_sensor_engine_protection()
        , m_sensor_low_pressure_state()
        , m_sensor_external_power()
        , m_sensor_defrost_valve()
        , m_sensor_hot_water_pump()
        , m_sensor_floor_heating_pump()
        , m_sensor_heating_pump()
        , m_sensor_mixer_1_state()
        , m_sensor_housing_ventilation()
        , m_sensor_ventilation_pump()
        , m_sensor_compressor_1()
        , m_sensor_compressor_2()
        , m_sensor_extra_pump()
        , m_sensor_secondary_heater_1()
        , m_sensor_secondary_heater_2_failure()
        , m_sensor_device_type()
        , m_sensor_firmware_version()
        , m_sensor_bivalence_level()
        , m_sensor_operational_state()
        , m_sensor_heating_curve_hc_return_offset()
        , m_sensor_heating_curve_hc_endpoint()
        , m_sensor_heating_curve_hc_parallel_shift()
        , m_sensor_heating_curve_hc_night_setback()
        , m_sensor_heating_curve_hc_constant_return()
        , m_sensor_heating_curve_mc1_endpoint()
        , m_sensor_heating_curve_mc1_parallel_shift()
        , m_sensor_heating_curve_mc1_night_setback()
        , m_sensor_heating_curve_mc1_constant_flow()
        , m_sensor_heating_mode()
        , m_sensor_hot_water_mode()
        , m_sensor_operating_hours_compressor_1()
        , m_sensor_impulses_compressor_1()
        , m_sensor_average_operating_time_compressor_1()
        , m_sensor_operating_hours_compressor_2()
        , m_sensor_impulses_compressor_2()
        , m_sensor_average_operating_time_compressor_2()
        , m_sensor_operating_hours_secondary_heater_1()
        , m_sensor_operating_hours_secondary_heater_2()
        , m_sensor_operating_hours_heat_pump()
        , m_sensor_error_1_code()
        , m_sensor_error_2_code()
        , m_sensor_error_3_code()
        , m_sensor_error_4_code()
        , m_sensor_error_5_code()
        , m_sensor_error_1_time()
        , m_sensor_error_2_time()
        , m_sensor_error_3_time()
        , m_sensor_error_4_time()
        , m_sensor_error_5_time()
        , m_sensor_deactivation_1_code()
        , m_sensor_deactivation_2_code()
        , m_sensor_deactivation_3_code()
        , m_sensor_deactivation_4_code()
        , m_sensor_deactivation_5_code()
        , m_sensor_deactivation_1_time()
        , m_sensor_deactivation_2_time()
        , m_sensor_deactivation_3_time()
        , m_sensor_deactivation_4_time()
        , m_sensor_deactivation_5_time()
        , m_sensor_device_communication()
        , m_request_delay(request_delay)
        , m_response_timeout(response_timeout)
        , m_max_retries(max_retries)
        , m_response_buffer{}
        , m_cursor(0)
        , m_lost_response(false)
        , m_response_ready(false)
        , m_slot_block(false)
        , m_config_response_state(0)
        , m_retry_count(0)
        , m_store_config_ack()
        , m_dataset_list()
        , m_request_queue()
        , m_timer()
    {
    }

    void Luxtronik::update()
    {
        if (m_lost_response)
        {
            ESP_LOGW(TAG, "Lost response(s) from Luxtronik in last update cycle");
        }

        m_sensor_device_communication.set_state(m_lost_response);
        m_lost_response = false;

        // push all configured periodic datasets into the queue
        for (const char* dataset : m_dataset_list)
        {
            m_request_queue.push_back(dataset);
        }

        if (!m_timer.is_running())
        {
            request_data();
        }
    }

    void Luxtronik::loop()
    {
        m_timer.loop();

        if (m_response_ready)
        {
            // we have a full response, so parse it
            parse_response(m_response_buffer);

            m_response_ready = false;
        }
        else
        {
            while (m_device.available())  // read data as long as available
            {
                if (m_cursor == sizeof(m_response_buffer))  // wrap around
                {
                    // if this happens, it's time to increase the response buffer ;)
                    ESP_LOGW(TAG, "Response too long, overwriting start of buffer");

                    m_cursor = 0;
                }

                uint8_t byte;
                bool res = m_device.read_byte(&byte);
                ESP_LOGVV(TAG, "Data read:  CRS %u  DAT %02X  RES %d", m_cursor, byte, res);

                // response is terminated by CRLF, we ignore CR and only react on LF
                if (byte != ASCII_CR)
                {
                    if (byte == ASCII_LF)  // end of response
                    {
                        m_response_buffer[m_cursor] = '\0';
                        m_cursor = 0;

                        // we have a full response, so parse it in next loop
                        m_response_ready = true;

                        break;
                    }
                    else
                    {
                        if ((byte >= MIN_PRINTABLE) && (byte <= MAX_PRINTABLE))
                        {
                            m_response_buffer[m_cursor] = byte;
                        }
                        else
                        {
                            // convert any non-printable characters
                            m_response_buffer[m_cursor] = '.';
                        }

                        ++m_cursor;
                    }
                }
            }
        }
    }

    void Luxtronik::dump_config()
    {
        ESP_LOGCONFIG(TAG, "Luxtronik V1");

#ifdef ESPHOME_LOG_HAS_CONFIG
        std::string datasets = "";

        for (const auto& ds : m_dataset_list)
        {
            if (!datasets.empty())
            {
                datasets += ", ";
            }

            datasets += ds;
        }

        ESP_LOGCONFIG(TAG, "  Data sets: %s", datasets.c_str());
#endif

        ESP_LOGCONFIG(TAG, "  Request delay: %u", m_request_delay);
        ESP_LOGCONFIG(TAG, "  Response timeout: %u", m_response_timeout);
        ESP_LOGCONFIG(TAG, "  Max. number of retries: %u", m_max_retries);

        LOG_TEMPERATURE_SENSOR("Flow Temperture Sensor", m_sensor_flow_temperature);
        LOG_TEMPERATURE_SENSOR("Return Temperture Sensor", m_sensor_return_temperature);
        LOG_TEMPERATURE_SENSOR("Return Set-Temperture Sensor", m_sensor_return_set_temperature);
        LOG_TEMPERATURE_SENSOR("Hot Gas Temperture Sensor", m_sensor_hot_gas_temperature);
        LOG_TEMPERATURE_SENSOR("Hot Water Temperture Sensor", m_sensor_hot_water_temperature);
        LOG_TEMPERATURE_SENSOR("Hot Water Set-Temperture Sensor", m_sensor_hot_water_set_temperature);
        LOG_TEMPERATURE_SENSOR("Outside Temperture Sensor", m_sensor_outside_temperature);
        LOG_TEMPERATURE_SENSOR("Heat-Source Input Temperture Sensor", m_sensor_heat_source_input_temperature);
        LOG_TEMPERATURE_SENSOR("Heat-Source Output Temperture Sensor", m_sensor_heat_source_output_temperature);
        LOG_TEMPERATURE_SENSOR("Mixed Circuit 1 Temperture Sensor", m_sensor_mixed_circuit_1_temperature);
        LOG_TEMPERATURE_SENSOR("Mixed Circuit 1 Set-Temperture Sensor", m_sensor_mixed_circuit_1_set_temperature);
        LOG_TEMPERATURE_SENSOR("Remote Adjuster Temperture Sensor", m_sensor_remote_adjuster_temperature);

        LOG_NUMERIC_SENSOR("Impulses Compressor 1 Sensor", m_sensor_impulses_compressor_1);
        LOG_NUMERIC_SENSOR("Impulses Compressor 2 Sensor", m_sensor_impulses_compressor_2);

        LOG_BOOL_SENSOR("Defrost/Brine/Flow Sensor", m_sensor_defrost_brine_flow);
        LOG_BOOL_SENSOR("Power Provider Lock Period Sensor", m_sensor_power_provider_lock_period);
        LOG_BOOL_SENSOR("Low Pressure State Sensor", m_sensor_low_pressure_state);
        LOG_BOOL_SENSOR("High Pressure State Sensor", m_sensor_high_pressure_state);
        LOG_BOOL_SENSOR("Engine Protection Sensor", m_sensor_engine_protection);
        LOG_BOOL_SENSOR("External Power Sensor", m_sensor_external_power);
        LOG_BOOL_SENSOR("Defrost Valve Sensor", m_sensor_defrost_valve);
        LOG_BOOL_SENSOR("Hot Water Pump Sensor", m_sensor_hot_water_pump);
        LOG_BOOL_SENSOR("Heating Pump Sensor", m_sensor_heating_pump);
        LOG_BOOL_SENSOR("Floor Heating Pump Sensor", m_sensor_floor_heating_pump);
        LOG_BOOL_SENSOR("Ventilation/Pump Sensor", m_sensor_ventilation_pump);
        LOG_BOOL_SENSOR("Housing Ventilation Sensor", m_sensor_housing_ventilation);
        LOG_BOOL_SENSOR("Compressor 1 Sensor", m_sensor_compressor_1);
        LOG_BOOL_SENSOR("Compressor 2 Sensor", m_sensor_compressor_2);
        LOG_BOOL_SENSOR("Extra Pump Sensor", m_sensor_extra_pump);
        LOG_BOOL_SENSOR("Secondary Heater 1 Sensor", m_sensor_secondary_heater_1);
        LOG_BOOL_SENSOR("Secondary Heater 2 Failure Sensor", m_sensor_secondary_heater_2_failure);
        LOG_BOOL_SENSOR("Device Communication Sensor", m_sensor_device_communication);

        LOG_STRING_SENSOR("Device Type Sensor", m_sensor_device_type);
        LOG_STRING_SENSOR("Firmware Version Sensor", m_sensor_firmware_version);
        LOG_STRING_SENSOR("Bivalence Level Sensor", m_sensor_bivalence_level);
        LOG_STRING_SENSOR("Operational State Sensor", m_sensor_operational_state);
        LOG_STRING_SENSOR("Mixer 1 Sensor", m_sensor_mixer_1_state);

        LOG_TEMPERATURE_SENSOR("Heating Curve HC Return Offset Sensor", m_sensor_heating_curve_hc_return_offset);
        LOG_TEMPERATURE_SENSOR("Heating Curve HC Endpoint Sensor", m_sensor_heating_curve_hc_endpoint);
        LOG_TEMPERATURE_SENSOR("Heating Curve HC Parallel Shift Sensor", m_sensor_heating_curve_hc_parallel_shift);
        LOG_TEMPERATURE_SENSOR("Heating Curve HC Night Setback Sensor", m_sensor_heating_curve_hc_night_setback);
        LOG_TEMPERATURE_SENSOR("Heating Curve HC Constant Return Sensor", m_sensor_heating_curve_hc_constant_return);
        LOG_TEMPERATURE_SENSOR("Heating Curve MC1 Endpoint Sensor", m_sensor_heating_curve_mc1_endpoint);
        LOG_TEMPERATURE_SENSOR("Heating Curve MC1 Parallel Shift Sensor", m_sensor_heating_curve_mc1_parallel_shift);
        LOG_TEMPERATURE_SENSOR("Heating Curve MC1 Night Setback Sensor", m_sensor_heating_curve_mc1_night_setback);
        LOG_TEMPERATURE_SENSOR("Heating Curve MC1 Constant Flow Sensor", m_sensor_heating_curve_mc1_constant_flow);

        LOG_STRING_SENSOR("Heating Mode Sensor", m_sensor_heating_mode);
        LOG_STRING_SENSOR("Hot Water Mode Sensor", m_sensor_hot_water_mode);

        LOG_DURATION_SENSOR("Operating Hours Compressor 1 Sensor", m_sensor_operating_hours_compressor_1);
        LOG_DURATION_SENSOR("Average Operating Time Compressor 1 Sensor", m_sensor_average_operating_time_compressor_1);
        LOG_DURATION_SENSOR("Operating Hours Compressor 2 Sensor", m_sensor_operating_hours_compressor_2);
        LOG_DURATION_SENSOR("Average Operating Time Compressor 2 Sensor", m_sensor_average_operating_time_compressor_2);
        LOG_DURATION_SENSOR("Operating Hours Secondary Heater 1 Sensor", m_sensor_operating_hours_secondary_heater_1);
        LOG_DURATION_SENSOR("Operating Hours Secondary Heater 2 Sensor", m_sensor_operating_hours_secondary_heater_2);
        LOG_DURATION_SENSOR("Operating Hours Heat Pump Sensor", m_sensor_operating_hours_heat_pump);

        LOG_STRING_SENSOR("Error 1 Code Sensor", m_sensor_error_1_code);
        LOG_STRING_SENSOR("Error 2 Code Sensor", m_sensor_error_2_code);
        LOG_STRING_SENSOR("Error 3 Code Sensor", m_sensor_error_3_code);
        LOG_STRING_SENSOR("Error 4 Code Sensor", m_sensor_error_4_code);
        LOG_STRING_SENSOR("Error 5 Code Sensor", m_sensor_error_5_code);
        LOG_STRING_SENSOR("Error 1 Time Sensor", m_sensor_error_1_time);
        LOG_STRING_SENSOR("Error 2 Time Sensor", m_sensor_error_2_time);
        LOG_STRING_SENSOR("Error 3 Time Sensor", m_sensor_error_3_time);
        LOG_STRING_SENSOR("Error 4 Time Sensor", m_sensor_error_4_time);
        LOG_STRING_SENSOR("Error 5 Time Sensor", m_sensor_error_5_time);
        LOG_STRING_SENSOR("Deactivation 1 Code Sensor", m_sensor_deactivation_1_code);
        LOG_STRING_SENSOR("Deactivation 2 Code Sensor", m_sensor_deactivation_2_code);
        LOG_STRING_SENSOR("Deactivation 3 Code Sensor", m_sensor_deactivation_3_code);
        LOG_STRING_SENSOR("Deactivation 4 Code Sensor", m_sensor_deactivation_4_code);
        LOG_STRING_SENSOR("Deactivation 5 Code Sensor", m_sensor_deactivation_5_time);
        LOG_STRING_SENSOR("Deactivation 1 Time Sensor", m_sensor_deactivation_1_time);
        LOG_STRING_SENSOR("Deactivation 2 Time Sensor", m_sensor_deactivation_2_time);
        LOG_STRING_SENSOR("Deactivation 3 Time Sensor", m_sensor_deactivation_3_time);
        LOG_STRING_SENSOR("Deactivation 4 Time Sensor", m_sensor_deactivation_4_time);
        LOG_STRING_SENSOR("Deactivation 5 Time Sensor", m_sensor_deactivation_5_code);
    }

    void Luxtronik::add_dataset(const char* code)
    {
        m_dataset_list.push_back(code);
    }

    void Luxtronik::next_dataset()
    {
        // only schedule if there is at least one dataset additionally to the current one
        if (m_request_queue.size() > 1)
        {
            m_timer.schedule(
                        std::chrono::milliseconds(m_request_delay),
                        [this]()
                        {
                            m_request_queue.pop_front();
                            request_data();
                        });
        }
        else
        {
            // remove current dataset as it was the last one
            m_request_queue.pop_front();
        }
    }

    void Luxtronik::request_data()
    {
        if (!m_request_queue.empty())
        {
            send_request(m_request_queue.front());

            m_timer.schedule(
                        std::chrono::milliseconds(m_response_timeout),
                        [this]() { handle_timeout(); });
        }
    }

    void Luxtronik::handle_timeout()
    {
        ESP_LOGW(TAG, "No response from Luxtronik within %u ms:  REQ %s", m_response_timeout, m_request_queue.front().c_str());

        if (++m_retry_count > m_max_retries)
        {
            ESP_LOGW(TAG, "Maximum number of retries reached, skipping data set");

            m_request_queue.pop_front();

            m_retry_count = 0;
            m_lost_response = true;
        }

        request_data();
    }

    void Luxtronik::send_request(const std::string& request)
    {
        ESP_LOGD(TAG, "Sending request:  DATA %s  TRY #%u", request.c_str(), m_retry_count + 1);

        // ensure we start with a clean state
        clear_uart_buffer();
        m_cursor = 0;
        m_response_ready = false;
        m_slot_block = false;

        m_device.write_str(request.c_str());
        m_device.write_byte(ASCII_CR);
        m_device.write_byte(ASCII_LF);
    }

    void Luxtronik::parse_response(const std::string& response)
    {
        if ((response.length() < MIN_RESPONSE_LENGTH) &&
            (response != TYPE_STORE_CONFIG) &&
            (response != TYPE_STORE_CONFIG_ACK))
        {
            return;
        }

        ESP_LOGD(TAG, "Retrieved response:  DATA %s", response.c_str());

        if ((response == TYPE_STORE_CONFIG) || (response == TYPE_STORE_CONFIG_ACK))
        {
            accept_response();
            m_store_config_ack += response;
            m_config_response_state = 0;

            if (m_store_config_ack == TYPE_STORE_CONFIG_SEQ)
            {
                m_store_config_ack = "";

                ESP_LOGD(TAG, "Configuration successfully stored");
                next_dataset();
            }
            else
            {
                // ensure we continue latest 5s after last received response
                m_timer.schedule(
                            std::chrono::milliseconds(STORE_SEQ_TIMEOUT),
                            [this]()
                            {
                                ESP_LOGW(TAG, "No full acknowledge received after storing configuration");

                                m_store_config_ack = "";
                                m_request_queue.pop_front();
                                request_data();
                            });
            }
        }
        else if (starts_with(response, TYPE_TEMPERATURES))
        {
            parse_temperatures(response);
        }
        else if (starts_with(response, TYPE_INPUTS))
        {
            parse_inputs(response);
        }
        else if (starts_with(response, TYPE_OUTPUTS))
        {
            parse_outputs(response);
        }
        else if (starts_with(response, TYPE_OPERATING_HOURS))
        {
            parse_operating_hours(response);
        }
        else if (starts_with(response, TYPE_ERRORS))
        {
            parse_errors(response);
        }
        else if (starts_with(response, TYPE_DEACTIVATIONS))
        {
            parse_deactivations(response);
        }
        else if (starts_with(response, TYPE_INFORMATION))
        {
            parse_information(response);
        }
        else if (starts_with(response, TYPE_HEATING_CURVES))
        {
            parse_heating_curves(response);
        }
        else if (starts_with(response, TYPE_HEATING_CURVES_CONFIG))
        {
            if (m_config_response_state != 1)  // ignore echo
            {
                parse_heating_curves(response);
            }

            ++m_config_response_state;
        }
        else if (starts_with(response, TYPE_HEATING_MODE))
        {
            parse_heating_mode(response);
        }
        else if (starts_with(response, TYPE_HOT_WATER_CONFIG))
        {
            if (m_config_response_state != 1)  // ignore echo
            {
                parse_hot_water_config(response);
            }

            ++m_config_response_state;
        }
        else if (starts_with(response, TYPE_HOT_WATER_MODE))
        {
            parse_hot_water_mode(response);
        }
        else if (starts_with(response, TYPE_NACK_778))
        {
            accept_response();

            size_t start = INDEX_NACK_START;
            size_t end = response.find(DELIMITER, start);
            // skip number of elements

            start = end + 1;
            end = response.find(DELIMITER, start);

            std::string cmd = response.substr(start, end - start);
            ESP_LOGW(TAG, "Request refused:  REQ %s", cmd.c_str());

            next_dataset();
        }
        else if (starts_with(response, TYPE_NACK_779))
        {
            accept_response();

            size_t start = INDEX_NACK_START;
            size_t end = response.find(DELIMITER, start);

            std::string cmd = response.substr(start, end - start);
            ESP_LOGW(TAG, "Request refused during config mode:  REQ %s", cmd.c_str());

            // try again to exit config mode
            m_request_queue.push_front(TYPE_STORE_CONFIG);
            request_data();
        }
    }

    void Luxtronik::parse_temperatures(const std::string& response)
    {
        accept_response();

        size_t start = INDEX_RESPONSE_START;
        size_t end = response.find(DELIMITER, start);
        // skip number of elements

        start = end + 1;
        end = response.find(DELIMITER, start);
        m_sensor_flow_temperature.set_state(response, start, end);

        start = end + 1;
        end = response.find(DELIMITER, start);
        m_sensor_return_temperature.set_state(response, start, end);

        start = end + 1;
        end = response.find(DELIMITER, start);
        m_sensor_return_set_temperature.set_state(response, start, end);

        start = end + 1;
        end = response.find(DELIMITER, start);
        m_sensor_hot_gas_temperature.set_state(response, start, end);

        start = end + 1;
        end = response.find(DELIMITER, start);
        m_sensor_outside_temperature.set_state(response, start, end);

        start = end + 1;
        end = response.find(DELIMITER, start);
        m_sensor_hot_water_temperature.set_state(response, start, end);

        start = end + 1;
        end = response.find(DELIMITER, start);
        m_sensor_hot_water_set_temperature.set_state(response, start, end);

        start = end + 1;
        end = response.find(DELIMITER, start);
        m_sensor_heat_source_input_temperature.set_state(response, start, end);

        start = end + 1;
        end = response.find(DELIMITER, start);
        m_sensor_heat_source_output_temperature.set_state(response, start, end);

        start = end + 1;
        end = response.find(DELIMITER, start);
        m_sensor_mixed_circuit_1_temperature.set_state(response, start, end);

        start = end + 1;
        end = response.find(DELIMITER, start);
        m_sensor_mixed_circuit_1_set_temperature.set_state(response, start, end);

        start = end + 1;
        end = response.find(DELIMITER, start);
        m_sensor_remote_adjuster_temperature.set_state(response, start, end);

        next_dataset();
    }

    void Luxtronik::parse_inputs(const std::string& response)
    {
        accept_response();

        size_t start = INDEX_RESPONSE_START;
        size_t end = response.find(DELIMITER, start);
        // skip number of elements

        start = end + 1;
        end = response.find(DELIMITER, start);
        m_sensor_defrost_brine_flow.set_state(response, start, end);  // on = defrost ending or flow rate ok (depending on device type)

        start = end + 1;
        end = response.find(DELIMITER, start);
        m_sensor_power_provider_lock_period.set_state(response, start, end);  // off = lock period, on = not locked

        start = end + 1;
        end = response.find(DELIMITER, start);
        m_sensor_high_pressure_state.set_state(response, start, end);  // off = pressure ok, on = not ok

        start = end + 1;
        end = response.find(DELIMITER, start);
        m_sensor_engine_protection.set_state(response, start, end, true);  // on = engine protection ok, off = not ok

        start = end + 1;
        end = response.find(DELIMITER, start);
        m_sensor_low_pressure_state.set_state(response, start, end, true);  // on = pressure ok, off = not ok

        start = end + 1;
        end = response.find(DELIMITER, start);
        m_sensor_external_power.set_state(response, start, end);

        next_dataset();
    }

    void Luxtronik::parse_outputs(const std::string& response)
    {
        accept_response();

        size_t start = INDEX_RESPONSE_START;
        size_t end = response.find(DELIMITER, start);
        // skip number of elements

        start = end + 1;
        end = response.find(DELIMITER, start);
        m_sensor_defrost_valve.set_state(response, start, end);

        start = end + 1;
        end = response.find(DELIMITER, start);
        m_sensor_hot_water_pump.set_state(response, start, end);

        start = end + 1;
        end = response.find(DELIMITER, start);
        m_sensor_floor_heating_pump.set_state(response, start, end);

        start = end + 1;
        end = response.find(DELIMITER, start);
        m_sensor_heating_pump.set_state(response, start, end);

        start = end + 1;
        end = response.find(DELIMITER, start);
        bool mixerOpen = get_binary_state(response, start, end);

        start = end + 1;
        end = response.find(DELIMITER, start);
        bool mixerClose = get_binary_state(response, start, end);

        // merge the two separate states (Mischer 1 fährt auf, Mischer 1 fährt zu) into one sensor
        m_sensor_mixer_1_state.set_state(
                mixerOpen
                    ? "opening"
                    : mixerClose
                        ? "closing"
                        : "idle");

        start = end + 1;
        end = response.find(DELIMITER, start);
        m_sensor_housing_ventilation.set_state(response, start, end);

        start = end + 1;
        end = response.find(DELIMITER, start);
        m_sensor_ventilation_pump.set_state(response, start, end);

        start = end + 1;
        end = response.find(DELIMITER, start);
        m_sensor_compressor_1.set_state(response, start, end);

        start = end + 1;
        end = response.find(DELIMITER, start);
        m_sensor_compressor_2.set_state(response, start, end);

        start = end + 1;
        end = response.find(DELIMITER, start);
        m_sensor_extra_pump.set_state(response, start, end);

        start = end + 1;
        end = response.find(DELIMITER, start);
        m_sensor_secondary_heater_1.set_state(response, start, end);

        start = end + 1;
        end = response.find(DELIMITER, start);
        m_sensor_secondary_heater_2_failure.set_state(response, start, end);

        next_dataset();
    }

    void Luxtronik::parse_operating_hours(const std::string& response)
    {
        accept_response();

        size_t start = INDEX_RESPONSE_START;
        size_t end = response.find(DELIMITER, start);
        // skip number of elements

        start = end + 1;
        end = response.find(DELIMITER, start);
        m_sensor_operating_hours_compressor_1.set_state(response, start, end);

        start = end + 1;
        end = response.find(DELIMITER, start);
        m_sensor_impulses_compressor_1.set_state(response, start, end);

        start = end + 1;
        end = response.find(DELIMITER, start);
        m_sensor_average_operating_time_compressor_1.set_state(response, start, end);

        start = end + 1;
        end = response.find(DELIMITER, start);
        m_sensor_operating_hours_compressor_2.set_state(response, start, end);

        start = end + 1;
        end = response.find(DELIMITER, start);
        m_sensor_impulses_compressor_2.set_state(response, start, end);

        start = end + 1;
        end = response.find(DELIMITER, start);
        m_sensor_average_operating_time_compressor_2.set_state(response, start, end);

        start = end + 1;
        end = response.find(DELIMITER, start);
        m_sensor_operating_hours_secondary_heater_1.set_state(response, start, end);

        start = end + 1;
        end = response.find(DELIMITER, start);
        m_sensor_operating_hours_secondary_heater_2.set_state(response, start, end);

        start = end + 1;
        end = response.find(DELIMITER, start);
        m_sensor_operating_hours_heat_pump.set_state(response, start, end);

        next_dataset();
    }

    void Luxtronik::parse_errors(const std::string& response)
    {
        accept_response();

        if ((response.length() >= MIN_SLOT_LENGTH) && starts_with(response, TYPE_ERRORS_SLOT))
        {
            char slotID = response.at(INDEX_SLOT_ID);
            switch (slotID)
            {
                case SLOT_1_ID: { parse_slot(response.substr(INDEX_SLOT_START), m_sensor_error_1_code, m_sensor_error_1_time); break; }
                case SLOT_2_ID: { parse_slot(response.substr(INDEX_SLOT_START), m_sensor_error_2_code, m_sensor_error_2_time); break; }
                case SLOT_3_ID: { parse_slot(response.substr(INDEX_SLOT_START), m_sensor_error_3_code, m_sensor_error_3_time); break; }
                case SLOT_4_ID: { parse_slot(response.substr(INDEX_SLOT_START), m_sensor_error_4_code, m_sensor_error_4_time); break; }
                case SLOT_5_ID: { parse_slot(response.substr(INDEX_SLOT_START), m_sensor_error_5_code, m_sensor_error_5_time); break; }
            }
        }
        else if (!m_slot_block)
        {
            m_slot_block = true;
        }
        else
        {
            m_slot_block = false;
            next_dataset();
        }
    }

    void Luxtronik::parse_deactivations(const std::string& response)
    {
        accept_response();

        if ((response.length() >= MIN_SLOT_LENGTH) && starts_with(response, TYPE_DEACTIVATIONS_SLOT))
        {
            char slotID = response.at(INDEX_SLOT_ID);
            switch (slotID)
            {
                case SLOT_1_ID: { parse_slot(response.substr(INDEX_SLOT_START), m_sensor_deactivation_1_code, m_sensor_deactivation_1_time); break; }
                case SLOT_2_ID: { parse_slot(response.substr(INDEX_SLOT_START), m_sensor_deactivation_2_code, m_sensor_deactivation_2_time); break; }
                case SLOT_3_ID: { parse_slot(response.substr(INDEX_SLOT_START), m_sensor_deactivation_3_code, m_sensor_deactivation_3_time); break; }
                case SLOT_4_ID: { parse_slot(response.substr(INDEX_SLOT_START), m_sensor_deactivation_4_code, m_sensor_deactivation_4_time); break; }
                case SLOT_5_ID: { parse_slot(response.substr(INDEX_SLOT_START), m_sensor_deactivation_5_code, m_sensor_deactivation_5_time); break; }
            }
        }
        else if (!m_slot_block)
        {
            m_slot_block = true;
        }
        else
        {
            m_slot_block = false;
            next_dataset();
        }
    }

    void Luxtronik::parse_information(const std::string& response)
    {
        accept_response();

        size_t start = INDEX_RESPONSE_START;
        size_t end = response.find(DELIMITER, start);
        // skip number of elements

        start = end + 1;
        end = response.find(DELIMITER, start);

        if (m_sensor_device_type.has_sensor())
        {
            std::string state = "";
            int32_t value = get_number(response, start, end);

            switch (static_cast<DeviceType>(value))
            {
                case DeviceType::ERC: { state = "ERC";                 break; }
                case DeviceType::SW1: { state = "SW1";                 break; }
                case DeviceType::SW2: { state = "SW2";                 break; }
                case DeviceType::WW1: { state = "WW1";                 break; }
                case DeviceType::WW2: { state = "WW2";                 break; }
                case DeviceType::L1I: { state = "L1I";                 break; }
                case DeviceType::L2I: { state = "L2I";                 break; }
                case DeviceType::L1A: { state = "L1A";                 break; }
                case DeviceType::L2A: { state = "L2A";                 break; }
                case DeviceType::KSW: { state = "KSW";                 break; }
                case DeviceType::KLW: { state = "KLW";                 break; }
                case DeviceType::SWC: { state = "SWC";                 break; }
                case DeviceType::LWC: { state = "LWC";                 break; }
                case DeviceType::L2G: { state = "L2G";                 break; }
                case DeviceType::WZS: { state = "WZS";                 break; }
                default:              { state = std::to_string(value); break; }
            }

            m_sensor_device_type.set_state(state);
        }

        start = end + 1;
        end = response.find(DELIMITER, start);
        m_sensor_firmware_version.set_state(response, start + 1, end);  // skip leading space

        start = end + 1;
        end = response.find(DELIMITER, start);

        if (m_sensor_bivalence_level.has_sensor())
        {
            std::string state = "";
            int32_t value = get_number(response, start, end);

            switch (static_cast<BivalenceLevel>(value))
            {
                case BivalenceLevel::SINGLE_COMPRESSOR: { state = "single_compressor";   break; }
                case BivalenceLevel::DUAL_COMPRESSOR:   { state = "dual_compressor";     break; }
                case BivalenceLevel::ADDITIONAL_HEATER: { state = "additional_heater";   break; }
                default:                                { state = std::to_string(value); break; }
            }

            m_sensor_bivalence_level.set_state(state);
        }

        start = end + 1;
        end = response.find(DELIMITER, start);

        if (m_sensor_operational_state.has_sensor())
        {
            std::string state = "";
            int32_t value = get_number(response, start, end);

            switch (static_cast<OperationalState>(value))
            {
                case OperationalState::HEAT:          { state = "heat";                break; }
                case OperationalState::HOT_WATER:     { state = "hot_water";           break; }
                case OperationalState::SWIMMING_POOL: { state = "swimming_pool";       break; }
                case OperationalState::PROVIDER_LOCK: { state = "provider_lock";       break; }
                case OperationalState::DEFROST:       { state = "defrost";             break; }
                case OperationalState::STANDBY:       { state = "standby";             break; }
                default:                              { state = std::to_string(value); break; }
            }

            m_sensor_operational_state.set_state(state);
        }

        next_dataset();
    }

    void Luxtronik::parse_heating_curves(const std::string& response)
    {
        accept_response();

        size_t start = INDEX_RESPONSE_START;
        size_t end = response.find(DELIMITER, start);
        // skip number of elements

        start = end + 1;
        end = response.find(DELIMITER, start);
        m_sensor_heating_curve_hc_return_offset.set_state(response, start, end);

        start = end + 1;
        end = response.find(DELIMITER, start);
        m_sensor_heating_curve_hc_endpoint.set_state(response, start, end);

        start = end + 1;
        end = response.find(DELIMITER, start);
        m_sensor_heating_curve_hc_parallel_shift.set_state(response, start, end);

        start = end + 1;
        end = response.find(DELIMITER, start);
        m_sensor_heating_curve_hc_night_setback.set_state(response, start, end);

        start = end + 1;
        end = response.find(DELIMITER, start);
        m_sensor_heating_curve_hc_constant_return.set_state(response, start, end);

        start = end + 1;
        end = response.find(DELIMITER, start);
        m_sensor_heating_curve_mc1_endpoint.set_state(response, start, end);

        start = end + 1;
        end = response.find(DELIMITER, start);
        m_sensor_heating_curve_mc1_parallel_shift.set_state(response, start, end);

        start = end + 1;
        end = response.find(DELIMITER, start);
        m_sensor_heating_curve_mc1_night_setback.set_state(response, start, end);

        start = end + 1;
        end = response.find(DELIMITER, start);
        m_sensor_heating_curve_mc1_constant_flow.set_state(response, start, end);

        next_dataset();
    }

    void Luxtronik::parse_heating_mode(const std::string& response)
    {
        accept_response();

        size_t start = INDEX_RESPONSE_START;
        size_t end = response.find(DELIMITER, start);
        // skip number of elements

        start = end + 1;
        end = response.find(DELIMITER, start);

        if (m_sensor_heating_mode.has_sensor())
        {
            std::string state = "";
            int32_t value = get_number(response, start, end);

            switch (static_cast<OperationalMode>(value))
            {
                case OperationalMode::AUTO:          { state = "auto";                break; }
                case OperationalMode::SECOND_HEATER: { state = "second_heater";       break; }
                case OperationalMode::PARTY:         { state = "party";               break; }
                case OperationalMode::VACATION:      { state = "vacation";            break; }
                case OperationalMode::OFF:           { state = "off";                 break; }
                default:                             { state = std::to_string(value); break; }
            }

            m_sensor_heating_mode.set_state(state);
        }

        next_dataset();
    }

    void Luxtronik::parse_hot_water_config(const std::string& response)
    {
        accept_response();

        size_t start = INDEX_RESPONSE_START;
        size_t end = response.find(DELIMITER, start);
        // skip number of elements

        start = end + 1;
        end = response.find(DELIMITER, start);
        m_sensor_hot_water_set_temperature.set_state(response, start, end);

        next_dataset();
    }

    void Luxtronik::parse_hot_water_mode(const std::string& response)
    {
        accept_response();

        size_t start = INDEX_RESPONSE_START;
        size_t end = response.find(DELIMITER, start);
        // skip number of elements

        start = end + 1;
        end = response.find(DELIMITER, start);

        if (m_sensor_hot_water_mode.has_sensor())
        {
            std::string state = "";
            int32_t value = get_number(response, start, end);

            switch (static_cast<OperationalMode>(value))
            {
                case OperationalMode::AUTO:          { state = "auto";                break; }
                case OperationalMode::SECOND_HEATER: { state = "second_heater";       break; }
                case OperationalMode::PARTY:         { state = "party";               break; }
                case OperationalMode::VACATION:      { state = "vacation";            break; }
                case OperationalMode::OFF:           { state = "off";                 break; }
                default:                             { state = std::to_string(value); break; }
            }

            m_sensor_hot_water_mode.set_state(state);
        }

        next_dataset();
    }


    void Luxtronik::parse_slot(const std::string& slot, StringSensor& sensor_code, StringSensor& sensor_time)
    {
        size_t start = 0;
        size_t end = slot.find(DELIMITER, start);
        // skip number of elements

        start = end + 1;
        end = slot.find(DELIMITER, start);
        sensor_code.set_state(slot, start, end);

        if (sensor_time.has_sensor())
        {
            std::tm time = {};

            start = end + 1;
            end = slot.find(DELIMITER, start);
            time.tm_mday = get_number(slot, start, end);

            start = end + 1;
            end = slot.find(DELIMITER, start);
            time.tm_mon = get_number(slot, start, end) - 1;  // convert from range 1-12 to range 0-11

            start = end + 1;
            end = slot.find(DELIMITER, start);
            time.tm_year = get_number(slot, start, end) + 100;  // convert from years since 2000 to years since 1900

            start = end + 1;
            end = slot.find(DELIMITER, start);
            time.tm_hour = get_number(slot, start, end);

            start = end + 1;
            end = slot.find(DELIMITER, start);
            time.tm_min = get_number(slot, start, end);

            time.tm_isdst = -1;  // auto-detect daylight saving time
            std::mktime(&time);  // convert to universal time

            char state[] = "yyyy-mm-ddThh:mm+zzzz";
            std::strftime(state, sizeof(state), "%Y-%m-%dT%H:%M%z", &time);

            sensor_time.set_state(state);
        }
    }

    void Luxtronik::set_hot_water_set_temperature(float value)
    {
        if ((value >= 0) && (value < 100))
        {
            ESP_LOGD(TAG, "Queuing requests for setting hot water set temperature:  VAL %.1f °C", value);

            m_request_queue.push_back(TYPE_HOT_WATER_CONFIG);
            m_request_queue.push_back(std::string(TYPE_HOT_WATER_CONFIG) + ";1;" + std::to_string(static_cast<uint32_t>(std::floor(value * 10))));
            m_request_queue.push_back(TYPE_STORE_CONFIG);

            if (!m_timer.is_running())
            {
                request_data();
            }
        }
        else
        {
            ESP_LOGW(TAG, "Invalid value for hot water set temperature:  VAL %.1f °C", value);
        }
    }

    void Luxtronik::set_heating_curves(HeatingCurves& value)
    {
        if (!value.hc_return_offset_avail)   { value.hc_return_offset   = m_sensor_heating_curve_hc_return_offset.get_state();   }
        if (!value.hc_endpoint_avail)        { value.hc_endpoint        = m_sensor_heating_curve_hc_endpoint.get_state();        }
        if (!value.hc_parallel_shift_avail)  { value.hc_parallel_shift  = m_sensor_heating_curve_hc_parallel_shift.get_state();  }
        if (!value.hc_night_setback_avail)   { value.hc_night_setback   = m_sensor_heating_curve_hc_night_setback.get_state();   }
        if (!value.hc_const_return_avail)    { value.hc_const_return    = m_sensor_heating_curve_hc_constant_return.get_state(); }
        if (!value.mc1_endpoint_avail)       { value.mc1_endpoint       = m_sensor_heating_curve_mc1_endpoint.get_state();       }
        if (!value.mc1_parallel_shift_avail) { value.mc1_parallel_shift = m_sensor_heating_curve_mc1_parallel_shift.get_state(); }
        if (!value.mc1_night_setback_avail)  { value.mc1_night_setback  = m_sensor_heating_curve_mc1_night_setback.get_state();  }
        if (!value.mc1_const_flow_avail)     { value.mc1_const_flow     = m_sensor_heating_curve_mc1_constant_flow.get_state();  }

        ESP_LOGD(
            TAG,
            "Queuing requests for setting heating curves:  HOF %.1f °C  HEP %.1f °C  HPS %.1f °C  HNS %.1f °C  HCR %.1f °C  MEP %.1f °C  MPS %.1f °C  MNS %.1f °C  MCF %.1f °C",
            value.hc_return_offset,
            value.hc_endpoint, value.hc_parallel_shift, value.hc_night_setback, value.hc_const_return,
            value.mc1_endpoint, value.mc1_parallel_shift, value.mc1_night_setback, value.mc1_const_flow);

        m_request_queue.push_back(TYPE_HEATING_CURVES_CONFIG);
        m_request_queue.push_back(
                            std::string(TYPE_HEATING_CURVES_CONFIG) + ";9;" +
                            std::to_string(static_cast<int32_t>(std::floor(value.hc_return_offset * 10))) + ";" +
                            std::to_string(static_cast<int32_t>(std::floor(value.hc_endpoint * 10))) + ";" +
                            std::to_string(static_cast<int32_t>(std::floor(value.hc_parallel_shift * 10))) + ";" +
                            std::to_string(static_cast<int32_t>(std::floor(value.hc_night_setback * 10))) + ";" +
                            std::to_string(static_cast<int32_t>(std::floor(value.hc_const_return * 10))) + ";" +
                            std::to_string(static_cast<int32_t>(std::floor(value.mc1_endpoint * 10))) + ";" +
                            std::to_string(static_cast<int32_t>(std::floor(value.mc1_parallel_shift * 10))) + ";" +
                            std::to_string(static_cast<int32_t>(std::floor(value.mc1_night_setback * 10))) + ";" +
                            std::to_string(static_cast<int32_t>(std::floor(value.mc1_const_flow * 10))));
        m_request_queue.push_back(TYPE_STORE_CONFIG);

        if (!m_timer.is_running())
        {
            request_data();
        }
    }

    void Luxtronik::clear_uart_buffer()
    {
        uint8_t byte;

        while (m_device.available())
        {
            m_device.read_byte(&byte);
        }
    }
}  // namespace esphome::luxtronik_v1
