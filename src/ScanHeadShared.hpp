/**
 * Copyright (c) JoeScan Inc. All Rights Reserved.
 *
 * Licensed under the BSD 3 Clause License. See LICENSE.txt in the project
 * root for license information.
 */

#ifndef JOESCAN_SCAN_HEAD_SHARED_H
#define JOESCAN_SCAN_HEAD_SHARED_H

#include <condition_variable>
#include <mutex>

#include "boost/circular_buffer.hpp"

#include "Profile.hpp"
#include "ScanHeadConfiguration.hpp"
#include "StatusMessage.hpp"
#include "joescan_pinchot.h"

namespace joescan {
class ScanHeadShared {
 public:
  ScanHeadShared(std::string serial, uint32_t id);

  ScanHeadConfiguration GetConfiguration() const;
  void SetConfig(ScanHeadConfiguration config);

  uint32_t AvailableProfiles();
  uint32_t WaitUntilAvailableProfiles(uint32_t count, uint32_t timeout_us);
  void EnableWaitUntilAvailable(void);
  void DisableWaitUntilAvailable(void);
  std::shared_ptr<Profile> PopProfile();
  std::vector<std::shared_ptr<Profile>> PopProfiles(uint32_t count);
  void PushProfile(std::shared_ptr<Profile> profile);

  StatusMessage GetStatusMessage() const;
  void ClearStatusMessage();
  void SetStatusMessage(StatusMessage status_message);

  uint64_t GetStatusMessageTimestamp() const;
  std::string GetSerial() const;
  uint32_t GetId() const;

 private:
  static const int kMaxCircularBufferSize = JS_SCAN_HEAD_PROFILES_MAX;

  ScanHeadConfiguration config;
  StatusMessage status_message;
  boost::circular_buffer<std::shared_ptr<Profile>> circ_buffer;
  std::mutex data_lock;
  std::condition_variable data_available;
  bool is_data_available_condition_enabled;
  uint64_t status_message_timestamp;
  std::string serial;
  uint32_t id;
};
} // namespace joescan

#endif
