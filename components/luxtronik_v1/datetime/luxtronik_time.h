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

#ifdef USE_DATETIME_TIME

#include "esphome/core/automation.h"
#include "esphome/components/datetime/time_entity.h"
#ifdef USE_TEXT_SENSOR
#include "esphome/components/text_sensor/text_sensor.h"
#endif
#ifdef USE_SWITCH
#include "esphome/components/switch/switch.h"
#endif


namespace esphome::luxtronik_v1
{
    class LuxtronikTime
            : public datetime::TimeEntity
    {
    public:
        LuxtronikTime()
            : datetime::TimeEntity()
            , m_set_trigger()
#ifdef USE_TEXT_SENSOR
            , m_source_sensor(nullptr)
#endif
#ifdef USE_SWITCH
            , m_edit_mode_switch(nullptr)
#endif
        {
        }

        Trigger<ESPTime>* get_set_trigger() { return &m_set_trigger; }
#ifdef USE_TEXT_SENSOR
        void set_source_sensor(text_sensor::TextSensor* sensor);
#endif
#ifdef USE_SWITCH
        void set_edit_mode_switch(switch_::Switch* swt) { m_edit_mode_switch = swt; }
#endif

    protected:
        void control(const datetime::TimeCall &call) override;

        Trigger<ESPTime> m_set_trigger;
#ifdef USE_TEXT_SENSOR
        text_sensor::TextSensor* m_source_sensor;
#endif
#ifdef USE_SWITCH
        switch_::Switch* m_edit_mode_switch;
#endif
    };
}  // namespace esphome::luxtronik_v1

#endif
