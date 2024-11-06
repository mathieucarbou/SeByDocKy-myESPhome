#include "gp8403_output.h"

#include "esphome/core/log.h"

namespace esphome {
namespace gp8403 {

static const char *const TAG = "gp8403.output";

static const uint8_t OUTPUT_REGISTER = 0x02;

void GP8403Output::dump_config() {
  ESP_LOGCONFIG(TAG, "GP8403 Output:");
  ESP_LOGCONFIG(TAG, "  Channel: %u", this->channel_);
}

void GP8403Output::write_state(float state) {
  //uint16_t coeff = get_gp8413() ? 4095 : 32767;
  //uint16_t coeff = this->parent_->gp8413_ ? 4095 : 32767;
  //uint16_t value = ((uint16_t) (state * coeff)) << 4;
  uint16_t value = (this->parent_->gp8413_) ? ((uint16_t) (32767 * coeff)) : (((uint16_t) (4095 * coeff)) << 4);
  i2c::ErrorCode err = this->parent_->write_register(OUTPUT_REGISTER + (2 * this->channel_), (uint8_t *) &value, 2);
  if (err != i2c::ERROR_OK) {
    ESP_LOGE(TAG, "Error writing to GP8403, code %d", err);
  }
}

}  // namespace gp8403
}  // namespace esphome
