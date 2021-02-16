import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, esp32_ble_tracker
from esphome.const import CONF_MAC_ADDRESS, UNIT_STEPS, ICON_SHOE_PRINT, CONF_ID

AUTO_LOAD = ['xiaomi_ble']
CODEOWNERS = ['@jesserockz']
DEPENDENCIES = ['esp32_ble_tracker']

CONF_STEPS = 'steps'

mi_band_4_ns = cg.esphome_ns.namespace('mi_band_4')
MiBand4 = mi_band_4_ns.class_(
    'MiBand4', esp32_ble_tracker.ESPBTDeviceListener, cg.Component)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(MiBand4),
    cv.Required(CONF_MAC_ADDRESS): cv.mac_address,
    cv.Optional(CONF_STEPS): sensor.sensor_schema(UNIT_STEPS, ICON_SHOE_PRINT, 0),
}).extend(esp32_ble_tracker.ESP_BLE_DEVICE_SCHEMA).extend(cv.COMPONENT_SCHEMA)


def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)
    yield esp32_ble_tracker.register_ble_device(var, config)

    cg.add(var.set_address(config[CONF_MAC_ADDRESS].as_hex))

    if CONF_STEPS in config:
        sens = yield sensor.new_sensor(config[CONF_STEPS])
        cg.add(var.set_steps(sens))
