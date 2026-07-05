/*
 * Copyright (c) 2024-2026 Jens-Uwe Rossbach
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


#include "luxtronik_climate.h"

#ifdef USE_CLIMATE

#include "esphome/core/log.h"


namespace esphome::luxtronik_v1
{
    static constexpr const char *const TAG = "luxtronik_climate";

    void LuxtronikClimate::setup()
    {
#ifdef USE_SENSOR
        auto* current_temp_sensor = parent_->get_hot_water_temperature_sensor().get_sensor();
        if (current_temp_sensor != nullptr)
        {
            this->current_temperature = current_temp_sensor->get_state();
            current_temp_sensor->add_on_state_callback([this](float value)
            {
                if (value != this->current_temperature)
                {
                    this->current_temperature = value;
                    publish_state();
                }
            });
        }

        auto* target_temp_sensor = parent_->get_hot_water_set_temperature_sensor().get_sensor();
        if (target_temp_sensor != nullptr)
        {
            this->target_temperature = current_temp_sensor->get_state();
            target_temp_sensor->add_on_state_callback([this](float value)
            {
                if (value != this->target_temperature)
                {
                    this->target_temperature = value;
                    publish_state();
                }
            });
        }
#endif

#ifdef USE_TEXT_SENSOR
        auto* mode_sensor = parent_->get_hot_water_mode_sensor().get_sensor();
        if (mode_sensor != nullptr)
        {
            auto get_climate_mode = [](const std::string& value) -> climate::ClimateMode
            {
                if (value == "off")
                {
                    return climate::CLIMATE_MODE_OFF;
                }

                return climate::CLIMATE_MODE_HEAT;
            };

            this->mode = get_climate_mode(mode_sensor->get_raw_state());
            mode_sensor->add_on_raw_state_callback([this, &get_climate_mode](const std::string& value)
            {
                auto mode = get_climate_mode(value);

                if (mode != this->mode)
                {
                    this->mode = mode;
                    publish_state();
                }
            });
        }

        auto* state_sensor = parent_->get_operational_state_sensor().get_sensor();
        if (state_sensor != nullptr)
        {
            auto get_climate_action = [](const std::string& value) -> climate::ClimateAction
            {
                if (value == "hot_water")
                {
                    return climate::CLIMATE_ACTION_HEATING;
                }

                return climate::CLIMATE_ACTION_IDLE;
            };

            this->action = get_climate_action(state_sensor->get_raw_state());
            state_sensor->add_on_raw_state_callback([this, &get_climate_action](const std::string& value)
            {
                auto action = get_climate_action(value);

                if (action != this->action)
                {
                    this->action = action;
                    publish_state();
                }
            });
        }
        #endif

        publish_state();
    }

    void LuxtronikClimate::control(const climate::ClimateCall& call)
    {
        if (call.get_mode().has_value())
        {
            auto mode = call.get_mode().value();
            this->mode = mode;

            switch (mode)
            {
                case climate::CLIMATE_MODE_OFF:
                {
                    ESP_LOGD(TAG, "Setting hot water mode to OFF");
                    parent_->set_operational_mode(
                                Luxtronik::OperationalModeType::HOT_WATER,
                                static_cast<uint8_t>(Luxtronik::OperationalMode::OFF));
                    break;
                }
                case climate::CLIMATE_MODE_HEAT:
                {
                    ESP_LOGD(TAG, "Setting hot water mode to AUTO");
                    parent_->set_operational_mode(
                                Luxtronik::OperationalModeType::HOT_WATER,
                                static_cast<uint8_t>(Luxtronik::OperationalMode::AUTO));
                    break;
                }
                default:
                {
                    break;
                }
            }
        }

        if (call.get_target_temperature().has_value())
        {
            auto temp = call.get_target_temperature().value();
            auto min_temp = get_traits().get_visual_min_temperature();
            auto max_temp = get_traits().get_visual_max_temperature();

            if (temp < min_temp)
            {
                temp = min_temp;
            }
            else if (temp > max_temp)
            {
                temp = max_temp;
            }

            this->target_temperature = temp;

            ESP_LOGD(TAG, "Setting hot water target temperature to %.1f °C", temp);
            parent_->set_hot_water_set_temperature(temp);
        }

        publish_state();
    }

    climate::ClimateTraits LuxtronikClimate::traits()
    {
        auto traits = climate::ClimateTraits();

        traits.set_supported_modes({
                    climate::CLIMATE_MODE_OFF,
                    climate::CLIMATE_MODE_HEAT});
        traits.set_feature_flags(
                    climate::CLIMATE_SUPPORTS_CURRENT_TEMPERATURE|
                    climate::CLIMATE_SUPPORTS_ACTION);

        return traits;
    }
}

#endif
