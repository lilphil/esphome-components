#include "I2CMotorDriver.h"

#include "esphome/core/helpers.h"
#include "esphome/core/log.h"
#include "LOLIN_I2C_MOTOR.h"

const int max_retries = 10;

namespace esphome {
namespace i2cmotordriver {

  static const char* const TAG = "I2CMotorDriver";
  LOLIN_I2C_MOTOR motor;

  void I2CMotorDriver::setup() {
    ESP_LOGCONFIG(TAG, "Setting up I2C motor driver component...");
    const int max_retries = 10;
    
    for (int attempt = 1; attempt <= max_retries; attempt++) {
      motor.getInfo();
      if (motor.PRODUCT_ID == PRODUCT_ID_I2C_MOTOR) {
        ESP_LOGCONFIG(TAG, "I2C motor driver device found (PRODUCT_ID: 0x%02X, VERSION: 0x%02X)", motor.PRODUCT_ID, motor.VERSION);
        return;
      }
      ESP_LOGW(TAG, "Attempt %d/%d: I2C motor driver not found, retrying...", attempt, max_retries);
      delay(100);
    }

    ESP_LOGE(TAG, "I2C motor driver device not found after %d attempts", max_retries);
  }

  void I2CMotorDriver::set_duty(const std::string &channel, uint8_t duty) {
    motor.changeDuty(parse_motor_channel(channel), duty);
    ESP_LOGD(TAG, "Set duty cycle: channel=%s, duty=%d", channel.c_str(), duty);
  }
  
  void I2CMotorDriver::set_freq(const std::string &channel, uint16_t frequency) {
    motor.changeFreq(parse_motor_channel(channel), frequency);
    ESP_LOGD(TAG, "Set frequency: channel=%s, frequency=%d Hz", channel.c_str(), frequency);
  }

  void I2CMotorDriver::set_status(const std::string &channel, const std::string &status) {
    MOTOR_CHANNEL parsed_channel = parse_motor_channel(channel);
    MOTOR_STATUS parsed_status = parse_motor_status(status);
    if (parsed_channel == MOTOR_CHANNEL::MOTOR_CH_A || parsed_channel == MOTOR_CHANNEL::MOTOR_CH_BOTH) {
      _status_a = parsed_status;
    }
    if (parsed_channel == MOTOR_CHANNEL::MOTOR_CH_B || parsed_channel == MOTOR_CHANNEL::MOTOR_CH_BOTH) {
      _status_b = parsed_status;
    }
    motor.changeStatus(parsed_channel, parsed_status);
    ESP_LOGD(TAG, "Set status: channel=%s, status=%s", channel.c_str(), status.c_str());
  }

  std::string I2CMotorDriver::get_status(const std::string &channel) {
    MOTOR_STATUS status;
    MOTOR_CHANNEL parsed_channel = parse_motor_channel(channel);

    if (parsed_channel == MOTOR_CHANNEL::MOTOR_CH_A) {
      status = _status_a;
    }
    else if (parsed_channel == MOTOR_CHANNEL::MOTOR_CH_B) {
      status = _status_b;
    }
    else {
      return "UNKNOWN";
    }

    switch (status) {
      case MOTOR_STATUS::MOTOR_STATUS_STOP:
        return "STOP";
	break;
      case MOTOR_STATUS::MOTOR_STATUS_CCW:
        return "CCW";
	break;
      case MOTOR_STATUS::MOTOR_STATUS_CW:
        return "CW";
	break;
      case MOTOR_STATUS::MOTOR_STATUS_SHORT_BRAKE:
        return "SHORT_BRAKE";
	break;
      case MOTOR_STATUS::MOTOR_STATUS_STANDBY:
        return "STANDBY";
	break;
      default:
        return "UNKNOWN";
	break;
    }
  }

  MOTOR_STATUS I2CMotorDriver::parse_motor_status(const std::string &status) {
    if (status == "STOP") return MOTOR_STATUS::MOTOR_STATUS_STOP;
    if (status == "CCW") return MOTOR_STATUS::MOTOR_STATUS_CCW;
    if (status == "CW") return MOTOR_STATUS::MOTOR_STATUS_CW;
    if (status == "SHORT_BRAKE") return MOTOR_STATUS::MOTOR_STATUS_SHORT_BRAKE;
    if (status == "STANDBY") return MOTOR_STATUS::MOTOR_STATUS_STANDBY;
    return MOTOR_STATUS::MOTOR_STATUS_STOP;
  }

  MOTOR_CHANNEL I2CMotorDriver::parse_motor_channel(const std::string &channel) {
    if (channel == "A") return MOTOR_CHANNEL::MOTOR_CH_A;
    if (channel == "B") return MOTOR_CHANNEL::MOTOR_CH_B;
    if (channel == "BOTH") return MOTOR_CHANNEL::MOTOR_CH_BOTH;
    
    ESP_LOGE(TAG, "Invalid motor channel: %s", channel.c_str());
    return MOTOR_CHANNEL::MOTOR_CH_BOTH;
  }

}
}
