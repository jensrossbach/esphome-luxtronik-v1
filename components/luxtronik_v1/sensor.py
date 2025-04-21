# Copyright (c) 2024-2025 Jens-Uwe Rossbach
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

from esphome.components import sensor
from esphome.const      import (
    CONF_ID,
    STATE_CLASS_MEASUREMENT,
    STATE_CLASS_TOTAL_INCREASING,
    DEVICE_CLASS_TEMPERATURE,
    UNIT_CELSIUS
)
from .                  import (
    Luxtronik,
    CONF_LUXTRONIK_ID
)


CODEOWNERS = ["@jensrossbach"]

CONF_FLOW_TEMPERATURE                 = "flow_temperature"
CONF_RETURN_TEMPERATURE               = "return_temperature"
CONF_RETURN_SET_TEMPERATURE           = "return_set_temperature"
CONF_HOT_GAS_TEMPERATURE              = "hot_gas_temperature"
CONF_OUTSIDE_TEMPERATURE              = "outside_temperature"
CONF_HOT_WATER_TEMPERATURE            = "hot_water_temperature"
CONF_HOT_WATER_SET_TEMPERATURE        = "hot_water_set_temperature"
CONF_HEAT_SOURCE_INPUT_TEMPERATURE    = "heat_source_input_temperature"
CONF_HEAT_SOURCE_OUTPUT_TEMPERATURE   = "heat_source_output_temperature"
CONF_MIXED_CIRCUIT_1_TEMPERATURE      = "mixed_circuit_1_temperature"
CONF_MIXED_CIRCUIT_1_SET_TEMPERATURE  = "mixed_circuit_1_set_temperature"
CONF_REMOTE_ADJUSTER_TEMPERATURE      = "remote_adjuster_temperature"
CONF_HEATING_CURVE_OFFSET             = "heating_curve_offset"
CONF_HEATING_CURVE_ENDPOINT           = "heating_curve_endpoint"
CONF_HEATING_CURVE_PARALLEL_SHIFT     = "heating_curve_parallel_shift"
CONF_HEATING_CURVE_NIGHT_SETBACK      = "heating_curve_night_setback"
CONF_HEATING_CURVE_CONSTANT_RETURN    = "heating_curve_constant_return"
CONF_HEATING_CURVE_MC1_END_POINT      = "heating_curve_mc1_end_point"
CONF_HEATING_CURVE_MC1_PARALLEL_SHIFT = "heating_curve_mc1_parallel_shift"
CONF_HEATING_CURVE_MC1_NIGHT_SETBACK  = "heating_curve_mc1_night_setback"
CONF_HEATING_CURVE_MC1_CONSTANT_FLOW  = "heating_curve_mc1_constant_flow"
CONF_IMPULSES_COMPRESSOR_1            = "impulses_compressor_1"
CONF_IMPULSES_COMPRESSOR_2            = "impulses_compressor_2"

