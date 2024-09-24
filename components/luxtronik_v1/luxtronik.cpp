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


#include "luxtronik.h"
#include "esphome/core/log.h"

#include <chrono>
#include <ctime>


namespace esphome::luxtronik_v1
{
    static constexpr const char *const TAG = "luxtronik";

    static constexpr const char* const TYPE_TEMPERATURES       = "1100";
    static constexpr const char* const TYPE_INPUTS             = "1200";
    static constexpr const char* const TYPE_OUTPUTS            = "1300";
    static constexpr const char* const TYPE_ERRORS             = "1500";
    static constexpr const char* const TYPE_ERRORS_SLOT        = "150";
    static constexpr const char* const TYPE_DEACTIVATIONS      = "1600";
    static constexpr const char* const TYPE_DEACTIVATIONS_SLOT = "160";
    static constexpr const char* const TYPE_INFORMATION        = "1700";
    static constexpr const char* const TYPE_ALL                = "1800";
    static constexpr const char* const TYPE_HEATING_MODE       = "3405";
    static constexpr const char* const TYPE_HOT_WATER_MODE     = "3505";

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

    static constexpr const size_t INDEX_RESPONSE_START = 5;
    static constexpr const size_t INDEX_SLOT_ID        = 8;

    static constexpr const uint16_t RESPONSE_TEMPERATURES   = 1 << 0;
    static constexpr const uint16_t RESPONSE_INPUTS         = 1 << 1;
    static constexpr const uint16_t RESPONSE_OUTPUTS        = 1 << 2;
    static constexpr const uint16_t RESPONSE_ERRORS         = 1 << 3;
    static constexpr const uint16_t RESPONSE_DEACTIVATIONS  = 1 << 4;
    static constexpr const uint16_t RESPONSE_INFORMATION    = 1 << 5;
    static constexpr const uint16_t RESPONSE_HEATING_MODE   = 1 << 6;
    static constexpr const uint16_t RESPONSE_HOT_WATER_MODE = 1 << 7;

    static constexpr const uint16_t RESPONSE_NONE = 0;
    static constexpr const uint16_t RESPONSE_ALL  = RESPONSE_TEMPERATURES|
                                                    RESPONSE_INPUTS|
                                                    RESPONSE_OUTPUTS|
                                                    RESPONSE_ERRORS|
                                                    RESPONSE_DEACTIVATIONS|
                                                    RESPONSE_INFORMATION|
                                                    RESPONSE_HEATING_MODE|
                                                    RESPONSE_HOT_WATER_MODE;

    enum class DeviceType
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

    enum class BivalenceLevel
    {
        SINGLE_COMPRESSOR = 1,
        DUAL_COMPRESSOR   = 2,
        ADDITIONAL_HEATER = 3
    };

    enum class OperationalState
    {
        HEAT          = 0,
        HOT_WATER     = 1,
        SWIMMING_POOL = 2,
        PROVIDER_LOCK = 3,
        DEFROST       = 4,
        STANDBY       = 5
    };

    enum class OperationalMode
    {
        AUTO          = 0,
        SECOND_HEATER = 1,
        PARTY         = 2,
        VACATION      = 3,
        OFF           = 4
    };

