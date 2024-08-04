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

from esphome.components import uart
from esphome.const      import (
    CONF_ID,
    CONF_UART_ID
)


CONF_LUXTRONIK_ID = "luxtronik_id"

luxtronik_ns = cg.esphome_ns.namespace("luxtronik_v1")
Luxtronik = luxtronik_ns.class_("Luxtronik", cg.PollingComponent)

CODEOWNERS = ["@jens_rossbach"]
DEPENDENCIES = ["uart"]

CONFIG_SCHEMA = cv.Schema(
{
    cv.GenerateID(): cv.declare_id(Luxtronik)
}).extend(uart.UART_DEVICE_SCHEMA).extend(cv.polling_component_schema("60s"))


async def to_code(config):
    uart_cmp = await cg.get_variable(config[CONF_UART_ID])
    cmp = cg.new_Pvariable(config[CONF_ID], uart_cmp)
    await cg.register_component(cmp, config)
