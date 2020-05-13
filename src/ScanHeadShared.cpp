/**
 * Copyright (c) JoeScan Inc. All Rights Reserved.
 *
 * Licensed under the BSD 3 Clause License. See LICENSE.txt in the project
 * root for license information.
 */

#include "ScanHeadShared.hpp"
#include <ctime>

using namespace joescan;

ScanHeadShared::ScanHeadShared(std::string serial, uint32_t id)
  : circ_buffer(kMaxCircularBufferSize)
{
  this->is_data_available_condition_enabled = false, this->id = id;
  this->serial = serial;
  this->status_message_timestamp = 0;
}

ScanHeadConfiguration ScanHeadShared::GetConfig() const
{
  return config;
}

void ScanHeadShared::SetConfig(ScanHeadConfiguration config)
{
  this->config = config;
}

uint32_t ScanHeadShared::AvailableProfiles()
{
  return circ_buffer.size();
}

uint32_t ScanHeadShared::WaitUntilAvailableProfiles(uint32_t count,
                                                    uint32_t timeout_us)
{
  std::chrono::microseconds elapsed(0);
  auto t0 = std::chrono::high_resolution_clock::now();

  while ((is_data_available_condition_enabled) &&
         (circ_buffer.size() < count) && (elapsed.count() < timeout_us)) {
    std::unique_lock<std::mutex> lock(data_lock);
    data_available.wait(lock);

    auto t1 = std::chrono::high_resolution_clock::now();
    elapsed = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0);
  }

  return circ_buffer.size();
}

void ScanHeadShared::EnableWaitUntilAvailable(void)
{
  std::lock_guard<std::mutex> lock(data_lock);
  is_data_available_condition_enabled = true;
  data_available.notify_all();
}

void ScanHeadShared::DisableWaitUntilAvailable(void)
{
  std::lock_guard<std::mutex> lock(data_lock);
  is_data_available_condition_enabled = false;
  data_available.notify_all();
}

std::shared_ptr<Profile> ScanHeadShared::PopProfile()
{
  std::shared_ptr<Profile> profile = nullptr;
  std::lock_guard<std::mutex> lock(data_lock);

  if (!circ_buffer.empty()) {
    profile = circ_buffer.front();
    circ_buffer.pop_front();
  }

  return profile;
}

std::vector<std::shared_ptr<Profile>> ScanHeadShared::PopProfiles(
  uint32_t count)
{
  std::vector<std::shared_ptr<Profile>> profiles;
  std::shared_ptr<Profile> profile = nullptr;
  std::lock_guard<std::mutex> lock(data_lock);

  while (!circ_buffer.empty() && (0 < count)) {
    profile = circ_buffer.front();
    circ_buffer.pop_front();

    profiles.push_back(profile);
    count--;
  }

  return profiles;
}

void ScanHeadShared::PushProfile(std::shared_ptr<Profile> profile)
{
  std::lock_guard<std::mutex> lock(data_lock);
  circ_buffer.push_back(profile);
  data_available.notify_all();
}

StatusMessage ScanHeadShared::GetStatusMessage() const
{
  return status_message;
}

void ScanHeadShared::SetStatusMessage(StatusMessage status_message)
{
  std::lock_guard<std::mutex> lock(data_lock);
  this->status_message = status_message;
  this->status_message_timestamp = std::time(nullptr);
  data_available.notify_all();
}

uint64_t ScanHeadShared::GetStatusMessageTimestamp() const
{
  return status_message_timestamp;
}

std::string ScanHeadShared::GetSerial() const
{
  return serial;
}

uint32_t ScanHeadShared::GetId() const
{
  return id;
}
