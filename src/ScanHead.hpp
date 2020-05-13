/**
 * Copyright (c) JoeScan Inc. All Rights Reserved.
 *
 * Licensed under the BSD 3 Clause License. See LICENSE.txt in the project
 * root for license information.
 */

#ifndef JSCANAPI_SCAN_HEAD_H
#define JSCANAPI_SCAN_HEAD_H

#include "ScanHeadConfiguration.hpp"
#include "ScanHeadShared.hpp"
#include "ScanManager.hpp"

#include "boost/circular_buffer.hpp"

#include "joescan_pinchot.h"

#include <string>
#include <vector>

namespace joescan {
struct ScanHeadTemperatures {
  double camera_temp_c[JS_CAMERA_MAX];
  double mainboard_temp_c;
  double mainboard_humidity;
};

class ScanHead {
 public:
  /**
   * Initializes a `ScanHead` object.
   *
   * @param shared A reference to shared data used by the scan head.
   */
  ScanHead(ScanManager &manager, ScanHeadShared &shared);

  /**
   * Gets the scan manager that owns this scan head.
   *
   * @return Reference to `ScanManager` object.
   */
  ScanManager &GetScanManager();

  /**
   * Gets the shared data owned by the `ScanHead` and `ScanHeadReceiver`.
   *
   * @return Reference to `ScanHeadShared` object.
   */
  ScanHeadShared &GetScanHeadShared();

  /**
   * Configures a scan head according to the specified parameters.
   *
   * @param config A reference to the configuration parameters.
   */
  void Configure(ScanHeadConfiguration &config);

  /**
   * Gets the current configuration of the scan head.
   *
   * @return The configuration of the scan head.
   */
  ScanHeadConfiguration GetConfig() const;

  /**
   * Gets the serial number of the scan head.
   *
   * @return String representation of the serial number.
   */
  std::string GetSerialNumber() const;

  /**
   * Gets the ID of the scan head.
   *
   * @return Numeric ID of the scan head.
   */
  uint32_t GetId() const;

  /** Gets the binary representation of the IP address of the scan head.
   * Note, this can be converted to a string representation by using the
   * `inet_ntop` function.
   *
   * @return The binary representation of the scan head's IP address.
   */
  uint32_t GetIpAddress() const;

  /**
   * Sets the binary representation of the IP address of the scan head.
   *
   * @param addr The IP address to set.
   */
  void SetIpAddress(uint32_t addr);

  /**
   * Performs a validation of the scan head's configuration to ensure there
   * are no conflicts. Note, this function is not currently implemented.
   *
   * @return Boolean `true` if configuration is valid, `false` otherwise.
   */
  bool ValidateConfig() const;

  /**
   * Returns the number of profiles that are available to be read from calling
   * the `GetProfiles` function.
   *
   * @return The number of profiles able to be read.
   */
  uint32_t AvailableProfiles();

  /**
   * Blocks until the number of profiles requested are available to be read.
   *
   * @param count The desired number of profiles to wait for.
   * @param timeout_us The max time to wait for in microseconds.
   * @return The number of profiles able to be read.
   */
  uint32_t WaitUntilAvailableProfiles(uint32_t count, uint32_t timeout_us);

  /**
   * Obtains up to the number of scanning profiles requested from the scan
   * head. Note, if the total number of profiles returned from the scan
   * head is less than what is requested, only the actual number of profiles
   * available will be returned in the vector.
   *
   * @param max The maximum number of profiles to return.
   * @return Vector holding references to profile data.
   */
  std::vector<std::shared_ptr<Profile>> GetProfiles(int max);

  /**
   * Obtains the last reported status message from a scan head. Note, status
   * messages are only sent by the scan head when not actively scanning.
   *
   * @return The last reported status message.
   */
  StatusMessage GetStatusMessage() const;

  /**
   * Sets the format of the data being presented to the end user
   *
   * @param format The format the data will be presented in
   */
  void SetDataFormat(jsDataFormat format);

  /**
   * Gets the format of the data being presented to the end user
   *
   * @return The format the data will be presented in
   */
  jsDataFormat GetDataFormat() const;

  ScanHeadTemperatures GetTemperatures();

  /**
   * Flushes all profiles from the internal buffer
   */
  void Flush();

 private:
  static const uint32_t kRESTport = 8080;
  ScanManager &scan_manager;
  ScanHeadShared &shared;
  jsDataFormat data_format;

  uint32_t ip_address;
  std::string ip_address_str;
};
} // namespace joescan

#endif // JSCANAPI_SCAN_HEAD_H
