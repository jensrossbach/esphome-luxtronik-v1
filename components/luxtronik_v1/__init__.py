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

from esphome            import automation
from esphome.components import uart
from esphome.const      import (
    CONF_ID,
    CONF_UART_ID,
    CONF_VALUE,
    CONF_MODE
)


CODEOWNERS = ["@jensrossbach"]
DEPENDENCIES = ["uart"]

CONF_LUXTRONIK_ID     = "luxtronik_id"
CONF_REQUEST_DELAY    = "request_delay"
CONF_RESPONSE_TIMEOUT = "response_timeout"
CONF_MAX_RETRIES      = "max_retries"
CONF_INCLUDE_DATASETS = "include_datasets"

CONF_HC_RETURN_OFFSET   = "hc_return_offset"
CONF_HC_ENDPOINT        = "hc_endpoint"
CONF_HC_PARALLEL_SHIFT  = "hc_parallel_shift"
CONF_HC_NIGHT_SETBACK   = "hc_night_setback"
CONF_HC_CONST_RETURN    = "hc_const_return"
CONF_MC1_ENDPOINT       = "mc1_endpoint"
CONF_MC1_PARALLEL_SHIFT = "mc1_parallel_shift"
CONF_MC1_NIGHT_SETBACK  = "mc1_night_setback"
CONF_MC1_CONST_FLOW     = "mc1_const_flow"

CONF_START_1_HOUR   = "start_1_hour"
CONF_START_1_MINUTE = "start_1_minute"
CONF_END_1_HOUR     = "end_1_hour"
CONF_END_1_MINUTE   = "end_1_minute"
CONF_START_2_HOUR   = "start_2_hour"
CONF_START_2_MINUTE = "start_2_minute"
CONF_END_2_HOUR     = "end_2_hour"
CONF_END_2_MINUTE   = "end_2_minute"

CONF_DATA_SOURCE      = "data_source"
CONF_EDIT_MODE_SWITCH = "edit_mode_switch"

TYPE_HEATING_MODE   = 0
TYPE_HOT_WATER_MODE = 1

luxtronik_ns = cg.esphome_ns.namespace("luxtronik_v1")
Luxtronik = luxtronik_ns.class_("Luxtronik", cg.PollingComponent)

RequestDatasetsAction = luxtronik_ns.class_("RequestDatasetsAction", automation.Action)
SetOperationalModeAction = luxtronik_ns.class_("SetOperationalModeAction", automation.Action)
SetHotWaterSetTemperatureAction = luxtronik_ns.class_("SetHotWaterSetTemperatureAction", automation.Action)
SetHotWaterOffTimesWeekAction = luxtronik_ns.class_("SetHotWaterOffTimesWeekAction", automation.Action)
SetHeatingCurvesAction = luxtronik_ns.class_("SetHeatingCurvesAction", automation.Action)

def unique_list(value):
    if len(value) != len(set(value)):
        raise cv.Invalid("All values must be unique.")
    return value

CONFIG_SCHEMA = cv.Schema(
{
    cv.GenerateID(): cv.declare_id(Luxtronik),
    cv.Optional(CONF_REQUEST_DELAY, default = 0): cv.int_range(min = 0, max = 2000),
    cv.Optional(CONF_RESPONSE_TIMEOUT, default = 2000): cv.int_range(min = 500, max = 5000),
    cv.Optional(CONF_MAX_RETRIES, default = 5): cv.int_range(min = 0, max = 15),
    cv.Optional(
        CONF_INCLUDE_DATASETS,
        default = [1100, 1200, 1300, 1450, 1500, 1600, 1700, 3200, 3400, 3405, 3505]):
        cv.All(
            cv.ensure_list(cv.one_of(1100, 1200, 1300, 1450, 1500, 1600, 1700, 3200, 3400, 3405, 3505)),
            cv.Length(min = 1),
            unique_list)
}).extend(uart.UART_DEVICE_SCHEMA).extend(cv.polling_component_schema("60s"))

SET_OPERATIONAL_MODE_SCHEMA = cv.Schema(
{
    cv.Required(CONF_ID): cv.use_id(Luxtronik),
    cv.Required(CONF_MODE): cv.templatable(cv.int_range(min = 0, max = 4))
})

