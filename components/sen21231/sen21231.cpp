#include "sen21231.h"
#include "esphome/core/log.h"

namespace esphome {
namespace sen21231 {

static const char *const TAG = "sen21231";
	
void SEN21231Component::setup() {
   ESP_LOGCONFIG(TAG, "Setting up SEN23231...");
//   ESP_LOGD(TAG , "debug %d" , this->debug_);
   
   this->write_debug_register(this->debug_);	
	
	
/*	
   if (this->debug_ == 0){
      if (!this->write_byte(PERSON_SENSOR_REG_DEBUG_MODE, this->debug_)) {  
	  this->error_code_ = COMMUNICATION_FAILED;
          this->mark_failed();
          return;    
      }
      else {
         ESP_LOGD(TAG, "debug mode (green led) is turned off");
      }
  }
  else{
      if (!this->write_byte(PERSON_SENSOR_REG_DEBUG_MODE, this->debug_)) {  
	  this->error_code_ = COMMUNICATION_FAILED;
          this->mark_failed();
          return;    
      }
      else {
      ESP_LOGD(TAG, "debug mode (green led) is turned on");
      }
    
  }
   */ 
}
	
void SEN21231Component::update() { this->read_data_(); }

float SEN21231Component::get_setup_priority() const { return setup_priority::DATA; }
	
void SEN21231Component::write_debug_register(uint8_t debug_value) {
  ESP_LOGV(TAG, "Setting debug register to %d", debug_value);
  if ((debug_value < 0) || (debug_value > 1))
    return;
  this->write_register(PERSON_SENSOR_REG_DEBUG_MODE, debug_value, 0);
}	
	
void SEN21231Component::write_register(uint8_t reg, uint8_t bits, uint8_t start_pos) {
  uint8_t write_reg;
  if (!this->read_byte(reg, &write_reg)) {
    this->mark_failed();
    ESP_LOGW(TAG, "read_byte failed - increase log level for more details!");
    return;
  }

  write_reg |= (bits << start_pos);

  if (!this->write_byte(reg, write_reg)) {
    ESP_LOGW(TAG, "write_byte failed - increase log level for more details!");
    return;
  }
}

/*	
uint8_t SEN21231Component::read_register(uint8_t reg) {
  uint8_t value;
  if (write(&reg, 1) != i2c::ERROR_OK) {
    ESP_LOGW(TAG, "Writing register failed!");
    return 0;
  }
  if (read(&value, 1) != i2c::ERROR_OK) {
    ESP_LOGW(TAG, "Reading register failed!");
    return 0;
  }
  return value;
}
*/
void SEN21231Component::dump_config() {
  ESP_LOGCONFIG(TAG, "SEN21231:");
  LOG_I2C_DEVICE(this);
  if (this->is_failed()) {
    ESP_LOGE(TAG, "Communication with SEN21231 failed!");
  }
  ESP_LOGI(TAG, "%s", this->is_failed() ? "FAILED" : "OK");
	
  LOG_UPDATE_INTERVAL(this);
	
  LOG_SENSOR("  ", "nfaces", this->nfaces_sensor_);	
  LOG_SENSOR("  ", "boxconf0", this->boxconf0_sensor_);
  LOG_SENSOR("  ", "x0", this->x0_sensor_);
  LOG_SENSOR("  ", "y0", this->y0_sensor_);
  LOG_SENSOR("  ", "w0", this->w0_sensor_);
  LOG_SENSOR("  ", "h0", this->h0_sensor_);
  LOG_SENSOR("  ", "idconf0", this->idconf0_sensor_);
  LOG_SENSOR("  ", "id0", this->id0_sensor_);
/*	
  LOG_SENSOR("  ", "debug", this->debug_);
*/	
}

void SEN21231Component::read_data_() {
  person_sensor_results_t results;
  this->read_bytes(PERSON_SENSOR_I2C_ADDRESS, (uint8_t *) &results, sizeof(results));
  
  if (this->nfaces_sensor_ != nullptr) {
	  this->nfaces_sensor_->publish_state(results.num_faces);
  }
  ESP_LOGD(TAG, "%d faces detected", results.num_faces);
  
  if (results.num_faces == 1) {
	
	if (this->boxconf0_sensor_ != nullptr) {
	  this->boxconf0_sensor_->publish_state((results.faces[0].box_confidence) );
	}
	ESP_LOGD(TAG, "boxconf0: %d", results.faces[0].box_confidence);
	
	if (this->x0_sensor_ != nullptr) {
	  this->x0_sensor_->publish_state(int(results.faces[0].box_left));
	}
	ESP_LOGD(TAG, "x0: %d", results.faces[0].box_left);
	
	if (this->y0_sensor_ != nullptr) {
	  this->y0_sensor_->publish_state(int(results.faces[0].box_top));
	} 
        ESP_LOGD(TAG, "y0: %d", results.faces[0].box_top);
	
	if (this->w0_sensor_ != nullptr) {
	  this->w0_sensor_->publish_state(int(results.faces[0].box_right));
	} 
        ESP_LOGD(TAG, "w0: %d", results.faces[0].box_right);
    
        if (this->h0_sensor_ != nullptr) {
	  this->h0_sensor_->publish_state(int(results.faces[0].box_bottom));
        }
	ESP_LOGD(TAG, "height: %d", results.faces[0].box_bottom);
	
        if (this->idconf0_sensor_ != nullptr) {
	  this->idconf0_sensor_->publish_state((results.faces[0].id_confidence) );
	}
	ESP_LOGD(TAG, "idconf0: %d", results.faces[0].id_confidence);
        
	if (this->id0_sensor_ != nullptr) {
	  this->id0_sensor_->publish_state((results.faces[0].id) );
	}
	ESP_LOGD(TAG, "id0: %d", results.faces[0].id);
	
	if (this->isfacing0_sensor_ != nullptr) {
	  this->isfacing0_sensor_->publish_state(int(results.faces[0].is_facing));
	}
	ESP_LOGD(TAG, "is facing towards camera: %d", results.faces[0].is_facing);
  }
/*  
  else{
	  this->conf0_sensor_->publish_state(-1);
	  this->x0_sensor_->publish_state(-1);
	  this->y0_sensor_->publish_state(-1);
	  this->w0_sensor_->publish_state(-1);
	  this->h0_sensor_->publish_state(-1);
	  this->isfacing0_sensor_->publish_state(-1);
  }
*/  
}

}  // namespace sen21231
}  // namespace esphome
