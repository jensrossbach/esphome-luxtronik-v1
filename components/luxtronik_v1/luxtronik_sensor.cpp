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


#include "luxtronik_sensor.h"


namespace esphome::luxtronik_v1
{
    void NumericSensor::set_state(const std::string& input_str, uint16_t start, uint16_t end)
    {
#ifdef USE_SENSOR
        if (m_sensor != nullptr)
        {
            m_task_handler.enqueue_task(std::bind(
                                &sensor::Sensor::publish_state,
                                m_sensor, static_cast<float>(
                                    std::atoi(input_str.substr(start, end - start).c_str()))));
        }
#endif
    }

    void TemperatureSensor::set_state(const std::string& input_str, uint16_t start, uint16_t end)
    {
#ifdef USE_SENSOR
        if (m_sensor != nullptr)
        {
            m_task_handler.enqueue_task(std::bind(
                                &sensor::Sensor::publish_state,
                                m_sensor, static_cast<float>(
                                    std::atoi(input_str.substr(start, end - start).c_str())) / 10));
        }
#endif
    }

    void StringSensor::set_state(const std::string& input_str)
    {
#ifdef USE_TEXT_SENSOR
        if (m_sensor != nullptr)
        {
            m_task_handler.enqueue_task(std::bind(
                                &text_sensor::TextSensor::publish_state,
                                m_sensor, input_str));
        }
#endif
    }

    void StringSensor::set_state(const std::string& input_str, uint16_t start, uint16_t end)
    {
#ifdef USE_TEXT_SENSOR
        if (m_sensor != nullptr)
        {
            m_task_handler.enqueue_task(std::bind(
                                &text_sensor::TextSensor::publish_state,
                                m_sensor, input_str.substr(start, end - start)));
        }
#endif
    }

    void DurationSensor::set_state(const std::string& input_str, uint16_t start, uint16_t end)
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

            m_task_handler.enqueue_task(std::bind(
                                &text_sensor::TextSensor::publish_state,
                                m_sensor, std::move(state)));
        }
#endif
    }

    void BoolSensor::set_state(const std::string& input_str, uint16_t start, uint16_t end, bool invert)
    {
#ifdef USE_BINARY_SENSOR
        if (m_sensor != nullptr)
        {
            std::string part = input_str.substr(start, end - start);
            m_task_handler.enqueue_task(std::bind(
                                &binary_sensor::BinarySensor::publish_state,
                                m_sensor, invert ? (part == "0") : (part != "0")));
        }
#endif
    }

    void BoolSensor::set_state(bool state)
    {
#ifdef USE_BINARY_SENSOR
        if (m_sensor != nullptr)
        {
            m_task_handler.enqueue_task(std::bind(
                                &binary_sensor::BinarySensor::publish_state,
                                m_sensor, state));
        }
#endif
    }
}  // namespace esphome::luxtronik_v1
