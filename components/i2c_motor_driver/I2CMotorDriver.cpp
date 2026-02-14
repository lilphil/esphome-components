#include "I2CMotorDriver.h"

#include "esphome/core/helpers.h"
#include "esphome/core/log.h"
#include "LOLIN_I2C_MOTOR.h"
#include "WEMOS_Motor.h"

namespace esphome {
namespace i2cmotordriver {

static const char *const TAG = "I2CMotorDriver";

static constexpr uint32_t WEMOS_DEFAULT_FREQ_HZ = 1000;
static constexpr int max_retries = 10;

void I2CMotorDriver::setup() {
  ESP_LOGCONFIG(TAG, "Setting up I2C motor driver component...");

  if (driver_type_ == "lolin_v2") {
    lolin_motor_ = new LOLIN_I2C_MOTOR(i2c_address_);
    for (int attempt = 1; attempt <= max_retries; attempt++) {
      lolin_motor_->getInfo();
      if (lolin_motor_->PRODUCT_ID == PRODUCT_ID_I2C_MOTOR) {
        ESP_LOGCONFIG(TAG, "I2C motor driver device found (PRODUCT_ID: 0x%02X, VERSION: 0x%02X)",
                     lolin_motor_->PRODUCT_ID, lolin_motor_->VERSION);
        return;
      }
      ESP_LOGW(TAG, "Attempt %d/%d: I2C motor driver not found, retrying...", attempt, max_retries);
      delay(100);
    }
    ESP_LOGE(TAG, "I2C motor driver device not found after %d attempts", max_retries);
    return;
  }

  if (driver_type_ == "wemos_v1") {
    if (stby_pin_ >= 0) {
      wemos_motor_a_ = new Motor(i2c_address_, _MOTOR_A, WEMOS_DEFAULT_FREQ_HZ, (uint8_t) stby_pin_);
      wemos_motor_b_ = new Motor(i2c_address_, _MOTOR_B, WEMOS_DEFAULT_FREQ_HZ, (uint8_t) stby_pin_);
    } else {
      wemos_motor_a_ = new Motor(i2c_address_, _MOTOR_A, WEMOS_DEFAULT_FREQ_HZ);
      wemos_motor_b_ = new Motor(i2c_address_, _MOTOR_B, WEMOS_DEFAULT_FREQ_HZ);
    }
    ESP_LOGCONFIG(TAG, "I2C motor driver ready (wemos_v1, address=0x%02X)", i2c_address_);
    return;
  }

  ESP_LOGE(TAG, "Unknown driver: %s", driver_type_.c_str());
}

void I2CMotorDriver::set_duty(const std::string &channel, uint8_t duty) {
  if (driver_type_ == "lolin_v2" && lolin_motor_ != nullptr) {
    lolin_motor_->changeDuty(parse_motor_channel(channel), (float) duty);
    ESP_LOGD(TAG, "Set duty cycle: channel=%s, duty=%d", channel.c_str(), duty);
    return;
  }
  if (driver_type_ == "wemos_v1" && wemos_motor_a_ != nullptr && wemos_motor_b_ != nullptr) {
    float pwm = (float) duty;
    if (channel == "A")
      wemos_motor_a_->setmotor(status_to_wemos_dir(_status_a), pwm);
    else if (channel == "B")
      wemos_motor_b_->setmotor(status_to_wemos_dir(_status_b), pwm);
    else if (channel == "BOTH") {
      wemos_motor_a_->setmotor(status_to_wemos_dir(_status_a), pwm);
      wemos_motor_b_->setmotor(status_to_wemos_dir(_status_b), pwm);
    }
    ESP_LOGD(TAG, "Set duty cycle: channel=%s, duty=%d", channel.c_str(), duty);
    return;
  }
}

void I2CMotorDriver::set_freq(const std::string &channel, uint16_t frequency) {
  if (driver_type_ == "lolin_v2" && lolin_motor_ != nullptr) {
    lolin_motor_->changeFreq(parse_motor_channel(channel), (uint32_t) frequency);
    ESP_LOGD(TAG, "Set frequency: channel=%s, frequency=%d Hz", channel.c_str(), frequency);
    return;
  }
  if (driver_type_ == "wemos_v1" && wemos_motor_a_ != nullptr && wemos_motor_b_ != nullptr) {
    uint32_t freq = (uint32_t) frequency;
    if (channel == "A" || channel == "BOTH")
      wemos_motor_a_->setfreq(freq);
    if (channel == "B" || channel == "BOTH")
      wemos_motor_b_->setfreq(freq);
    ESP_LOGD(TAG, "Set frequency: channel=%s, frequency=%d Hz", channel.c_str(), frequency);
    return;
  }
}

void I2CMotorDriver::set_status(const std::string &channel, const std::string &status) {
  MOTOR_CHANNEL ch = parse_motor_channel(channel);
  MOTOR_STATUS st = parse_motor_status(status);

  if (ch == MOTOR_CH_A || ch == MOTOR_CH_BOTH)
    _status_a = st;
  if (ch == MOTOR_CH_B || ch == MOTOR_CH_BOTH)
    _status_b = st;

  if (driver_type_ == "lolin_v2" && lolin_motor_ != nullptr) {
    lolin_motor_->changeStatus(ch, (unsigned char) st);
    ESP_LOGD(TAG, "Set status: channel=%s, status=%s", channel.c_str(), status.c_str());
    return;
  }
  if (driver_type_ == "wemos_v1" && wemos_motor_a_ != nullptr && wemos_motor_b_ != nullptr) {
    uint8_t dir = status_to_wemos_dir(status);
    if (channel == "A")
      wemos_motor_a_->setmotor(dir);
    else if (channel == "B")
      wemos_motor_b_->setmotor(dir);
    else if (channel == "BOTH") {
      wemos_motor_a_->setmotor(dir);
      wemos_motor_b_->setmotor(dir);
    }
    ESP_LOGD(TAG, "Set status: channel=%s, status=%s", channel.c_str(), status.c_str());
    return;
  }
}

std::string I2CMotorDriver::get_status(const std::string &channel) {
  MOTOR_CHANNEL ch = parse_motor_channel(channel);
  MOTOR_STATUS status;
  if (ch == MOTOR_CH_A)
    status = _status_a;
  else if (ch == MOTOR_CH_B)
    status = _status_b;
  else
    return "UNKNOWN";

  switch (status) {
    case MOTOR_STATUS_STOP:
      return "STOP";
    case MOTOR_STATUS_CCW:
      return "CCW";
    case MOTOR_STATUS_CW:
      return "CW";
    case MOTOR_STATUS_SHORT_BRAKE:
      return "SHORT_BRAKE";
    case MOTOR_STATUS_STANDBY:
      return "STANDBY";
    default:
      return "UNKNOWN";
  }
}

MOTOR_STATUS I2CMotorDriver::parse_motor_status(const std::string &status) {
  if (status == "STOP") return MOTOR_STATUS_STOP;
  if (status == "CCW") return MOTOR_STATUS_CCW;
  if (status == "CW") return MOTOR_STATUS_CW;
  if (status == "SHORT_BRAKE") return MOTOR_STATUS_SHORT_BRAKE;
  if (status == "STANDBY") return MOTOR_STATUS_STANDBY;
  return MOTOR_STATUS_STOP;
}

MOTOR_CHANNEL I2CMotorDriver::parse_motor_channel(const std::string &channel) {
  if (channel == "A") return MOTOR_CH_A;
  if (channel == "B") return MOTOR_CH_B;
  if (channel == "BOTH") return MOTOR_CH_BOTH;
  ESP_LOGE(TAG, "Invalid motor channel: %s", channel.c_str());
  return MOTOR_CH_BOTH;
}

uint8_t I2CMotorDriver::status_to_wemos_dir(const std::string &status) {
  if (status == "STOP") return _STOP;
  if (status == "CCW") return _CCW;
  if (status == "CW") return _CW;
  if (status == "SHORT_BRAKE") return _SHORT_BRAKE;
  if (status == "STANDBY") return _STANDBY;
  return _STOP;
}

uint8_t I2CMotorDriver::status_to_wemos_dir(MOTOR_STATUS st) {
  switch (st) {
    case MOTOR_STATUS_STOP: return _STOP;
    case MOTOR_STATUS_CCW: return _CCW;
    case MOTOR_STATUS_CW: return _CW;
    case MOTOR_STATUS_SHORT_BRAKE: return _SHORT_BRAKE;
    case MOTOR_STATUS_STANDBY: return _STANDBY;
    default: return _STOP;
  }
}

}  // namespace i2cmotordriver
}  // namespace esphome
