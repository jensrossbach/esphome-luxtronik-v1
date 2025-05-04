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

#include "esphome/core/component.h"
#include "esphome/core/automation.h"

#include "luxtronik.h"


namespace esphome::luxtronik_v1
{
    template<typename... Ts> class SetHotWaterSetTemperatureAction : public Action<Ts...>
    {
    public:
    SetHotWaterSetTemperatureAction(Luxtronik *luxtronik)
            : m_luxtronik(luxtronik)
            , m_set_temperature_value()
        {
        }

        void play(Ts... x) override
        {
            m_luxtronik->set_hot_water_set_temperature(m_set_temperature_value.value(x...));
        }

        template<typename V> void set_hot_water_set_temperature_value(V value)
        {
            m_set_temperature_value = value;
        }

    protected:
        Luxtronik *m_luxtronik;
        TemplatableValue<float, Ts...> m_set_temperature_value;
    };
}  // namespace esphome::ferraris
