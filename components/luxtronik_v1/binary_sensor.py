# Copyright (c) 2024 Jens-Uwe Rossbach
#
# This code is licensed under the MIT License.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.


import esphome.codegen           as cg
import esphome.config_validation as cv

from esphome.components import binary_sensor
from esphome.const      import (
    CONF_ID,
    DEVICE_CLASS_LOCK,
    DEVICE_CLASS_PROBLEM,
    DEVICE_CLASS_RUNNING,
    ENTITY_CATEGORY_DIAGNOSTIC
)
from .                  import (
    Luxtronik,
    CONF_LUXTRONIK_ID
)


CODEOWNERS = ["@jensrossbach"]

CONF_DEFROST_BRINE_FLOW         = "defrost_brine_flow"
CONF_POWER_PROVIDER_LOCK_PERIOD = "power_provider_lock_period"
CONF_LOW_PRESSURE_STATE         = "low_pressure_state"
CONF_HIGH_PRESSURE_STATE        = "high_pressure_state"
CONF_ENGINE_PROTECTION          = "engine_protection"
CONF_EXTERNAL_POWER             = "external_power"
CONF_DEFROST_VALVE              = "defrost_valve"
CONF_HOT_WATER_PUMP             = "hot_water_pump"
CONF_HEATING_PUMP               = "heating_pump"
CONF_FLOOR_HEATING_PUMP         = "floor_heating_pump"
CONF_VENTILATION_HOUSING        = "housing_ventilation"
CONF_VENTILATION_PUMP           = "ventilation_pump"
CONF_COMPRESSOR_1               = "compressor_1"
CONF_COMPRESSOR_2               = "compressor_2"
CONF_EXTRA_PUMP                 = "extra_pump"
CONF_SECONDARY_HEATER_1         = "secondary_heater_1"
CONF_SECONDARY_HEATER_2_FAILURE = "secondary_heater_2_failure"
CONF_DEVICE_COMMUNICATION       = "device_communication"

CONFIG_SCHEMA = cv.Schema(
{
    cv.GenerateID(CONF_LUXTRONIK_ID): cv.use_id(Luxtronik),
    cv.Optional(CONF_DEFROST_BRINE_FLOW): binary_sensor.binary_sensor_schema(
        icon="mdi:snowflake-off"
    ),
    cv.Optional(CONF_POWER_PROVIDER_LOCK_PERIOD): binary_sensor.binary_sensor_schema(
        device_class=DEVICE_CLASS_LOCK
    ),
    cv.Optional(CONF_LOW_PRESSURE_STATE): binary_sensor.binary_sensor_schema(
        device_class=DEVICE_CLASS_PROBLEM
    ),
    cv.Optional(CONF_HIGH_PRESSURE_STATE): binary_sensor.binary_sensor_schema(
        device_class=DEVICE_CLASS_PROBLEM
    ),
    cv.Optional(CONF_ENGINE_PROTECTION): binary_sensor.binary_sensor_schema(
        device_class=DEVICE_CLASS_PROBLEM
    ),
    cv.Optional(CONF_EXTERNAL_POWER): binary_sensor.binary_sensor_schema(
        icon="mdi:transmission-tower"
    ),
    cv.Optional(CONF_DEFROST_VALVE): binary_sensor.binary_sensor_schema(
        icon="mdi:pipe-valve"
    ),
    cv.Optional(CONF_HOT_WATER_PUMP): binary_sensor.binary_sensor_schema(
        device_class=DEVICE_CLASS_RUNNING,
        icon="mdi:pump"
    ),
    cv.Optional(CONF_HEATING_PUMP): binary_sensor.binary_sensor_schema(
        device_class=DEVICE_CLASS_RUNNING,
        icon="mdi:pump"
    ),
    cv.Optional(CONF_FLOOR_HEATING_PUMP): binary_sensor.binary_sensor_schema(
        device_class=DEVICE_CLASS_RUNNING,
        icon="mdi:pump"
    ),
    cv.Optional(CONF_VENTILATION_HOUSING): binary_sensor.binary_sensor_schema(
        device_class=DEVICE_CLASS_RUNNING,
        icon="mdi:fan"
    ),
    cv.Optional(CONF_VENTILATION_PUMP): binary_sensor.binary_sensor_schema(
        device_class=DEVICE_CLASS_RUNNING,
        icon="mdi:fan"
    ),
    cv.Optional(CONF_COMPRESSOR_1): binary_sensor.binary_sensor_schema(
        device_class=DEVICE_CLASS_RUNNING,
        icon="mdi:engine"
    ),
    cv.Optional(CONF_COMPRESSOR_2): binary_sensor.binary_sensor_schema(
        device_class=DEVICE_CLASS_RUNNING,
        icon="mdi:engine"
    ),
    cv.Optional(CONF_EXTRA_PUMP): binary_sensor.binary_sensor_schema(
        device_class=DEVICE_CLASS_RUNNING,
        icon="mdi:pump"
    ),
    cv.Optional(CONF_SECONDARY_HEATER_1): binary_sensor.binary_sensor_schema(
        device_class=DEVICE_CLASS_RUNNING,
        icon="mdi:heating-coil"
    ),
    cv.Optional(CONF_SECONDARY_HEATER_2_FAILURE): binary_sensor.binary_sensor_schema(
        device_class=DEVICE_CLASS_PROBLEM
    ),
    cv.Optional(CONF_DEVICE_COMMUNICATION): binary_sensor.binary_sensor_schema(
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        device_class=DEVICE_CLASS_PROBLEM
    )
})


async def to_code(config):
    cmp = await cg.get_variable(config[CONF_LUXTRONIK_ID])

    for key, conf in config.items():
        if isinstance(conf, dict):
            id = conf[CONF_ID]
            if id and id.type == binary_sensor.BinarySensor:
                sens = await binary_sensor.new_binary_sensor(conf)
                cg.add(getattr(cmp, f"set_sensor_{key}")(sens))
