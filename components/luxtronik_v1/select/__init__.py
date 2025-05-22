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
from esphome.components import select, text_sensor
from esphome.const      import (
    ENTITY_CATEGORY_CONFIG,
    CONF_OPTIONS,
    CONF_SET_ACTION
)
from ..                 import (
    luxtronik_ns,
    CONF_DATA_SOURCE
)


LuxtronikSelect = luxtronik_ns.class_("LuxtronikSelect", select.Select)

CONFIG_SCHEMA = select.select_schema(
        LuxtronikSelect,
        entity_category=ENTITY_CATEGORY_CONFIG).extend(
{
    cv.Required(CONF_OPTIONS): cv.All(cv.ensure_list(cv.string_strict), cv.Length(min = 1, max = 255)),
    cv.Optional(CONF_SET_ACTION): automation.validate_automation(single = True),
    cv.Optional(CONF_DATA_SOURCE): cv.use_id(text_sensor.TextSensor)
})


async def to_code(config):
        sel = await select.new_select(
                            config,
                            options = config[CONF_OPTIONS])

        if CONF_DATA_SOURCE in config:
            sns = await cg.get_variable(config[CONF_DATA_SOURCE])
            cg.add(sel.set_source_sensor(sns))

        if CONF_SET_ACTION in config:
            await automation.build_automation(
                sel.get_set_trigger(), [(cg.std_string, "x"), (cg.uint8, "i")], config[CONF_SET_ACTION])
