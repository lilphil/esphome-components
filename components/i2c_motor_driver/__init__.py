import esphome.config_validation as cv
import esphome.codegen as cg

CODEOWNERS = ["@lilphil"]
DEPENDENCIES = []
AUTO_LOAD = []
MULTI_CONF = True

# C++ namespace
ns = cg.esphome_ns.namespace("i2cmotordriver")
I2CMotorDriver = ns.class_("I2CMotorDriver", cg.Component)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(I2CMotorDriver),
    cv.Required("driver"): cv.one_of("lolin_v2", "wemos_v1", lower=True),
    cv.Optional("i2c_address", default=0x30): cv.hex_int_range(1, 127),
    cv.Optional("stby_pin"): cv.positive_int,
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config["id"])
    await cg.register_component(var, config)
    cg.add(var.set_driver(config["driver"]))
    cg.add(var.set_i2c_address(config["i2c_address"]))
    if "stby_pin" in config:
        cg.add(var.set_stby_pin(config["stby_pin"]))
