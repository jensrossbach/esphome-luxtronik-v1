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


#include "luxtronik_number.h"


#ifdef USE_NUMBER

namespace esphome::luxtronik_v1
{
    void LuxtronikNumber::control(float value)
    {
        m_set_trigger.trigger(value);
        publish_state(value);
    }

#ifdef USE_SENSOR
    void LuxtronikNumber::set_source_sensor(sensor::Sensor* sensor)
    {
        m_source_sensor = sensor;

        m_source_sensor->add_on_state_callback([this](float value)
        {
#ifdef USE_SWITCH
            bool edit_mode = (m_edit_mode_switch != nullptr) ? m_edit_mode_switch->state : false;
#else
            bool edit_mode = false;
#endif

            if (!edit_mode)
            {
                publish_state(value);
            }
        });
    }
#endif
}  // namespace esphome::luxtronik_v1

#endif
