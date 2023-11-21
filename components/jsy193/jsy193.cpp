#include "jsy193.h"
#include "esphome/core/log.h"

namespace esphome {
namespace jsy193 {

static const char *const TAG = "jsy193";

static const uint8_t JSY193_CMD_READ_IN_REGISTERS = 0x04;
static const uint8_t JSY193_CMD_START_ADDRESS = 0x100;
static const uint8_t JSY193_CMD_RESET_ENERGY = 0x42;
static const uint8_t JSY193_REGISTER_COUNT = 20;  // 20x 16-bit registers

void JSY193::on_modbus_data(const std::vector<uint8_t> &data) {
  if (data.size() < JSY193_REGISTER_COUNT*2) {
    ESP_LOGW(TAG, "Invalid size for JSY193 data!");
    return;
  }

  auto jsy193_get_16bit = [&](size_t i) -> uint16_t {
    return (uint16_t(data[i + 0]) << 8) | (uint16_t(data[i + 1]) << 0);
  };
  auto jsy193_get_32bit = [&](size_t i) -> uint32_t {
    return (uint32_t(jsy193_get_16bit(i + 2)) << 16) | (uint32_t(jsy193_get_16bit(i + 0)) << 0);
  };

  uint16_t raw_voltage = jsy193_get_16bit(0);
  float voltage1 = raw_voltage / 100.0f;  // max 655?35 V

  uint16_t raw_sign = jsy193_get_16bit(6); //0 for positive, 1 for negative 
  
  uint16_t raw_current = jsy193_get_16bit(2);  
  float current1 = ((1 - raw_sign)*raw_current - raw_sign*raw_current)/100.0f;  // max 4294967.295 A
  
  uint16_t raw_power   = jsy193_get_16bit(4);
  float power1 = (1 - raw_sign)*raw_power - raw_sign*raw_power;  // max 4294967.295 A
    
  float pos_energy1 = static_cast<float>(jsy193_get_32bit(8))/1000.0f;
  float neg_energy1 = static_cast<float>(jsy193_get_32bit(12))/1000.0f;

  uint16_t raw_power_factor = jsy193_get_16bit(16);
  float power_factor = raw_power_factor / 1000.0f;

  uint16_t raw_frequency = jsy193_get_16bit(18);
  float frequency1 = raw_frequency / 100.0f;
  
  
  uint16_t raw_voltage = jsy193_get_16bit(20);
  float voltage2 = raw_voltage / 100.0f;  // max 655?35 V

  uint16_t raw_sign = jsy193_get_16bit(26); //0 for positive, 1 for negative 
  
  uint16_t raw_current = jsy193_get_16bit(22);  
  float current2 = ((1 - raw_sign)*raw_current - raw_sign*raw_current)/100.0f;  // max 
  
  uint16_t raw_power   = jsy193_get_16bit(24);
  float power2 = (1 - raw_sign)*raw_power - raw_sign*raw_power;  // max
    
  float pos_energy2 = static_cast<float>(jsy193_get_32bit(28))/1000.0f;
  float neg_energy2 = static_cast<float>(jsy193_get_32bit(32))/1000.0f;

  uint16_t raw_power_factor = jsy193_get_16bit(36);
  float power_factor2 = raw_power_factor / 1000.0f;

  uint16_t raw_frequency = jsy193_get_16bit(38);
  float frequency2 = raw_frequency / 100.0f;

  
  ESP_LOGD(TAG, "JSY193: V1=%.1f V, I1=%.3f A, P1=%.1f W, E1+=%.1f kWh , E1-=%.1f kWh, F1=%.1f Hz, PF1=%.2f , V2=%.1f V, I2=%.3f A, P2=%.1f W, E2+=%.1f kWh , E2-=%.1f kWh, F2=%.1f Hz, PF2=%.2f", voltage1, current1, power1,
           pos_energy1, neg_energy1, frequency1, power_factor1, voltage2, current2, power2, pos_energy2, neg_energy2, frequency2, power_factor2);
  if (this->voltage1_sensor_ != nullptr)
    this->voltage1_sensor_->publish_state(voltage1);
  if (this->current1_sensor_ != nullptr)
    this->current1_sensor_->publish_state(current1);
  if (this->power1_sensor_ != nullptr)
    this->power1_sensor_->publish_state(power1);
  if (this->pos_energy1_sensor_ != nullptr)
    this->pos_energy1_sensor_->publish_state(pos_energy1);
  if (this->neg_energy1_sensor_ != nullptr)
    this->neg_energy1_sensor_->publish_state(neg_energy1);
  if (this->frequency1_sensor_ != nullptr)
    this->frequency1_sensor_->publish_state(frequency1);
  if (this->power_factor1_sensor_ != nullptr)
    this->power_factor1_sensor_->publish_state(power_factor1);
  
  if (this->voltage2_sensor_ != nullptr)
    this->voltage2_sensor_->publish_state(voltage2);
  if (this->current2_sensor_ != nullptr)
    this->current2_sensor_->publish_state(current2);
  if (this->power2_sensor_ != nullptr)
    this->power2_sensor_->publish_state(power2);
  if (this->pos_energy2_sensor_ != nullptr)
    this->pos_energy2_sensor_->publish_state(pos_energy2);
  if (this->neg_energy2_sensor_ != nullptr)
    this->neg_energy2_sensor_->publish_state(neg_energy2);
  if (this->frequency2_sensor_ != nullptr)
    this->frequency2_sensor_->publish_state(frequency2);
  if (this->power_factor2_sensor_ != nullptr)
    this->power_factor2_sensor_->publish_state(power_factor2);
}

void JSY193::update() { this->send(JSY193_CMD_READ_IN_REGISTERS, JSY193_CMD_START_ADDRESS , JSY193_REGISTER_COUNT); }
void JSY193::dump_config() {
  ESP_LOGCONFIG(TAG, "JSY193:");
  ESP_LOGCONFIG(TAG, "  Address: 0x%02X", this->address_);
  LOG_SENSOR("", "Voltage1", this->voltage1_sensor_);
  LOG_SENSOR("", "Current1", this->current1_sensor_);
  LOG_SENSOR("", "Power1", this->power1_sensor_);
  LOG_SENSOR("", "Positive Energy1", this->pos_energy1_sensor_);
  LOG_SENSOR("", "Negative Energy1", this->neg_energy1_sensor_);
  LOG_SENSOR("", "Frequency1", this->frequency1_sensor_);
  LOG_SENSOR("", "Power Factor1", this->power_factor1_sensor_);
  
  LOG_SENSOR("", "Voltage2", this->voltage2_sensor_);
  LOG_SENSOR("", "Current2", this->current2_sensor_);
  LOG_SENSOR("", "Power2", this->power2_sensor_);
  LOG_SENSOR("", "Positive Energy2", this->pos_energy2_sensor_);
  LOG_SENSOR("", "Negative Energy2", this->neg_energy2_sensor_);
  LOG_SENSOR("", "Frequency2", this->frequency2_sensor_);
  LOG_SENSOR("", "Power Factor2", this->power_factor2_sensor_);
}

void JSY193::reset_energy_() {
  std::vector<uint8_t> cmd;
  cmd.push_back(this->address_);
  cmd.push_back(JSY193_CMD_RESET_ENERGY);
  this->send_raw(cmd);
}

}  // namespace jsy193
}  // namespace esphome