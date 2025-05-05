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

#include <cstdint>
#include <string>


#ifdef USE_SENSOR
#define LOG_NUMERIC_SENSOR(type, obj) LOG_SENSOR("", type, obj.get_sensor());
#define LOG_TEMPERATURE_SENSOR(type, obj) LOG_SENSOR("", type, obj.get_sensor());
#else
#define LOG_NUMERIC_SENSOR(type, obj)
#define LOG_TEMPERATURE_SENSOR(type, obj)
#endif

#ifdef USE_BINARY_SENSOR
#define LOG_BOOL_SENSOR(type, obj) LOG_BINARY_SENSOR("", type, obj.get_sensor());
#else
#define LOG_BOOL_SENSOR(type, obj)
#endif

#ifdef USE_TEXT_SENSOR
#define LOG_STRING_SENSOR(type, obj) LOG_TEXT_SENSOR("", type, obj.get_sensor());
#define LOG_DURATION_SENSOR(type, obj) \
    LOG_TEXT_SENSOR("", type, obj.get_sensor()); \
    if (obj.has_sensor()) \
    { \
        ESP_LOGCONFIG(TAG, "  Format: %u", obj.get_format()); \
    }
#else
#define LOG_STRING_SENSOR(type, obj)
#define LOG_DURATION_SENSOR(type, obj)
#endif


namespace esphome::luxtronik_v1
{
    class NumericSensor
    {
    public:
        NumericSensor()
#ifdef USE_SENSOR
            : m_sensor(nullptr)
#endif
        {
        }

        void set_state(const std::string& input_str, uint16_t start, uint16_t end)
        {
#ifdef USE_SENSOR
            if (m_sensor != nullptr)
            {
                m_sensor->publish_state(
                            static_cast<float>(
                                std::atoi(input_str.substr(start, end - start).c_str())));
            }
#endif
        }

        float get_state() const
        {
#ifdef USE_SENSOR
            return (m_sensor != nullptr) ? m_sensor->state : 0.0;
#else
            return 0.0;
#endif
        }

#ifdef USE_SENSOR
        void set_sensor(sensor::Sensor* sensor) { m_sensor = sensor; }
        sensor::Sensor* get_sensor() { return m_sensor; }
#endif

    protected:
#ifdef USE_SENSOR
        sensor::Sensor* m_sensor;
#endif
    };

    class TemperatureSensor : public NumericSensor
    {
    public:
        TemperatureSensor()
            : NumericSensor()
        {
        }

        void set_state(const std::string& input_str, uint16_t start, uint16_t end)
        {
#ifdef USE_SENSOR
            if (m_sensor != nullptr)
            {
                m_sensor->publish_state(
                            static_cast<float>(
                                std::atoi(input_str.substr(start, end - start).c_str())) / 10);
            }
#endif
        }
    };

    class StringSensor
    {
    public:
        StringSensor()
#ifdef USE_TEXT_SENSOR
            : m_sensor(nullptr)
#endif
        {
        }

        void set_state(const std::string& input_str)
        {
#ifdef USE_TEXT_SENSOR
            if (m_sensor != nullptr)
            {
                m_sensor->publish_state(input_str);
            }
#endif
        }

        void set_state(const std::string& input_str, uint16_t start, uint16_t end)
        {
#ifdef USE_TEXT_SENSOR
            if (m_sensor != nullptr)
            {
                m_sensor->publish_state(input_str.substr(start, end - start));
            }
#endif
        }

        bool has_sensor()
        {
#ifdef USE_TEXT_SENSOR
            return m_sensor != nullptr;
#else
            return false;
#endif
        }

#ifdef USE_TEXT_SENSOR
        void set_sensor(text_sensor::TextSensor* sensor) { m_sensor = sensor; }
        text_sensor::TextSensor* get_sensor() { return m_sensor; }
#endif

    protected:
#ifdef USE_TEXT_SENSOR
        text_sensor::TextSensor* m_sensor;
#endif
    };

    class DurationSensor : public StringSensor
    {
    public:
        enum class Format
        {
            ISO_8601,       // ISO-8601 duration format
            HUMAN_READABLE  // Human readable string in the form "h:mm:ss"
        };

        DurationSensor()
            : StringSensor()
            , m_format(Format::ISO_8601)
        {
        }

        void set_state(const std::string& input_str, uint16_t start, uint16_t end)
        {
#ifdef USE_TEXT_SENSOR
            if (m_sensor != nullptr)
            {
                std::string state;
                uint32_t value = static_cast<uint32_t>(std::atol(input_str.substr(start, end - start).c_str()));

                if (value > 0)
                {
                    uint32_t hours = value / 3600;
                    uint32_t minutes = (value / 60) % 60;
                    uint32_t seconds = value % 60;

                    switch (m_format)
                    {
                        case Format::ISO_8601:
                        {
                            state = "PT";

                            if (hours > 0)   { state += std::to_string(hours)   + "H"; }
                            if (minutes > 0) { state += std::to_string(minutes) + "M"; }
                            if (seconds > 0) { state += std::to_string(seconds) + "S"; }

                            break;
                        }
                        case Format::HUMAN_READABLE:
                        {
                            state = std::to_string(hours) + ':'
                                    + ((minutes < 10) ? '0' + std::to_string(minutes) : std::to_string(minutes)) + ':'
                                    + ((seconds < 10) ? '0' + std::to_string(seconds) : std::to_string(seconds));

                            break;
                        }
                    }
                }

                m_sensor->publish_state(state);
            }
#endif
        }

#ifdef ESPHOME_LOG_HAS_CONFIG
        const char* get_format() const
        {
            const char* format = nullptr;

            switch (m_format)
            {
                case Format::ISO_8601:       { format = "iso_8601";       break; }
                case Format::HUMAN_READABLE: { format = "human_readable"; break; }
            }

            return format;
        }
#endif

#ifdef USE_TEXT_SENSOR
        void set_sensor(text_sensor::TextSensor* sensor, int32_t format)
        {
            m_sensor = sensor;

            switch (format)
            {
                case 0: { m_format = Format::ISO_8601;       break; }
                case 1: { m_format = Format::HUMAN_READABLE; break; }
            }
        }
#endif

    protected:
        Format m_format;
    };

    class BoolSensor
    {
    public:
        BoolSensor()
#ifdef USE_BINARY_SENSOR
            : m_sensor(nullptr)
#endif
        {
        }

        void set_state(const std::string& input_str, uint16_t start, uint16_t end, bool invert = false)
        {
#ifdef USE_BINARY_SENSOR
            if (m_sensor != nullptr)
            {
                std::string part = input_str.substr(start, end - start);
                m_sensor->publish_state(invert ? (part == "0") : (part != "0"));
            }
#endif
        }

        void set_state(bool state)
        {
#ifdef USE_BINARY_SENSOR
            if (m_sensor != nullptr)
            {
                m_sensor->publish_state(state);
            }
#endif
        }

#ifdef USE_BINARY_SENSOR
        void set_sensor(binary_sensor::BinarySensor* sensor) { m_sensor = sensor; }
        binary_sensor::BinarySensor* get_sensor() { return m_sensor; }
#endif

    protected:
#ifdef USE_BINARY_SENSOR
        binary_sensor::BinarySensor* m_sensor;
#endif
    };
}  // namespace esphome::luxtronik_v1