async def to_code(config):
    uart_cmp = await cg.get_variable(config[CONF_UART_ID])
    cmp = cg.new_Pvariable(
                config[CONF_ID],
                uart_cmp,
                config[CONF_REQUEST_DELAY],
                config[CONF_RESPONSE_TIMEOUT],
                config[CONF_MAX_RETRIES])
    await cg.register_component(cmp, config)

    for ds in config[CONF_INCLUDE_DATASETS]:
        cg.add(cmp.add_dataset(str(ds)))

@automation.register_action(
    "luxtronik_v1.request_datasets",
    RequestDatasetsAction,
    cv.Schema(
    {
        cv.Required(CONF_ID): cv.use_id(Luxtronik)
    }))
async def request_datasets_action_to_code(config, action_id, template_arg, args):
    parent = await cg.get_variable(config[CONF_ID])
    act = cg.new_Pvariable(action_id, template_arg, parent)

    return act

@automation.register_action(
    "luxtronik_v1.set_heating_mode",
    SetOperationalModeAction,
    SET_OPERATIONAL_MODE_SCHEMA)
async def set_heating_mode_action_to_code(config, action_id, template_arg, args):
    parent = await cg.get_variable(config[CONF_ID])
    act = cg.new_Pvariable(action_id, template_arg, parent, TYPE_HEATING_MODE)

    tmpl = await cg.templatable(config[CONF_MODE], args, int)
    cg.add(act.set_operational_mode_value(tmpl))

    return act

@automation.register_action(
    "luxtronik_v1.set_hot_water_mode",
    SetOperationalModeAction,
    SET_OPERATIONAL_MODE_SCHEMA)
async def set_hot_water_mode_action_to_code(config, action_id, template_arg, args):
    parent = await cg.get_variable(config[CONF_ID])
    act = cg.new_Pvariable(action_id, template_arg, parent, TYPE_HOT_WATER_MODE)

    tmpl = await cg.templatable(config[CONF_MODE], args, int)
    cg.add(act.set_operational_mode_value(tmpl))

    return act

@automation.register_action(
    "luxtronik_v1.set_hot_water_set_temperature",
    SetHotWaterSetTemperatureAction,
    cv.Schema(
    {
        cv.Required(CONF_ID): cv.use_id(Luxtronik),
        cv.Required(CONF_VALUE): cv.templatable(cv.float_range(min = 30.0, max = 65.0))
    }))
async def set_hot_water_set_temperature_action_to_code(config, action_id, template_arg, args):
    parent = await cg.get_variable(config[CONF_ID])
    act = cg.new_Pvariable(action_id, template_arg, parent)

    tmpl = await cg.templatable(config[CONF_VALUE], args, float)
    cg.add(act.set_hot_water_set_temperature_value(tmpl))

    return act

@automation.register_action(
    "luxtronik_v1.set_hot_water_off_times_week",
    SetHotWaterOffTimesWeekAction,
    cv.Schema(
    {
        cv.Required(CONF_ID): cv.use_id(Luxtronik),
        cv.Required(CONF_START_1_HOUR): cv.templatable(cv.int_range(min = 0, max = 23)),
        cv.Required(CONF_START_1_MINUTE): cv.templatable(cv.int_range(min = 0, max = 59)),
        cv.Required(CONF_END_1_HOUR): cv.templatable(cv.int_range(min = 0, max = 23)),
        cv.Required(CONF_END_1_MINUTE): cv.templatable(cv.int_range(min = 0, max = 59)),
        cv.Required(CONF_START_2_HOUR): cv.templatable(cv.int_range(min = 0, max = 23)),
        cv.Required(CONF_START_2_MINUTE): cv.templatable(cv.int_range(min = 0, max = 59)),
        cv.Required(CONF_END_2_HOUR): cv.templatable(cv.int_range(min = 0, max = 23)),
        cv.Required(CONF_END_2_MINUTE): cv.templatable(cv.int_range(min = 0, max = 59))
    }))