    Luxtronik::Luxtronik(uart::UARTComponent* uart)
        : PollingComponent()
        , m_device(uart)
#ifdef USE_SENSOR
        , m_sensor_flow_temperature(nullptr)
        , m_sensor_return_temperature(nullptr)
        , m_sensor_return_set_temperature(nullptr)
        , m_sensor_hot_gas_temperature(nullptr)
        , m_sensor_outside_temperature(nullptr)
        , m_sensor_hot_water_temperature(nullptr)
        , m_sensor_hot_water_set_temperature(nullptr)
        , m_sensor_heat_source_input_temperature(nullptr)
        , m_sensor_heat_source_output_temperature(nullptr)
        , m_sensor_mixed_circuit_1_temperature(nullptr)
        , m_sensor_mixed_circuit_1_set_temperature(nullptr)
        , m_sensor_remote_adjuster_temperature(nullptr)
#endif
#ifdef USE_BINARY_SENSOR
        , m_sensor_defrost_brine_flow(nullptr)
        , m_sensor_power_provider_lock_period(nullptr)
        , m_sensor_high_pressure_state(nullptr)
        , m_sensor_engine_protection(nullptr)
        , m_sensor_low_pressure_state(nullptr)
        , m_sensor_external_power(nullptr)
        , m_sensor_defrost_valve(nullptr)
        , m_sensor_hot_water_pump(nullptr)
        , m_sensor_floor_heating_pump(nullptr)
        , m_sensor_heating_pump(nullptr)
        , m_sensor_housing_ventilation(nullptr)
        , m_sensor_ventilation_pump(nullptr)
        , m_sensor_compressor_1(nullptr)
        , m_sensor_compressor_2(nullptr)
        , m_sensor_extra_pump(nullptr)
        , m_sensor_secondary_heater_1(nullptr)
        , m_sensor_secondary_heater_2_failure(nullptr)
        , m_sensor_device_communication(nullptr)
#endif
#ifdef USE_TEXT_SENSOR
        , m_sensor_mixer_1_state(nullptr)
        , m_sensor_device_type(nullptr)
        , m_sensor_firmware_version(nullptr)
        , m_sensor_bivalence_level(nullptr)
        , m_sensor_operational_state(nullptr)
        , m_sensor_heating_mode(nullptr)
        , m_sensor_hot_water_mode(nullptr)
        , m_sensor_error_1_code(nullptr)
        , m_sensor_error_2_code(nullptr)
        , m_sensor_error_3_code(nullptr)
        , m_sensor_error_4_code(nullptr)
        , m_sensor_error_5_code(nullptr)
        , m_sensor_error_1_time(nullptr)
        , m_sensor_error_2_time(nullptr)
        , m_sensor_error_3_time(nullptr)
        , m_sensor_error_4_time(nullptr)
        , m_sensor_error_5_time(nullptr)
        , m_sensor_deactivation_1_code(nullptr)
        , m_sensor_deactivation_2_code(nullptr)
        , m_sensor_deactivation_3_code(nullptr)
        , m_sensor_deactivation_4_code(nullptr)
        , m_sensor_deactivation_5_code(nullptr)
        , m_sensor_deactivation_1_time(nullptr)
        , m_sensor_deactivation_2_time(nullptr)
        , m_sensor_deactivation_3_time(nullptr)
        , m_sensor_deactivation_4_time(nullptr)
        , m_sensor_deactivation_5_time(nullptr)
#endif
        , m_response_buffer{}
        , m_cursor(0)
        , m_received_responses(RESPONSE_ALL)
        , m_response_ready(false)
        , m_slot_block(false)
    {
    }