CONFIG_SCHEMA = cv.Schema(
{
    cv.GenerateID(CONF_LUXTRONIK_ID): cv.use_id(Luxtronik),
    cv.Optional(CONF_FLOW_TEMPERATURE): sensor.sensor_schema(
        device_class = DEVICE_CLASS_TEMPERATURE,
        state_class = STATE_CLASS_MEASUREMENT,
        unit_of_measurement = UNIT_CELSIUS,
        accuracy_decimals = 1,
        icon = "mdi:thermometer-chevron-up"
    ),
    cv.Optional(CONF_RETURN_TEMPERATURE): sensor.sensor_schema(
        device_class = DEVICE_CLASS_TEMPERATURE,
        state_class = STATE_CLASS_MEASUREMENT,
        unit_of_measurement = UNIT_CELSIUS,
        accuracy_decimals = 1,
        icon = "mdi:thermometer-chevron-down"
    ),
    cv.Optional(CONF_RETURN_SET_TEMPERATURE): sensor.sensor_schema(
        device_class = DEVICE_CLASS_TEMPERATURE,
        state_class = STATE_CLASS_MEASUREMENT,
        unit_of_measurement = UNIT_CELSIUS,
        accuracy_decimals = 1,
        icon = "mdi:thermometer-chevron-down"
    ),
    cv.Optional(CONF_HOT_GAS_TEMPERATURE): sensor.sensor_schema(
        device_class = DEVICE_CLASS_TEMPERATURE,
        state_class = STATE_CLASS_MEASUREMENT,
        unit_of_measurement = UNIT_CELSIUS,
        accuracy_decimals = 1
    ),
    cv.Optional(CONF_OUTSIDE_TEMPERATURE): sensor.sensor_schema(
        device_class = DEVICE_CLASS_TEMPERATURE,
        state_class = STATE_CLASS_MEASUREMENT,
        unit_of_measurement = UNIT_CELSIUS,
        accuracy_decimals = 1,
        icon = "mdi:sun-thermometer"
    ),
    cv.Optional(CONF_HOT_WATER_TEMPERATURE): sensor.sensor_schema(
        device_class = DEVICE_CLASS_TEMPERATURE,
        state_class = STATE_CLASS_MEASUREMENT,
        unit_of_measurement = UNIT_CELSIUS,
        accuracy_decimals = 1,
        icon = "mdi:coolant-temperature"
    ),
    cv.Optional(CONF_HOT_WATER_SET_TEMPERATURE): sensor.sensor_schema(
        device_class = DEVICE_CLASS_TEMPERATURE,
        state_class = STATE_CLASS_MEASUREMENT,
        unit_of_measurement = UNIT_CELSIUS,
        accuracy_decimals = 1,
        icon = "mdi:coolant-temperature"
    ),
    cv.Optional(CONF_HEAT_SOURCE_INPUT_TEMPERATURE): sensor.sensor_schema(
        device_class = DEVICE_CLASS_TEMPERATURE,
        state_class = STATE_CLASS_MEASUREMENT,
        unit_of_measurement = UNIT_CELSIUS,
        accuracy_decimals = 1,
        icon = "mdi:thermometer-chevron-up"
    ),
    cv.Optional(CONF_HEAT_SOURCE_OUTPUT_TEMPERATURE): sensor.sensor_schema(
        device_class = DEVICE_CLASS_TEMPERATURE,
        state_class = STATE_CLASS_MEASUREMENT,
        unit_of_measurement = UNIT_CELSIUS,
        accuracy_decimals = 1,
        icon = "mdi:thermometer-chevron-down"
    ),
    cv.Optional(CONF_MIXED_CIRCUIT_1_TEMPERATURE): sensor.sensor_schema(
        device_class = DEVICE_CLASS_TEMPERATURE,
        state_class = STATE_CLASS_MEASUREMENT,
        unit_of_measurement = UNIT_CELSIUS,
        accuracy_decimals = 1
    ),
    cv.Optional(CONF_MIXED_CIRCUIT_1_SET_TEMPERATURE): sensor.sensor_schema(
        device_class = DEVICE_CLASS_TEMPERATURE,
        state_class = STATE_CLASS_MEASUREMENT,
        unit_of_measurement = UNIT_CELSIUS,
        accuracy_decimals = 1
    ),
    cv.Optional(CONF_REMOTE_ADJUSTER_TEMPERATURE): sensor.sensor_schema(
        device_class = DEVICE_CLASS_TEMPERATURE,
        state_class = STATE_CLASS_MEASUREMENT,
        unit_of_measurement = UNIT_CELSIUS,
        accuracy_decimals = 1
    ),
    cv.Optional(CONF_HEATING_CURVE_OFFSET): sensor.sensor_schema(
        device_class = DEVICE_CLASS_TEMPERATURE,
        state_class = STATE_CLASS_MEASUREMENT,
        unit_of_measurement = UNIT_CELSIUS,
        accuracy_decimals = 1,
        icon = "mdi:plus-minus-variant"
    ),
    cv.Optional(CONF_HEATING_CURVE_ENDPOINT): sensor.sensor_schema(
        device_class = DEVICE_CLASS_TEMPERATURE,
        state_class = STATE_CLASS_MEASUREMENT,
        unit_of_measurement = UNIT_CELSIUS,
        accuracy_decimals = 1,
        icon = "mdi:arrow-collapse-right"
    ),
    cv.Optional(CONF_HEATING_CURVE_PARALLEL_SHIFT): sensor.sensor_schema(
        device_class = DEVICE_CLASS_TEMPERATURE,
        state_class = STATE_CLASS_MEASUREMENT,
        unit_of_measurement = UNIT_CELSIUS,
        accuracy_decimals = 1,
        icon = "mdi:arrow-expand"
    ),
    cv.Optional(CONF_HEATING_CURVE_NIGHT_SETBACK): sensor.sensor_schema(
        device_class = DEVICE_CLASS_TEMPERATURE,
        state_class = STATE_CLASS_MEASUREMENT,
        unit_of_measurement = UNIT_CELSIUS,
        accuracy_decimals = 1,
        icon = "mdi:moon-waning-crescent"
    ),
    cv.Optional(CONF_HEATING_CURVE_CONSTANT_RETURN): sensor.sensor_schema(
        device_class = DEVICE_CLASS_TEMPERATURE,
        state_class = STATE_CLASS_MEASUREMENT,
        unit_of_measurement = UNIT_CELSIUS,
        accuracy_decimals = 1,
        icon = "mdi:format-vertical-align-center"
    ),
    cv.Optional(CONF_HEATING_CURVE_MC1_END_POINT): sensor.sensor_schema(
        device_class = DEVICE_CLASS_TEMPERATURE,
        state_class = STATE_CLASS_MEASUREMENT,
        unit_of_measurement = UNIT_CELSIUS,
        accuracy_decimals = 1,
        icon = "mdi:arrow-collapse-right"
    ),
    cv.Optional(CONF_HEATING_CURVE_MC1_PARALLEL_SHIFT): sensor.sensor_schema(
        device_class = DEVICE_CLASS_TEMPERATURE,
        state_class = STATE_CLASS_MEASUREMENT,
        unit_of_measurement = UNIT_CELSIUS,
        accuracy_decimals = 1,
        icon = "mdi:arrow-expand"
    ),
    cv.Optional(CONF_HEATING_CURVE_MC1_NIGHT_SETBACK): sensor.sensor_schema(
        device_class = DEVICE_CLASS_TEMPERATURE,
        state_class = STATE_CLASS_MEASUREMENT,
        unit_of_measurement = UNIT_CELSIUS,
        accuracy_decimals = 1,
        icon = "mdi:moon-waning-crescent"
    ),
    cv.Optional(CONF_HEATING_CURVE_MC1_CONSTANT_FLOW): sensor.sensor_schema(
        device_class = DEVICE_CLASS_TEMPERATURE,
        state_class = STATE_CLASS_MEASUREMENT,
        unit_of_measurement = UNIT_CELSIUS,
        accuracy_decimals = 1,
        icon = "mdi:format-vertical-align-center"
    ),
    cv.Optional(CONF_IMPULSES_COMPRESSOR_1): sensor.sensor_schema(
        state_class = STATE_CLASS_TOTAL_INCREASING,
        accuracy_decimals = 0,
        icon = "mdi:counter"
    ),
    cv.Optional(CONF_IMPULSES_COMPRESSOR_2): sensor.sensor_schema(
        state_class = STATE_CLASS_TOTAL_INCREASING,
        accuracy_decimals = 0,
        icon = "mdi:counter"
    )
})


async def to_code(config):
    cmp = await cg.get_variable(config[CONF_LUXTRONIK_ID])

    for key, conf in config.items():
        if isinstance(conf, dict):
            id = conf[CONF_ID]
            if id and id.type == sensor.Sensor:
                sens = await sensor.new_sensor(conf)
                cg.add(getattr(cmp, f"set_sensor_{key}")(sens))
