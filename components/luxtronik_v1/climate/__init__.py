# Copyright (c) 2024-2026 Jens-Uwe Rossbach
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

from esphome.components import climate
from esphome.const      import (
    CONF_VISUAL,
    CONF_MIN_TEMPERATURE,
    CONF_MAX_TEMPERATURE
)
from ..                 import (
    Luxtronik,
    CONF_LUXTRONIK_ID
)

CODEOWNERS = ["@jensrossbach"]

luxtronik_ns = cg.esphome_ns.namespace("luxtronik_v1")
LuxtronikClimate = luxtronik_ns.class_("LuxtronikClimate", climate.Climate, cg.Component)

DEF_MIN_TEMP = 30.0
DEF_MAX_TEMP = 65.0

BASE_CLIMATE_SCHEMA = climate.climate_schema(LuxtronikClimate)
BASE_VISUAL_SCHEMA = BASE_CLIMATE_SCHEMA.schema[cv.Optional(CONF_VISUAL)]

CONFIG_SCHEMA = BASE_CLIMATE_SCHEMA.extend({
    cv.GenerateID(CONF_LUXTRONIK_ID): cv.use_id(Luxtronik),
    cv.Optional(CONF_VISUAL): BASE_VISUAL_SCHEMA.extend({
        cv.Optional(CONF_MIN_TEMPERATURE, default=DEF_MIN_TEMP): cv.float_range(min=DEF_MIN_TEMP, max=50.0),
        cv.Optional(CONF_MAX_TEMPERATURE, default=DEF_MAX_TEMP): cv.float_range(min=45.0, max=DEF_MAX_TEMP),
    })
}).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    cmp = await climate.new_climate(config)
    await cg.register_component(cmp, config)
    await cg.register_parented(cmp, config[CONF_LUXTRONIK_ID])
