#include "mi_band_4.h"
#include "esphome/core/log.h"

#ifdef ARDUINO_ARCH_ESP32

namespace esphome {
namespace mi_band_4 {

static const char *TAG = "mi_band_4";

void MiBand4::dump_config() {
  ESP_LOGCONFIG(TAG, "Xiaomi Mi Band 4");
  LOG_SENSOR("  ", "Steps", this->steps_);
}

bool MiBand4::parse_device(const esp32_ble_tracker::ESPBTDevice &device) {
  if (device.address_uint64() != this->address_) {
    ESP_LOGVV(TAG, "parse_device(): unknown MAC address.");
    return false;
  }
  ESP_LOGVV(TAG, "parse_device(): MAC address %s found.", device.address_str().c_str());

  bool success = false;
  for (auto &service_data : device.get_service_datas()) {
    auto res = parse_header(service_data);
    if (!res.has_value()) {
      continue;
    }

    if (!(parse_message(service_data.data, *res))) {
      continue;
    }
    if (!(report_results(res, device.address_str()))) {
      continue;
    }
    if (res->steps.has_value() && this->steps_ != nullptr)
      this->steps_->publish_state(*res->steps);
    success = true;
  }

  if (!success) {
    return false;
  }

  return true;
}

optional<ParseResult> MiBand4::parse_header(const esp32_ble_tracker::ServiceData &service_data) {
  ParseResult result;
  if (!service_data.uuid.contains(0xE0, 0xFE)) {
    ESP_LOGVV(TAG, "parse_header(): no service data UUID magic bytes");
    return {};
  }

  return result;
}

bool MiBand4::parse_message(const std::vector<uint8_t> &message, ParseResult &result) {
  // Byte 0-1 service uuid
  // Byte 2-5 Steps in uint32_t

  const uint8_t *data = message.data();
  const int data_length = 4;

  if (message.size() != data_length) {
    ESP_LOGVV(TAG, "parse_message(): payload has wrong size (%d)!", message.size());
    return false;
  }

  const uint32_t steps = encode_uint32(data[3], data[2], data[1], data[0]);
  result.steps = steps;

  return true;
}

bool MiBand4::report_results(const optional<ParseResult> &result, const std::string &address) {
  if (!result.has_value()) {
    ESP_LOGVV(TAG, "report_results(): no results available.");
    return false;
  }

  ESP_LOGD(TAG, "Got Mi Band 4 (%s):", address.c_str());

  if (result->steps.has_value()) {
    ESP_LOGD(TAG, "  Steps: %.0f", *result->steps);
  }
  return true;
}

}  // namespace mi_band_4
}  // namespace esphome

#endif
