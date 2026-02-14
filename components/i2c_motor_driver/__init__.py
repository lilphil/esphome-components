import esphome.config_validation as cv
import esphome.codegen as cg

CODEOWNERS = ["@lilphil"]
DEPENDENCIES = [ ]
AUTO_LOAD = [ ]
MULTI_CONF = True

# C++ namespace
ns = cg.esphome_ns.namespace("i2cmotordriver")
I2CMotorDriver = ns.class_("I2CMotorDriver", cg.Component)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(I2CMotorDriver),
    # Schema definition, containing the options available for the component
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config["id"])
    await cg.register_component(var, config)
