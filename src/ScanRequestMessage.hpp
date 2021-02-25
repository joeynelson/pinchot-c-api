/**
 * Copyright (c) JoeScan Inc. All Rights Reserved.
 *
 * Licensed under the BSD 3 Clause License. See LICENSE.txt in the project
 * root for license information.
 */

#ifndef JOESCAN_SCAN_REQUEST_MESSAGE_H
#define JOESCAN_SCAN_REQUEST_MESSAGE_H

#include <chrono>
#include <set>

#include "Enums.hpp"
#include "NetworkTypes.hpp"
#include "joescan_pinchot.h"

namespace joescan {

class ScanRequest {
 public:
  ScanRequest(jsDataFormat format, uint32_t clientAddress, int clientPort,
              int scan_head_id, uint32_t interval, uint32_t scanCount,
              const jsScanHeadConfiguration &config);
  ScanRequest(const Datagram &datagram);
  ScanRequest() = default;
  ~ScanRequest() = default;

  Datagram Serialize(uint8_t requestSequence);
  static ScanRequest Deserialize(const Datagram &datagram);

  inline int Length() const;

  inline UdpPacketType GetRequestType() const;
  inline uint8_t GetScanHeadId() const;
  inline uint8_t GetCameraId() const;
  inline uint8_t GetLaserId() const;
  inline uint8_t GetFlags() const;
  inline uint8_t GetRequestSequence() const;

  inline uint32_t GetMinimumLaserExposure() const;
  inline uint32_t GetDefaultLaserExposure() const;
  inline uint32_t GetMaximumLaserExposure() const;

  inline uint32_t GetMinimumCameraExposure() const;
  inline uint32_t GetDefaultCameraExposure() const;
  inline uint32_t GetMaximumCameraExposure() const;

  inline uint32_t GetLaserDetectionThreshold() const;
  inline uint32_t GetSaturationThreshold() const;
  inline uint32_t GetSaturationPercent() const;
  inline uint32_t GetAverageImageIntensity() const;

  inline uint32_t GetScanInterval() const;
  inline uint32_t GetScanOffset() const;
  inline uint32_t GetNumberOfScans() const;

  inline uint32_t GetClientAddress() const;
  inline uint16_t GetClientPort() const;

  inline uint16_t GetDataTypes() const;
  inline uint16_t GetStartColumn() const;
  inline uint16_t GetEndColumn() const;
  inline const std::vector<uint16_t> &GetStepValues() const;

  void SetDataTypesAndSteps(DataType types, std::vector<uint16_t> steps);
  void SetLaserExposure(uint32_t min, uint32_t def, uint32_t max);
  void SetCameraExposure(uint32_t min, uint32_t def, uint32_t max);

  bool operator==(const ScanRequest &other) const;
  bool operator!=(const ScanRequest &other) const;

 private:
  uint16_t magic;
  UdpPacketType requestType = UdpPacketType::StartScanning;

  uint8_t scan_head_id;
  uint8_t camera_id;
  uint8_t laser_id;

  // deprecated exposure setting, interleaved or simultaneous
  uint8_t DEPRECATED_DO_NOT_USE;

  // Laser exposure, 3 * 4 bytes
  uint32_t minimumLaserExposure;
  uint32_t defaultLaserExposure;
  uint32_t maximumLaserExposure;

  // Camera exposure, 3 * 4 bytes
  uint32_t minimumCameraExposure;
  uint32_t defaultCameraExposure;
  uint32_t maximumCameraExposure;

  // Autoexposure pixel and percentage controls, 4 * 4 bytes
  // Minimum brightness value a pixel must reach for the FPGA to register it as
  // the laser peak
  uint32_t laserDetectionThreshold;

  // Minimum brightness value a pixel must reach for the FPGA to consider the
  // pixel fully saturated
  uint32_t saturationThreshold;

  // Target % of fully saturated pixels within the scan window that the scan
  // autoexposure attempts to reach
  uint32_t saturationPercent;

  // Average pixel brightness target that the image autoexposure attempts to
  // reach
  uint32_t averageImageIntensity;

