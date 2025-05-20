# Copyright (c) 2025 Jens-Uwe Rossbach
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

from esphome            import automation
from esphome.components import number, switch, sensor
from esphome.const      import (
    ENTITY_CATEGORY_CONFIG,
    DEVICE_CLASS_TEMPERATURE,
    UNIT_CELSIUS,
    CONF_MAX_VALUE,
    CONF_MIN_VALUE,
    CONF_SET_ACTION,
    CONF_STEP
)
from ..                 import (
    luxtronik_ns,
    CONF_DATA_SOURCE,
    CONF_EDIT_MODE_SWITCH
)


LuxtronikNumber = luxtronik_ns.class_("LuxtronikNumber", number.Number)

CONFIG_SCHEMA = number.number_schema(
        LuxtronikNumber,
        entity_category=ENTITY_CATEGORY_CONFIG,
        device_class=DEVICE_CLASS_TEMPERATURE,
        unit_of_measurement=UNIT_CELSIUS).extend(
{
    cv.Required(CONF_MIN_VALUE): cv.float_,
    cv.Required(CONF_MAX_VALUE): cv.float_,
    cv.Optional(CONF_STEP, default = 0.5): cv.positive_float,
    cv.Optional(CONF_SET_ACTION): automation.validate_automation(single = True),
    cv.Optional(CONF_DATA_SOURCE): cv.use_id(sensor.Sensor),
    cv.Optional(CONF_EDIT_MODE_SWITCH): cv.use_id(switch.Switch)
})


async def to_code(config):
    num = await number.new_number(
                        config,
                        min_value = config[CONF_MIN_VALUE],
                        max_value = config[CONF_MAX_VALUE],
                        step = config[CONF_STEP])

    if CONF_DATA_SOURCE in config:
        sns = await cg.get_variable(config[CONF_DATA_SOURCE])
        cg.add(num.set_source_sensor(sns))

    if CONF_EDIT_MODE_SWITCH in config:
        swt = await cg.get_variable(config[CONF_EDIT_MODE_SWITCH])
        cg.add(num.set_edit_mode_switch(swt))

    if CONF_SET_ACTION in config:
        await automation.build_automation(
            num.get_set_trigger(), [(float, "x")], config[CONF_SET_ACTION])
