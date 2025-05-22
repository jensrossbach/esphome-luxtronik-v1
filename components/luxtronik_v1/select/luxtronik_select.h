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


#pragma once

#include "esphome/core/defines.h"
#include "esphome/core/automation.h"
#include "esphome/components/select/select.h"
#ifdef USE_TEXT_SENSOR
#include "esphome/components/text_sensor/text_sensor.h"
#endif


namespace esphome::luxtronik_v1
{
    class LuxtronikSelect
            : public select::Select
    {
    public:
        LuxtronikSelect()
            : select::Select()
            , m_set_trigger()
        {
        }

        Trigger<std::string, uint8_t>* get_set_trigger() { return &m_set_trigger; }

#ifdef USE_SENSOR
        void set_source_sensor(text_sensor::TextSensor* sensor);
#endif

    protected:
        void control(const std::string &value) override;

        Trigger<std::string, uint8_t> m_set_trigger;
#ifdef USE_SENSOR
        text_sensor::TextSensor* m_source_sensor;
#endif
    };
}  // namespace esphome::luxtronik_v1