  // Scan start/duration (server/FPGA will determine the next actual viable
  // start time from the interval), 3 * 4 bytes
  // Interval (in usec?  nsec?) between the start of each scan
  uint32_t scanInterval;
  // Offset (in usec?  nsec?) from the start of the natural scan interval
  // boundary that this device operates (i.e., the trigger phase offset time)
  uint32_t scanOffset;
  // Total number of scans/images to collect for this command
  uint32_t numberOfScans;

  // Data routing, 4 + 2 bytes
  // IP address of client PC
  uint32_t clientAddress;
  // Port on which the client is listening for data packets
  uint16_t clientPort;

  // Multiple scan request packets may be generated during one start/stop scan
  // pair from the API.  All scan requests related to the same API-level scan
  // command are given a common sequence number, so that the scan server can
  // handle any edge case where the user stops & restarts scanning rapidly.  By
  // observing the new request sequence number, the scan server can
  // unambiguously know to treat the last request as the start of something new.

  // Misc stuff, 2 * 1 bytes
  // Currently unused
  uint8_t flags;
  // Used to indicate that this request is/isn't connected with
  // prior/subsequent scans
  uint8_t requestSequence;

  // Data format, (3 + # of types) * 2 bytes
  // Bitmask of requested data return values
  uint16_t dataTypes;
  // First camera column to return data from
  uint16_t startCol;
  // Last camera column to return data from
  uint16_t endCol;

  // The step values are ordered by the profile data type, with lowest-value
  // type's skip first, increasing.  So for example, if a request has brightness
  // and width data types, the brightness skip value is first in the vector,
  // followed by the width skip value.
  std::vector<uint16_t> steps;

  // Overall: 74 + # of types * 2 bytes (steps)
};

inline int ScanRequest::Length() const
{
  size_t sz = 74 + steps.size() * 2;
  return static_cast<int>(sz);
}

inline UdpPacketType ScanRequest::GetRequestType() const
{
  return requestType;
}

inline uint8_t ScanRequest::GetScanHeadId() const
{
  return scan_head_id;
}

inline uint8_t ScanRequest::GetCameraId() const
{
  return camera_id;
}

inline uint8_t ScanRequest::GetLaserId() const
{
  return laser_id;
}

inline uint8_t ScanRequest::GetFlags() const
{
  return flags;
}

inline uint8_t ScanRequest::GetRequestSequence() const
{
  return requestSequence;
}

inline uint32_t ScanRequest::GetMinimumLaserExposure() const
{
  return minimumLaserExposure;
}

inline uint32_t ScanRequest::GetDefaultLaserExposure() const
{
  return defaultLaserExposure;
}

inline uint32_t ScanRequest::GetMaximumLaserExposure() const
{
  return maximumLaserExposure;
}

inline uint32_t ScanRequest::GetMinimumCameraExposure() const
{
  return minimumCameraExposure;
}

inline uint32_t ScanRequest::GetDefaultCameraExposure() const
{
  return defaultCameraExposure;
}

inline uint32_t ScanRequest::GetMaximumCameraExposure() const
{
  return maximumCameraExposure;
}

inline uint32_t ScanRequest::GetLaserDetectionThreshold() const
{
  return laserDetectionThreshold;
}

inline uint32_t ScanRequest::GetSaturationThreshold() const
{
  return saturationThreshold;
}

inline uint32_t ScanRequest::GetSaturationPercent() const
{
  return saturationPercent;
}

inline uint32_t ScanRequest::GetAverageImageIntensity() const
{
  return averageImageIntensity;
}

inline uint32_t ScanRequest::GetScanInterval() const
{
  return scanInterval;
}

inline uint32_t ScanRequest::GetScanOffset() const
{
  return scanOffset;
}

inline uint32_t ScanRequest::GetNumberOfScans() const
{
  return numberOfScans;
}

inline uint32_t ScanRequest::GetClientAddress() const
{
  return clientAddress;
}

inline uint16_t ScanRequest::GetClientPort() const
{
  return clientPort;
}

inline uint16_t ScanRequest::GetDataTypes() const
{
  return dataTypes;
}

inline uint16_t ScanRequest::GetStartColumn() const
{
  return startCol;
}

inline uint16_t ScanRequest::GetEndColumn() const
{
  return endCol;
}

inline const std::vector<uint16_t> &ScanRequest::GetStepValues() const
{
  return steps;
}

} // namespace joescan

#endif // JOESCAN_SCAN_REQUEST_H
