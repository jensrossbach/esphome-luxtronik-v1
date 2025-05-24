/*
 * Copyright (c) 2025 Jens-Uwe Rossbach
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


#include "luxtronik_time.h"


#ifdef USE_DATETIME_TIME

namespace esphome::luxtronik_v1
{
    void LuxtronikTime::control(const datetime::TimeCall &call)
    {
        bool has_hour = call.get_hour().has_value();
        bool has_minute = call.get_minute().has_value();

        ESPTime value = {};

        if (has_hour)
        {
            hour_ = *call.get_hour();
            value.hour = hour_;
        }
        if (has_minute)
        {
            minute_ = *call.get_minute();
            value.minute = minute_;
        }

        second_ = 0;
        value.second = 0;

        m_set_trigger.trigger(value);
        publish_state();
    }

#ifdef USE_TEXT_SENSOR
    void LuxtronikTime::set_source_sensor(text_sensor::TextSensor* sensor)
    {
        m_source_sensor = sensor;

        m_source_sensor->add_on_state_callback([this](std::string value)
        {
#ifdef USE_SWITCH
            bool edit_mode = (m_edit_mode_switch != nullptr) ? m_edit_mode_switch->state : false;
#else
            bool edit_mode = false;
#endif

            if (!edit_mode)
            {
                hour_ = static_cast<uint8_t>(atoi(value.substr(0, value.find(':')).c_str()));
                minute_ = static_cast<uint8_t>(atoi(value.substr(value.find(':') + 1).c_str()));
                second_ = 0;

                publish_state();
            }
        });
    }
#endif
}  // namespace esphome::luxtronik_v1

#endif
