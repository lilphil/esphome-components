#pragma once

#include "esphome/core/component.h"
#include "LOLIN_I2C_MOTOR.h"

namespace esphome {
namespace lolini2cmotor {

class LolinI2CMotor : public Component {
  public:
    float get_setup_priority() const override {
      // Setting a custom priority for this component to be initialized after hardware setup
      return setup_priority::IO;
    }

    void setup() override;
    void set_duty(const std::string &channel, uint8_t duty);
    void set_freq(const std::string &channel, uint16_t frequency);
    void set_status(const std::string &channel, const std::string &status);

  private:
    MOTOR_STATUS parse_motor_status(const std::string &status);
    MOTOR_CHANNEL parse_motor_channel(const std::string &channel);
};

}
}