async def set_heating_curves_action_to_code(config, action_id, template_arg, args):
    parent = await cg.get_variable(config[CONF_ID])
    act = cg.new_Pvariable(action_id, template_arg, parent)

    tmpl = await cg.templatable(config[CONF_START_1_HOUR], args, int)
    cg.add(act.set_start_1_hour_value(tmpl))
    tmpl = await cg.templatable(config[CONF_START_1_MINUTE], args, int)
    cg.add(act.set_start_1_minute_value(tmpl))
    tmpl = await cg.templatable(config[CONF_END_1_HOUR], args, int)
    cg.add(act.set_end_1_hour_value(tmpl))
    tmpl = await cg.templatable(config[CONF_END_1_MINUTE], args, int)
    cg.add(act.set_end_1_minute_value(tmpl))
    tmpl = await cg.templatable(config[CONF_START_2_HOUR], args, int)
    cg.add(act.set_start_2_hour_value(tmpl))
    tmpl = await cg.templatable(config[CONF_START_2_MINUTE], args, int)
    cg.add(act.set_start_2_minute_value(tmpl))
    tmpl = await cg.templatable(config[CONF_END_2_HOUR], args, int)
    cg.add(act.set_end_2_hour_value(tmpl))
    tmpl = await cg.templatable(config[CONF_END_2_MINUTE], args, int)
    cg.add(act.set_end_2_minute_value(tmpl))

    return act

@automation.register_action(
    "luxtronik_v1.set_heating_curves",
    SetHeatingCurvesAction,
    cv.Schema(
    {
        cv.Required(CONF_ID): cv.use_id(Luxtronik),
        cv.Optional(CONF_HC_RETURN_OFFSET): cv.templatable(cv.float_range(min = -5.0, max = 5.0)),
        cv.Optional(CONF_HC_ENDPOINT): cv.templatable(cv.positive_float),
        cv.Optional(CONF_HC_PARALLEL_SHIFT): cv.templatable(cv.positive_float),
        cv.Optional(CONF_HC_NIGHT_SETBACK): cv.templatable(cv.float_range(max = 0.0)),
        cv.Optional(CONF_HC_CONST_RETURN): cv.templatable(cv.positive_float),
        cv.Optional(CONF_MC1_ENDPOINT): cv.templatable(cv.positive_float),
        cv.Optional(CONF_MC1_PARALLEL_SHIFT): cv.templatable(cv.positive_float),
        cv.Optional(CONF_MC1_NIGHT_SETBACK): cv.templatable(cv.float_range(max = 0.0)),
        cv.Optional(CONF_MC1_CONST_FLOW): cv.templatable(cv.positive_float)
    }))
async def set_heating_curves_action_to_code(config, action_id, template_arg, args):
    parent = await cg.get_variable(config[CONF_ID])
    act = cg.new_Pvariable(action_id, template_arg, parent)

    if CONF_HC_RETURN_OFFSET in config:
        tmpl = await cg.templatable(config[CONF_HC_RETURN_OFFSET], args, float)
        cg.add(act.set_hc_return_offset_value(tmpl))
    if CONF_HC_ENDPOINT in config:
        tmpl = await cg.templatable(config[CONF_HC_ENDPOINT], args, float)
        cg.add(act.set_hc_endpoint_value(tmpl))
    if CONF_HC_PARALLEL_SHIFT in config:
        tmpl = await cg.templatable(config[CONF_HC_PARALLEL_SHIFT], args, float)
        cg.add(act.set_hc_parallel_shift_value(tmpl))
    if CONF_HC_NIGHT_SETBACK in config:
        tmpl = await cg.templatable(config[CONF_HC_NIGHT_SETBACK], args, float)
        cg.add(act.set_hc_night_setback_value(tmpl))
    if CONF_HC_CONST_RETURN in config:
        tmpl = await cg.templatable(config[CONF_HC_CONST_RETURN], args, float)
        cg.add(act.set_hc_constant_return_value(tmpl))
    if CONF_MC1_ENDPOINT in config:
        tmpl = await cg.templatable(config[CONF_MC1_ENDPOINT], args, float)
        cg.add(act.set_mc1_endpoint_value(tmpl))
    if CONF_MC1_PARALLEL_SHIFT in config:
        tmpl = await cg.templatable(config[CONF_MC1_PARALLEL_SHIFT], args, float)
        cg.add(act.set_mc1_parallel_shift_value(tmpl))
    if CONF_MC1_NIGHT_SETBACK in config:
        tmpl = await cg.templatable(config[CONF_MC1_NIGHT_SETBACK], args, float)
        cg.add(act.set_mc1_night_setback_value(tmpl))
    if CONF_MC1_CONST_FLOW in config:
        tmpl = await cg.templatable(config[CONF_MC1_CONST_FLOW], args, float)
        cg.add(act.set_mc1_constant_flow_value(tmpl))

    return act
