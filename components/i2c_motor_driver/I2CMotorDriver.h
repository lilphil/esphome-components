#pragma once

#include "esphome/core/component.h"
#include "LOLIN_I2C_MOTOR.h"

class Motor;

namespace esphome {
namespace i2cmotordriver {

class I2CMotorDriver : public Component {
 public:
  float get_setup_priority() const override {
    return setup_priority::IO;
  }

  void setup() override;

  void set_driver(const std::string &driver) { driver_type_ = driver; }
  void set_i2c_address(uint8_t address) { i2c_address_ = address; }
  void set_stby_pin(int8_t pin) { stby_pin_ = pin; }

  void set_duty(const std::string &channel, uint8_t duty);
  void set_freq(const std::string &channel, uint16_t frequency);
  void set_status(const std::string &channel, const std::string &status);

  std::string get_status(const std::string &channel);

 private:
  MOTOR_STATUS parse_motor_status(const std::string &status);
  MOTOR_CHANNEL parse_motor_channel(const std::string &channel);
  uint8_t status_to_wemos_dir(const std::string &status);
  uint8_t status_to_wemos_dir(MOTOR_STATUS st);

  std::string driver_type_;
  uint8_t i2c_address_ = 0x30;
  int8_t stby_pin_ = -1;

  LOLIN_I2C_MOTOR *lolin_motor_ = nullptr;
  Motor *wemos_motor_a_ = nullptr;
  Motor *wemos_motor_b_ = nullptr;

  MOTOR_STATUS _status_a = MOTOR_STATUS_STOP;
  MOTOR_STATUS _status_b = MOTOR_STATUS_STOP;
};

}  // namespace i2cmotordriver
}  // namespace esphome