    void Luxtronik::loop()
    {
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

    void Luxtronik::update()
    {
        if (m_received_responses != RESPONSE_ALL)
        {
            ESP_LOGW(TAG, "No full response from Luxtronik in last update cycle:  RESP %04X", m_received_responses);
        }

#ifdef USE_BINARY_SENSOR
        if (m_sensor_device_communication != nullptr)
        {
            m_sensor_device_communication->publish_state(m_received_responses != RESPONSE_ALL);
        }
#endif

        m_received_responses = RESPONSE_NONE;
        m_slot_block = false;

        // reset cursor in case last response was not complete
        m_cursor = 0;

        // request temperatures
        send_request(TYPE_TEMPERATURES);
    }

    void Luxtronik::dump_config()
    {
        ESP_LOGCONFIG(TAG, "Luxtronik V1");

#ifdef USE_SENSOR
        LOG_SENSOR("", "Flow Temperture Sensor", m_sensor_flow_temperature);
        LOG_SENSOR("", "Return Temperture Sensor", m_sensor_return_temperature);
        LOG_SENSOR("", "Return Set-Temperture Sensor", m_sensor_return_set_temperature);
        LOG_SENSOR("", "Hot Gas Temperture Sensor", m_sensor_hot_gas_temperature);
        LOG_SENSOR("", "Hot Water Temperture Sensor", m_sensor_hot_water_temperature);
        LOG_SENSOR("", "Hot Water Set-Temperture Sensor", m_sensor_hot_water_set_temperature);
        LOG_SENSOR("", "Outside Temperture Sensor", m_sensor_outside_temperature);
        LOG_SENSOR("", "Heat-Source Input Temperture Sensor", m_sensor_heat_source_input_temperature);
        LOG_SENSOR("", "Heat-Source Output Temperture Sensor", m_sensor_heat_source_output_temperature);
        LOG_SENSOR("", "Mixed Circuit 1 Temperture Sensor", m_sensor_mixed_circuit_1_temperature);
        LOG_SENSOR("", "Mixed Circuit 1 Set-Temperture Sensor", m_sensor_mixed_circuit_1_set_temperature);
        LOG_SENSOR("", "Remote Adjuster Temperture Sensor", m_sensor_remote_adjuster_temperature);
#endif

#ifdef USE_BINARY_SENSOR
        LOG_BINARY_SENSOR("", "Defrost/Brine/Flow Sensor", m_sensor_defrost_brine_flow);
        LOG_BINARY_SENSOR("", "Power Provider Lock Period Sensor", m_sensor_power_provider_lock_period);
        LOG_BINARY_SENSOR("", "Low Pressure State Sensor", m_sensor_low_pressure_state);
        LOG_BINARY_SENSOR("", "High Pressure State Sensor", m_sensor_high_pressure_state);
        LOG_BINARY_SENSOR("", "Engine Protection Sensor", m_sensor_engine_protection);
        LOG_BINARY_SENSOR("", "External Power Sensor", m_sensor_external_power);
        LOG_BINARY_SENSOR("", "Defrost Valve Sensor", m_sensor_defrost_valve);
        LOG_BINARY_SENSOR("", "Hot Water Pump Sensor", m_sensor_hot_water_pump);
        LOG_BINARY_SENSOR("", "Heating Pump Sensor", m_sensor_heating_pump);
        LOG_BINARY_SENSOR("", "Floor Heating Pump Sensor", m_sensor_floor_heating_pump);
        LOG_BINARY_SENSOR("", "Ventilation/Pump Sensor", m_sensor_ventilation_pump);
        LOG_BINARY_SENSOR("", "Housing Ventilation Sensor", m_sensor_housing_ventilation);
        LOG_BINARY_SENSOR("", "Compressor 1 Sensor", m_sensor_compressor_1);
        LOG_BINARY_SENSOR("", "Compressor 2 Sensor", m_sensor_compressor_2);
        LOG_BINARY_SENSOR("", "Extra Pump Sensor", m_sensor_extra_pump);
        LOG_BINARY_SENSOR("", "Secondary Heater 1 Sensor", m_sensor_secondary_heater_1);
        LOG_BINARY_SENSOR("", "Secondary Heater 2 Failure Sensor", m_sensor_secondary_heater_2_failure);
        LOG_BINARY_SENSOR("", "Device Communication Sensor", m_sensor_device_communication);
#endif

#ifdef USE_TEXT_SENSOR
        LOG_TEXT_SENSOR("", "Mixer 1 Sensor", m_sensor_mixer_1_state);
        LOG_TEXT_SENSOR("", "Device Type Sensor", m_sensor_device_type);
        LOG_TEXT_SENSOR("", "Firmware Version Sensor", m_sensor_firmware_version);
        LOG_TEXT_SENSOR("", "Bivalence Level Sensor", m_sensor_bivalence_level);
        LOG_TEXT_SENSOR("", "Operational State Sensor", m_sensor_operational_state);
        LOG_TEXT_SENSOR("", "Error 1 Code Sensor", m_sensor_error_1_code);
        LOG_TEXT_SENSOR("", "Error 2 Code Sensor", m_sensor_error_2_code);
        LOG_TEXT_SENSOR("", "Error 3 Code Sensor", m_sensor_error_3_code);
        LOG_TEXT_SENSOR("", "Error 4 Code Sensor", m_sensor_error_4_code);
        LOG_TEXT_SENSOR("", "Error 5 Code Sensor", m_sensor_error_5_code);
        LOG_TEXT_SENSOR("", "Error 1 Time Sensor", m_sensor_error_1_time);
        LOG_TEXT_SENSOR("", "Error 2 Time Sensor", m_sensor_error_2_time);
        LOG_TEXT_SENSOR("", "Error 3 Time Sensor", m_sensor_error_3_time);
        LOG_TEXT_SENSOR("", "Error 4 Time Sensor", m_sensor_error_4_time);
        LOG_TEXT_SENSOR("", "Error 5 Time Sensor", m_sensor_error_5_time);
        LOG_TEXT_SENSOR("", "Deactivation 1 Code Sensor", m_sensor_deactivation_1_code);
        LOG_TEXT_SENSOR("", "Deactivation 2 Code Sensor", m_sensor_deactivation_2_code);
        LOG_TEXT_SENSOR("", "Deactivation 3 Code Sensor", m_sensor_deactivation_3_code);
        LOG_TEXT_SENSOR("", "Deactivation 4 Code Sensor", m_sensor_deactivation_4_code);
        LOG_TEXT_SENSOR("", "Deactivation 5 Code Sensor", m_sensor_deactivation_5_time);
        LOG_TEXT_SENSOR("", "Deactivation 1 Time Sensor", m_sensor_deactivation_1_time);
        LOG_TEXT_SENSOR("", "Deactivation 2 Time Sensor", m_sensor_deactivation_2_time);
        LOG_TEXT_SENSOR("", "Deactivation 3 Time Sensor", m_sensor_deactivation_3_time);
        LOG_TEXT_SENSOR("", "Deactivation 4 Time Sensor", m_sensor_deactivation_4_time);
        LOG_TEXT_SENSOR("", "Deactivation 5 Time Sensor", m_sensor_deactivation_5_code);
#endif
    }

    void Luxtronik::send_request(const char* request)
    {
        ESP_LOGD(TAG, "Sending request:  DATA %s", request);

        m_device.write_str(request);
        m_device.write_byte(ASCII_CR);
        m_device.write_byte(ASCII_LF);
    }

    void Luxtronik::parse_response(const std::string& response)
    {
        if (response.length() < MIN_RESPONSE_LENGTH)
        {
            return;
        }

        ESP_LOGD(TAG, "Retrieved response:  DATA %s", response.c_str());

        if (starts_with(response, TYPE_TEMPERATURES))
        {
            size_t start = INDEX_RESPONSE_START;
            size_t end = response.find(DELIMITER, start);
            // skip number of elements

            start = end + 1;
            end = response.find(DELIMITER, start);
#ifdef USE_SENSOR
            if (m_sensor_flow_temperature != nullptr)
            {
                m_sensor_flow_temperature->
                    publish_state(
                        get_temperature(
                            response.substr(start, end - start)));
            }
#endif

            start = end + 1;
            end = response.find(DELIMITER, start);
#ifdef USE_SENSOR
            if (m_sensor_return_temperature != nullptr)
            {
                m_sensor_return_temperature->
                    publish_state(
                        get_temperature(
                            response.substr(start, end - start)));
            }
#endif

            start = end + 1;
            end = response.find(DELIMITER, start);
#ifdef USE_SENSOR
            if (m_sensor_return_set_temperature != nullptr)
            {
                m_sensor_return_set_temperature->
                    publish_state(
                        get_temperature(
                            response.substr(start, end - start)));
            }
#endif

            start = end + 1;
            end = response.find(DELIMITER, start);
#ifdef USE_SENSOR
            if (m_sensor_hot_gas_temperature != nullptr)
            {
                m_sensor_hot_gas_temperature->
                    publish_state(
                        get_temperature(
                            response.substr(start, end - start)));
            }
#endif

            start = end + 1;
            end = response.find(DELIMITER, start);
#ifdef USE_SENSOR
            if (m_sensor_outside_temperature != nullptr)
            {
                m_sensor_outside_temperature->
                    publish_state(
                        get_temperature(
                            response.substr(start, end - start)));
            }
#endif

            start = end + 1;
            end = response.find(DELIMITER, start);
#ifdef USE_SENSOR
            if (m_sensor_hot_water_temperature != nullptr)
            {
                m_sensor_hot_water_temperature->
                    publish_state(
                        get_temperature(
                            response.substr(start, end - start)));
            }
#endif

            start = end + 1;
            end = response.find(DELIMITER, start);
#ifdef USE_SENSOR
            if (m_sensor_hot_water_set_temperature != nullptr)
            {
                m_sensor_hot_water_set_temperature->
                    publish_state(
                        get_temperature(
                            response.substr(start, end - start)));
            }
#endif

            start = end + 1;
            end = response.find(DELIMITER, start);
#ifdef USE_SENSOR
            if (m_sensor_heat_source_input_temperature != nullptr)
            {
                m_sensor_heat_source_input_temperature->
                    publish_state(
                        get_temperature(
                            response.substr(start, end - start)));
            }
#endif

            start = end + 1;
            end = response.find(DELIMITER, start);
#ifdef USE_SENSOR
            if (m_sensor_heat_source_output_temperature != nullptr)
            {
                m_sensor_heat_source_output_temperature->
                    publish_state(
                        get_temperature(
                            response.substr(start, end - start)));
            }
#endif

            start = end + 1;
            end = response.find(DELIMITER, start);
#ifdef USE_SENSOR
            if (m_sensor_mixed_circuit_1_temperature != nullptr)
            {
                m_sensor_mixed_circuit_1_temperature->
                    publish_state(
                        get_temperature(
                            response.substr(start, end - start)));
            }
#endif

            start = end + 1;
            end = response.find(DELIMITER, start);
#ifdef USE_SENSOR
            if (m_sensor_mixed_circuit_1_set_temperature != nullptr)
            {
                m_sensor_mixed_circuit_1_set_temperature->
                    publish_state(
                        get_temperature(
                            response.substr(start, end - start)));
            }
#endif

            start = end + 1;
            end = response.find(DELIMITER, start);
#ifdef USE_SENSOR
            if (m_sensor_remote_adjuster_temperature != nullptr)
            {
                m_sensor_remote_adjuster_temperature->
                    publish_state(
                        get_temperature(
                            response.substr(start, end - start)));
            }
#endif

            m_received_responses |= RESPONSE_TEMPERATURES;

            // request inputs
            send_request(TYPE_INPUTS);
        }
        else if (starts_with(response, TYPE_INPUTS))
        {
            size_t start = INDEX_RESPONSE_START;
            size_t end = response.find(DELIMITER, start);
            // skip number of elements

            start = end + 1;
            end = response.find(DELIMITER, start);
#ifdef USE_BINARY_SENSOR
            if (m_sensor_defrost_brine_flow != nullptr)
            {
                m_sensor_defrost_brine_flow->
                    publish_state(
                        get_binary_state(  // on = defrost ending or flow rate ok (depending on device type)
                            response.substr(start, end - start)));
            }
#endif

            start = end + 1;
            end = response.find(DELIMITER, start);
#ifdef USE_BINARY_SENSOR
            if (m_sensor_power_provider_lock_period != nullptr)
            {
                m_sensor_power_provider_lock_period->
                    publish_state(
                        get_binary_state(  // off = lock period, on = not locked
                            response.substr(start, end - start)));
            }
#endif

            start = end + 1;
            end = response.find(DELIMITER, start);
#ifdef USE_BINARY_SENSOR
            if (m_sensor_high_pressure_state != nullptr)
            {
                m_sensor_high_pressure_state->
                    publish_state(
                        get_binary_state(  // off = pressure ok, on = not ok
                            response.substr(start, end - start)));
            }
#endif

            start = end + 1;
            end = response.find(DELIMITER, start);
#ifdef USE_BINARY_SENSOR
            if (m_sensor_engine_protection != nullptr)
            {
                m_sensor_engine_protection->
                    publish_state(
                        !get_binary_state(  // on = engine protection ok, off = not ok
                            response.substr(start, end - start)));
            }
#endif

            start = end + 1;
            end = response.find(DELIMITER, start);
#ifdef USE_BINARY_SENSOR
            if (m_sensor_low_pressure_state != nullptr)
            {
                m_sensor_low_pressure_state->
                    publish_state(
                        !get_binary_state(  // on = pressure ok, off = not ok
                            response.substr(start, end - start)));
            }
#endif

            start = end + 1;
            end = response.find(DELIMITER, start);
#ifdef USE_BINARY_SENSOR
            if (m_sensor_external_power != nullptr)
            {
                m_sensor_external_power->
                    publish_state(
                        get_binary_state(
                            response.substr(start, end - start)));
            }
#endif

            m_received_responses |= RESPONSE_INPUTS;

            // request outputs
            send_request(TYPE_OUTPUTS);
        }
        else if (starts_with(response, TYPE_OUTPUTS))
        {
            size_t start = INDEX_RESPONSE_START;
            size_t end = response.find(DELIMITER, start);
            // skip number of elements

            start = end + 1;
            end = response.find(DELIMITER, start);
#ifdef USE_BINARY_SENSOR
            if (m_sensor_defrost_valve != nullptr)
            {
                m_sensor_defrost_valve->
                    publish_state(
                        get_binary_state(
                            response.substr(start, end - start)));
            }
#endif

            start = end + 1;
            end = response.find(DELIMITER, start);
#ifdef USE_BINARY_SENSOR
            if (m_sensor_hot_water_pump != nullptr)
            {
                m_sensor_hot_water_pump->
                    publish_state(
                        get_binary_state(
                            response.substr(start, end - start)));
            }
#endif

            start = end + 1;
            end = response.find(DELIMITER, start);
#ifdef USE_BINARY_SENSOR
            if (m_sensor_floor_heating_pump != nullptr)
            {
                m_sensor_floor_heating_pump->
                    publish_state(
                        get_binary_state(
                            response.substr(start, end - start)));
            }
#endif

            start = end + 1;
            end = response.find(DELIMITER, start);
#ifdef USE_BINARY_SENSOR
            if (m_sensor_heating_pump != nullptr)
            {
                m_sensor_heating_pump->
                    publish_state(
                        get_binary_state(
                            response.substr(start, end - start)));
            }
#endif

            start = end + 1;
            end = response.find(DELIMITER, start);
            bool mixerOpen = get_binary_state(response.substr(start, end - start));

            start = end + 1;
            end = response.find(DELIMITER, start);
            bool mixerClose = get_binary_state(response.substr(start, end - start));

#ifdef USE_TEXT_SENSOR
            // merge the two separate states (Mischer 1 fährt auf, Mischer 1 fährt zu) into one sensor
            if (m_sensor_mixer_1_state != nullptr)
            {
                m_sensor_mixer_1_state->
                    publish_state(
                        mixerOpen
                            ? "opening"
                            : mixerClose
                                ? "closing"
                                : "idle");
            }
#endif

            start = end + 1;
            end = response.find(DELIMITER, start);
#ifdef USE_BINARY_SENSOR
            if (m_sensor_housing_ventilation != nullptr)
            {
                m_sensor_housing_ventilation->
                    publish_state(
                        get_binary_state(
                            response.substr(start, end - start)));
            }
#endif

            start = end + 1;
            end = response.find(DELIMITER, start);
#ifdef USE_BINARY_SENSOR
            if (m_sensor_ventilation_pump != nullptr)
            {
                m_sensor_ventilation_pump->
                    publish_state(
                        get_binary_state(
                            response.substr(start, end - start)));
            }
#endif

            start = end + 1;
            end = response.find(DELIMITER, start);
#ifdef USE_BINARY_SENSOR
            if (m_sensor_compressor_1 != nullptr)
            {
                m_sensor_compressor_1->
                    publish_state(
                        get_binary_state(
                            response.substr(start, end - start)));
            }
#endif

            start = end + 1;
            end = response.find(DELIMITER, start);
#ifdef USE_BINARY_SENSOR
            if (m_sensor_compressor_2 != nullptr)
            {
                m_sensor_compressor_2->
                    publish_state(
                        get_binary_state(
                            response.substr(start, end - start)));
            }
#endif

            start = end + 1;
            end = response.find(DELIMITER, start);
#ifdef USE_BINARY_SENSOR
            if (m_sensor_extra_pump != nullptr)
            {
                m_sensor_extra_pump->
                    publish_state(
                        get_binary_state(
                            response.substr(start, end - start)));
            }
#endif

            start = end + 1;
            end = response.find(DELIMITER, start);
#ifdef USE_BINARY_SENSOR
            if (m_sensor_secondary_heater_1 != nullptr)
            {
                m_sensor_secondary_heater_1->
                    publish_state(
                        get_binary_state(
                            response.substr(start, end - start)));
            }
#endif

            start = end + 1;
            end = response.find(DELIMITER, start);
#ifdef USE_BINARY_SENSOR
            if (m_sensor_secondary_heater_2_failure != nullptr)
            {
                m_sensor_secondary_heater_2_failure->
                    publish_state(
                        get_binary_state(
                            response.substr(start, end - start)));
            }
#endif

            m_received_responses |= RESPONSE_OUTPUTS;

            // request errors
            send_request(TYPE_ERRORS);
        }
        else if (starts_with(response, TYPE_ERRORS))
        {
            size_t start = INDEX_RESPONSE_START;
            size_t end = response.find(DELIMITER, start);
            std::string slot = response.substr(start, end - start);

            if (starts_with(slot, TYPE_ERRORS_SLOT) && (response.length() >= MIN_SLOT_LENGTH))
            {
#ifdef USE_TEXT_SENSOR
                char slotID = response.at(INDEX_SLOT_ID);
                switch (slotID)
                {
                    case SLOT_1_ID: { parse_slot(response.substr(10), m_sensor_error_1_code, m_sensor_error_1_time); break; }
                    case SLOT_2_ID: { parse_slot(response.substr(10), m_sensor_error_2_code, m_sensor_error_2_time); break; }
                    case SLOT_3_ID: { parse_slot(response.substr(10), m_sensor_error_3_code, m_sensor_error_3_time); break; }
                    case SLOT_4_ID: { parse_slot(response.substr(10), m_sensor_error_4_code, m_sensor_error_4_time); break; }
                    case SLOT_5_ID: { parse_slot(response.substr(10), m_sensor_error_5_code, m_sensor_error_5_time); break; }
                }
#endif
            }
            else if (!m_slot_block)
            {
                m_slot_block = true;
            }
            else
            {
                m_slot_block = false;
                m_received_responses |= RESPONSE_ERRORS;

                // request deactivations
                send_request(TYPE_DEACTIVATIONS);
            }
        }
        else if (starts_with(response, TYPE_DEACTIVATIONS))
        {
            size_t start = INDEX_RESPONSE_START;
            size_t end = response.find(DELIMITER, start);
            std::string slot = response.substr(start, end - start);

            if (starts_with(slot, TYPE_DEACTIVATIONS_SLOT) && (response.length() >= MIN_SLOT_LENGTH))
            {
#ifdef USE_TEXT_SENSOR
                char slotID = response.at(INDEX_SLOT_ID);
                switch (slotID)
                {
                    case SLOT_1_ID: { parse_slot(response.substr(10), m_sensor_deactivation_1_code, m_sensor_deactivation_1_time); break; }
                    case SLOT_2_ID: { parse_slot(response.substr(10), m_sensor_deactivation_2_code, m_sensor_deactivation_2_time); break; }
                    case SLOT_3_ID: { parse_slot(response.substr(10), m_sensor_deactivation_3_code, m_sensor_deactivation_3_time); break; }
                    case SLOT_4_ID: { parse_slot(response.substr(10), m_sensor_deactivation_4_code, m_sensor_deactivation_4_time); break; }
                    case SLOT_5_ID: { parse_slot(response.substr(10), m_sensor_deactivation_5_code, m_sensor_deactivation_5_time); break; }
                }
#endif
            }
            else if (!m_slot_block)
            {
                m_slot_block = true;
            }
            else
            {
                m_slot_block = false;
                m_received_responses |= RESPONSE_DEACTIVATIONS;

                // request information
                send_request(TYPE_INFORMATION);
            }
        }
        else if (starts_with(response, TYPE_INFORMATION))
        {
            int32_t value = 0;
            std::string text = "";
            size_t start = INDEX_RESPONSE_START;
            size_t end = response.find(DELIMITER, start);
            // skip number of elements

            start = end + 1;
            end = response.find(DELIMITER, start);
#ifdef USE_TEXT_SENSOR
            if (m_sensor_device_type != nullptr)
            {
                value = std::atoi(response.substr(start, end - start).c_str());
                switch (static_cast<DeviceType>(value))
                {
                    case DeviceType::ERC: { text = "ERC";                 break; }
                    case DeviceType::SW1: { text = "SW1";                 break; }
                    case DeviceType::SW2: { text = "SW2";                 break; }
                    case DeviceType::WW1: { text = "WW1";                 break; }
                    case DeviceType::WW2: { text = "WW2";                 break; }
                    case DeviceType::L1I: { text = "L1I";                 break; }
                    case DeviceType::L2I: { text = "L2I";                 break; }
                    case DeviceType::L1A: { text = "L1A";                 break; }
                    case DeviceType::L2A: { text = "L2A";                 break; }
                    case DeviceType::KSW: { text = "KSW";                 break; }
                    case DeviceType::KLW: { text = "KLW";                 break; }
                    case DeviceType::SWC: { text = "SWC";                 break; }
                    case DeviceType::LWC: { text = "LWC";                 break; }
                    case DeviceType::L2G: { text = "L2G";                 break; }
                    case DeviceType::WZS: { text = "WZS";                 break; }
                    default:              { text = std::to_string(value); break; }
                }
                m_sensor_device_type->publish_state(text);
            }
#endif

            start = end + 1;
            end = response.find(DELIMITER, start);
#ifdef USE_TEXT_SENSOR
            if (m_sensor_firmware_version != nullptr)
            {
                m_sensor_firmware_version->
                    publish_state(
                        response.substr(
                            start + 1,  // version without 'V' prefix
                            end - start - 1));
            }
#endif

            start = end + 1;
            end = response.find(DELIMITER, start);
#ifdef USE_TEXT_SENSOR
            if (m_sensor_bivalence_level != nullptr)
            {
                value = std::atoi(response.substr(start, end - start).c_str());
                switch (static_cast<BivalenceLevel>(value))
                {
                    case BivalenceLevel::SINGLE_COMPRESSOR: { text = "single_compressor";   break; }
                    case BivalenceLevel::DUAL_COMPRESSOR:   { text = "dual_compressor";     break; }
                    case BivalenceLevel::ADDITIONAL_HEATER: { text = "additional_heater";   break; }
                    default:                                { text = std::to_string(value); break; }
                }
                m_sensor_bivalence_level->publish_state(text);
            }
#endif

            start = end + 1;
            end = response.find(DELIMITER, start);
#ifdef USE_TEXT_SENSOR
            if (m_sensor_operational_state != nullptr)
            {
                value = std::atoi(response.substr(start, end - start).c_str());
                switch (static_cast<OperationalState>(value))
                {
                    case OperationalState::HEAT:          { text = "heat";                break; }
                    case OperationalState::HOT_WATER:     { text = "hot_water";           break; }
                    case OperationalState::SWIMMING_POOL: { text = "swimming_pool";       break; }
                    case OperationalState::PROVIDER_LOCK: { text = "provider_lock";       break; }
                    case OperationalState::DEFROST:       { text = "defrost";             break; }
                    case OperationalState::STANDBY:       { text = "standby";             break; }
                    default:                              { text = std::to_string(value); break; }
                }
                m_sensor_operational_state->publish_state(text);
            }
#endif

            m_received_responses |= RESPONSE_INFORMATION;

            // request heating mode
            send_request(TYPE_HEATING_MODE);
        }
        else if (starts_with(response, TYPE_HEATING_MODE))
        {
            int32_t value = 0;
            std::string text = "";
            size_t start = INDEX_RESPONSE_START;
            size_t end = response.find(DELIMITER, start);
            // skip number of elements

            start = end + 1;
            end = response.find(DELIMITER, start);
#ifdef USE_TEXT_SENSOR
            if (m_sensor_heating_mode != nullptr)
            {
                value = std::atoi(response.substr(start, end - start).c_str());
                switch (static_cast<OperationalMode>(value))
                {
                    case OperationalMode::AUTO:          { text = "auto";                break; }
                    case OperationalMode::SECOND_HEATER: { text = "second_heater";       break; }
                    case OperationalMode::PARTY:         { text = "party";               break; }
                    case OperationalMode::VACATION:      { text = "vacation";            break; }
                    case OperationalMode::OFF:           { text = "off";                 break; }
                    default:                             { text = std::to_string(value); break; }
                }
                m_sensor_heating_mode->publish_state(text);
            }
#endif

            m_received_responses |= RESPONSE_HEATING_MODE;

            // request hot water mode
            send_request(TYPE_HOT_WATER_MODE);
        }
        else if (starts_with(response, TYPE_HOT_WATER_MODE))
        {
            int32_t value = 0;
            std::string text = "";
            size_t start = INDEX_RESPONSE_START;
            size_t end = response.find(DELIMITER, start);
            // skip number of elements

            start = end + 1;
            end = response.find(DELIMITER, start);
#ifdef USE_TEXT_SENSOR
            if (m_sensor_hot_water_mode != nullptr)
            {
                value = std::atoi(response.substr(start, end - start).c_str());
                switch (static_cast<OperationalMode>(value))
                {
                    case OperationalMode::AUTO:          { text = "auto";                break; }
                    case OperationalMode::SECOND_HEATER: { text = "second_heater";       break; }
                    case OperationalMode::PARTY:         { text = "party";               break; }
                    case OperationalMode::VACATION:      { text = "vacation";            break; }
                    case OperationalMode::OFF:           { text = "off";                 break; }
                    default:                             { text = std::to_string(value); break; }
                }
                m_sensor_hot_water_mode->publish_state(text);
            }
#endif

            m_received_responses |= RESPONSE_HOT_WATER_MODE;
        }
    }

#ifdef USE_TEXT_SENSOR
    void Luxtronik::parse_slot(const std::string& slot, text_sensor::TextSensor* sensor_code, text_sensor::TextSensor* sensor_time)
    {
        size_t start = 0;
        size_t end = slot.find(DELIMITER, start);
        // skip number of elements

        start = end + 1;
        end = slot.find(DELIMITER, start);
        if (sensor_code != nullptr)
        {
            sensor_code->
                publish_state(
                    slot.substr(start, end - start));
        }

        if (sensor_time != nullptr)
        {
            std::tm dt = {};

            start = end + 1;
            end = slot.find(DELIMITER, start);
            dt.tm_mday = static_cast<int32_t>(get_number(slot.substr(start, end - start)));

            start = end + 1;
            end = slot.find(DELIMITER, start);
            dt.tm_mon = static_cast<int32_t>(get_number(slot.substr(start, end - start))) - 1;  // convert from range 1-12 to range 0-11

            start = end + 1;
            end = slot.find(DELIMITER, start);
            dt.tm_year = static_cast<int32_t>(get_number(slot.substr(start, end - start))) + 100;  // convert from years since 2000 to years since 1900

            start = end + 1;
            end = slot.find(DELIMITER, start);
            dt.tm_hour = static_cast<int32_t>(get_number(slot.substr(start, end - start)));

            start = end + 1;
            end = slot.find(DELIMITER, start);
            dt.tm_min = static_cast<int32_t>(get_number(slot.substr(start, end - start)));

            dt.tm_isdst = -1;  // auto-detect daylight saving time
            std::mktime(&dt);  // convert to universal time

            char timeStr[] = "yyyy-mm-ddThh:mm+zzzz";
            std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%dT%H:%M%z", &dt);

            sensor_time->publish_state(timeStr);
        }
    }
#endif
}  // namespace esphome::luxtronik_v1
