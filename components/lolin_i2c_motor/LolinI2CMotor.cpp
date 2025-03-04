#include "LolinI2CMotor.h"

#include "esphome/core/helpers.h"
#include "esphome/core/log.h"
#include "LOLIN_I2C_MOTOR.h"

const int max_retries = 10;

namespace esphome {
namespace lolini2cmotor {

  static const char* const TAG = "LolinI2CMotor";
  LOLIN_I2C_MOTOR motor;

  void LolinI2CMotor::setup() {
    ESP_LOGCONFIG(TAG, "Setting up Lolin I2C motor component...");
    const int max_retries = 10;
    
    for (int attempt = 1; attempt <= max_retries; attempt++) {
      motor.getInfo();
      if (motor.PRODUCT_ID == PRODUCT_ID_I2C_MOTOR) {
        ESP_LOGCONFIG(TAG, "Lolin I2C motor device found (PRODUCT_ID: 0x%02X, VERSION: 0x%02X)", motor.PRODUCT_ID, motor.VERSION);
        return;
      }
      ESP_LOGW(TAG, "Attempt %d/%d: Lolin I2C motor not found, retrying...", attempt, max_retries);
      delay(100);
    }

    ESP_LOGE(TAG, "Lolin I2C motor device not found after %d attempts", max_retries);
  }

  void LolinI2CMotor::set_duty(const std::string &channel, uint8_t duty) {
    motor.changeDuty(parse_motor_channel(channel), duty);
    ESP_LOGD(TAG, "Set duty cycle: channel=%s, duty=%d", channel.c_str(), duty);
  }

  void LolinI2CMotor::set_freq(const std::string &channel, uint16_t frequency) {
    motor.changeFreq(parse_motor_channel(channel), frequency);
    ESP_LOGD(TAG, "Set frequency: channel=%s, frequency=%d Hz", channel.c_str(), frequency);
  }

  void LolinI2CMotor::set_status(const std::string &channel, const std::string &status) {
    motor.changeStatus(parse_motor_channel(channel), parse_motor_status(status));
    ESP_LOGD(TAG, "Set status: channel=%s, status=%s", channel.c_str(), status.c_str());
  }

  MOTOR_STATUS LolinI2CMotor::parse_motor_status(const std::string &status) {
    if (status == "STOP") return MOTOR_STATUS::MOTOR_STATUS_STOP;
    if (status == "CCW") return MOTOR_STATUS::MOTOR_STATUS_CCW;
    if (status == "CW") return MOTOR_STATUS::MOTOR_STATUS_CW;
    if (status == "SHORT_BRAKE") return MOTOR_STATUS::MOTOR_STATUS_SHORT_BRAKE;
    if (status == "STANDBY") return MOTOR_STATUS::MOTOR_STATUS_STANDBY;
    return MOTOR_STATUS::MOTOR_STATUS_STOP;
  }

  MOTOR_CHANNEL LolinI2CMotor::parse_motor_channel(const std::string &channel) {
    if (channel == "A") return MOTOR_CHANNEL::MOTOR_CH_A;
    if (channel == "B") return MOTOR_CHANNEL::MOTOR_CH_B;
    if (channel == "BOTH") return MOTOR_CHANNEL::MOTOR_CH_BOTH;
    
    ESP_LOGE(TAG, "Invalid motor channel: %s", channel.c_str());
    return MOTOR_CHANNEL::MOTOR_CH_BOTH;
  }

}
}
