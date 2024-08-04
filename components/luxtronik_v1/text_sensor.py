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

from esphome.components import text_sensor
from esphome.const      import (
    CONF_ID,
    DEVICE_CLASS_TIMESTAMP
)
from .                  import (
    Luxtronik,
    CONF_LUXTRONIK_ID
)


CODEOWNERS = ["@jens_rossbach"]

CONF_DEVICE_TYPE         = "device_type"
CONF_FIRMWARE_VERSION    = "firmware_version"
CONF_BIVALENCE_LEVEL     = "bivalence_level"
CONF_OPERATIONAL_STATE   = "operational_state"
CONF_HEATING_MODE        = "heating_mode"
CONF_HOT_WATER_MODE      = "hot_water_mode"
CONF_OUTPUT_MIXER_1      = "mixer_1_state"
CONF_ERROR_1_CODE        = "error_1_code"
CONF_ERROR_1_TIME        = "error_1_time"
CONF_ERROR_2_CODE        = "error_2_code"
CONF_ERROR_2_TIME        = "error_2_time"
CONF_ERROR_3_CODE        = "error_3_code"
CONF_ERROR_3_TIME        = "error_3_time"
CONF_ERROR_4_CODE        = "error_4_code"
CONF_ERROR_4_TIME        = "error_4_time"
CONF_ERROR_5_CODE        = "error_5_code"
CONF_ERROR_5_TIME        = "error_5_time"
CONF_DEACTIVATION_1_CODE = "deactivation_1_code"
CONF_DEACTIVATION_1_TIME = "deactivation_1_time"
CONF_DEACTIVATION_2_CODE = "deactivation_2_code"
CONF_DEACTIVATION_2_TIME = "deactivation_2_time"
CONF_DEACTIVATION_3_CODE = "deactivation_3_code"
CONF_DEACTIVATION_3_TIME = "deactivation_3_time"
CONF_DEACTIVATION_4_CODE = "deactivation_4_code"
CONF_DEACTIVATION_4_TIME = "deactivation_4_time"
CONF_DEACTIVATION_5_CODE = "deactivation_5_code"
CONF_DEACTIVATION_5_TIME = "deactivation_5_time"

CONFIG_SCHEMA = cv.Schema(
{
    cv.GenerateID(CONF_LUXTRONIK_ID): cv.use_id(Luxtronik),
    cv.Optional(CONF_DEVICE_TYPE): text_sensor.text_sensor_schema(
        icon="mdi:card-account-details"
    ),
    cv.Optional(CONF_FIRMWARE_VERSION): text_sensor.text_sensor_schema(
        icon="mdi:tag"
    ),
    cv.Optional(CONF_BIVALENCE_LEVEL): text_sensor.text_sensor_schema(
        icon="mdi:order-bool-descending-variant"
    ),
    cv.Optional(CONF_OPERATIONAL_STATE): text_sensor.text_sensor_schema(
        icon="mdi:state-machine"
    ),
    cv.Optional(CONF_HEATING_MODE): text_sensor.text_sensor_schema(
        icon="mdi:radiator"
    ),
    cv.Optional(CONF_HOT_WATER_MODE): text_sensor.text_sensor_schema(
        icon="mdi:water-boiler"
    ),
    cv.Optional(CONF_OUTPUT_MIXER_1): text_sensor.text_sensor_schema(
        icon="mdi:merge"
    ),
    cv.Optional(CONF_ERROR_1_CODE): text_sensor.text_sensor_schema(
        icon="mdi:alert"
    ),
    cv.Optional(CONF_ERROR_1_TIME): text_sensor.text_sensor_schema(
        device_class=DEVICE_CLASS_TIMESTAMP
    ),
    cv.Optional(CONF_ERROR_2_CODE): text_sensor.text_sensor_schema(
        icon="mdi:alert"
    ),
    cv.Optional(CONF_ERROR_2_TIME): text_sensor.text_sensor_schema(
        device_class=DEVICE_CLASS_TIMESTAMP
    ),
    cv.Optional(CONF_ERROR_3_CODE): text_sensor.text_sensor_schema(
        icon="mdi:alert"
    ),
    cv.Optional(CONF_ERROR_3_TIME): text_sensor.text_sensor_schema(
        device_class=DEVICE_CLASS_TIMESTAMP
    ),
    cv.Optional(CONF_ERROR_4_CODE): text_sensor.text_sensor_schema(
        icon="mdi:alert"
    ),
    cv.Optional(CONF_ERROR_4_TIME): text_sensor.text_sensor_schema(
        device_class=DEVICE_CLASS_TIMESTAMP
    ),
    cv.Optional(CONF_ERROR_5_CODE): text_sensor.text_sensor_schema(
        icon="mdi:alert"
    ),
    cv.Optional(CONF_ERROR_5_TIME): text_sensor.text_sensor_schema(
        device_class=DEVICE_CLASS_TIMESTAMP
    ),
    cv.Optional(CONF_DEACTIVATION_1_CODE): text_sensor.text_sensor_schema(
        icon="mdi:information-slab-circle"
    ),
    cv.Optional(CONF_DEACTIVATION_1_TIME): text_sensor.text_sensor_schema(
        device_class=DEVICE_CLASS_TIMESTAMP
    ),
    cv.Optional(CONF_DEACTIVATION_2_CODE): text_sensor.text_sensor_schema(
        icon="mdi:information-slab-circle"
    ),
    cv.Optional(CONF_DEACTIVATION_2_TIME): text_sensor.text_sensor_schema(
        device_class=DEVICE_CLASS_TIMESTAMP
    ),
    cv.Optional(CONF_DEACTIVATION_3_CODE): text_sensor.text_sensor_schema(
        icon="mdi:information-slab-circle"
    ),
    cv.Optional(CONF_DEACTIVATION_3_TIME): text_sensor.text_sensor_schema(
        device_class=DEVICE_CLASS_TIMESTAMP
    ),
    cv.Optional(CONF_DEACTIVATION_4_CODE): text_sensor.text_sensor_schema(
        icon="mdi:information-slab-circle"
    ),
    cv.Optional(CONF_DEACTIVATION_4_TIME): text_sensor.text_sensor_schema(
        device_class=DEVICE_CLASS_TIMESTAMP
    ),
    cv.Optional(CONF_DEACTIVATION_5_CODE): text_sensor.text_sensor_schema(
        icon="mdi:information-slab-circle"
    ),
    cv.Optional(CONF_DEACTIVATION_5_TIME): text_sensor.text_sensor_schema(
        device_class=DEVICE_CLASS_TIMESTAMP
    )
})


async def to_code(config):
    cmp = await cg.get_variable(config[CONF_LUXTRONIK_ID])

    for key, conf in config.items():
        if isinstance(conf, dict):
            id = conf[CONF_ID]
            if id and id.type == text_sensor.TextSensor:
                sens = await text_sensor.new_text_sensor(conf)
                cg.add(getattr(cmp, f"set_sensor_{key}")(sens))
