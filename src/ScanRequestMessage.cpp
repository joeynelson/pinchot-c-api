/**
 * Copyright (c) JoeScan Inc. All Rights Reserved.
 *
 * Licensed under the BSD 3 Clause License. See LICENSE.txt in the project
 * root for license information.
 */

#include "ScanRequestMessage.hpp"
#include "DataFormats.hpp"
#include "TcpSerializationHelpers.hpp"
#include "NetworkIncludes.hpp"
#include <algorithm>

using namespace joescan;

ScanRequest::ScanRequest(jsDataFormat format, uint32_t clientAddress,
                         int clientPort, int scan_head_id, uint32_t interval,
                         uint32_t scanCount,
                         const jsScanHeadConfiguration &config)
{
  this->clientAddress = clientAddress;
  this->clientPort = clientPort;
  this->scan_head_id = scan_head_id;
  this->camera_id = 0; // TODO: If these become useful, don't hardcode.
  this->laser_id = 0;  // TODO: If these become useful, don't hardcode.
  this->flags = 0;    // TODO: If these become useful, don't hardcode.
  minimumLaserExposure = config.laser_on_time_min_us;
  defaultLaserExposure = config.laser_on_time_def_us;
  maximumLaserExposure = config.laser_on_time_max_us;
  minimumCameraExposure = config.camera_exposure_time_min_us;
  defaultCameraExposure = config.camera_exposure_time_def_us;
  maximumCameraExposure = config.camera_exposure_time_max_us;
  laserDetectionThreshold = config.laser_detection_threshold;
  saturationThreshold = config.saturation_threshold;
  saturationPercent = config.saturation_percentage;
  // TODO: what to do with this?
  averageImageIntensity = 50;
  scanInterval = interval;
  scanOffset = config.scan_offset_us;
  // If the caller requested a specific number of scans, obey them, otherwise
  // let's hope they didn't mean to scan for a whole week.
  numberOfScans = (scanCount == 0) ? 1000000 : scanCount;
  startCol = 0;
  endCol = 1455;

  dataTypes = DataFormats::GetDataType(format);
  steps = DataFormats::GetStep(format);
}

ScanRequest::ScanRequest(const Datagram &datagram)
{
  if (datagram.size() < 76) {
    // throw std::exception();
  }
  int index = 0;

  magic = ntohs(*reinterpret_cast<const uint16_t *>(&datagram[index]));
  if (magic != kCommandMagic) {
    throw std::exception();
  }
  index += sizeof(uint16_t);

  uint8_t size = datagram[index++];

  requestType = UdpPacketType::_from_integral(datagram[index++]);

  clientAddress =
    ntohl(*(reinterpret_cast<const uint32_t *>(&datagram[index])));
  index += sizeof(uint32_t);
  clientPort = ntohs(*(reinterpret_cast<const uint16_t *>(&datagram[index])));
  index += sizeof(uint16_t);
  requestSequence = datagram[index++];

  scan_head_id = datagram[index++];
  camera_id = datagram[index++];
  laser_id = datagram[index++];
  DEPRECATED_DO_NOT_USE = datagram[index++];
  flags = datagram[index++];

  minimumLaserExposure =
    ntohl(*(reinterpret_cast<const uint32_t *>(&datagram[index])));
  index += sizeof(uint32_t);
  defaultLaserExposure =
    ntohl(*(reinterpret_cast<const uint32_t *>(&datagram[index])));
  index += sizeof(uint32_t);
  maximumLaserExposure =
    ntohl(*(reinterpret_cast<const uint32_t *>(&datagram[index])));
  index += sizeof(uint32_t);

  minimumCameraExposure =
    ntohl(*(reinterpret_cast<const uint32_t *>(&datagram[index])));
  index += sizeof(uint32_t);
  defaultCameraExposure =
    ntohl(*(reinterpret_cast<const uint32_t *>(&datagram[index])));
  index += sizeof(uint32_t);
  maximumCameraExposure =
    ntohl(*(reinterpret_cast<const uint32_t *>(&datagram[index])));
  index += sizeof(uint32_t);

  laserDetectionThreshold =
    ntohl(*(reinterpret_cast<const uint32_t *>(&datagram[index])));
  index += sizeof(uint32_t);
  saturationThreshold =
    ntohl(*(reinterpret_cast<const uint32_t *>(&datagram[index])));
  index += sizeof(uint32_t);
  saturationPercent =
    ntohl(*(reinterpret_cast<const uint32_t *>(&datagram[index])));
  index += sizeof(uint32_t);
  averageImageIntensity =
    ntohl(*(reinterpret_cast<const uint32_t *>(&datagram[index])));
  index += sizeof(uint32_t);

  scanInterval = ntohl(*(reinterpret_cast<const uint32_t *>(&datagram[index])));
  index += sizeof(uint32_t);
  scanOffset = ntohl(*(reinterpret_cast<const uint32_t *>(&datagram[index])));
  index += sizeof(uint32_t);
  numberOfScans =
    ntohl(*(reinterpret_cast<const uint32_t *>(&datagram[index])));
  index += sizeof(uint32_t);

  dataTypes = ntohs(*(reinterpret_cast<const uint16_t *>(&datagram[index])));
  index += sizeof(uint16_t);
  startCol = ntohs(*(reinterpret_cast<const uint16_t *>(&datagram[index])));
  index += sizeof(uint16_t);
  endCol = ntohs(*(reinterpret_cast<const uint16_t *>(&datagram[index])));
  index += sizeof(uint16_t);

  for (int i = 1; i <= dataTypes; i <<= 1) {
    if (i & dataTypes) {
      uint16_t stepVal =
        ntohs(*(reinterpret_cast<const uint16_t *>(&datagram[index])));
      steps.push_back(stepVal);
      index += sizeof(uint16_t);
    }
  }

  if (size != Length()) {
    // throw std::exception();
  }
}

ScanRequest ScanRequest::Deserialize(const Datagram &datagram)
{
  return ScanRequest(datagram);
}

Datagram ScanRequest::Serialize(uint8_t requestSequence)
{
  std::vector<uint8_t> scanRequestPacket;
  scanRequestPacket.reserve(Length());

  size_t index = 0;
  index += SerializeIntegralToBytes(scanRequestPacket, &kCommandMagic);

  scanRequestPacket.push_back(Length());
  scanRequestPacket.push_back(requestType);
  index += 2;

  index += SerializeIntegralToBytes(scanRequestPacket, &clientAddress);
  index += SerializeIntegralToBytes(scanRequestPacket, &clientPort);

  scanRequestPacket.push_back(requestSequence);
  scanRequestPacket.push_back(scan_head_id);
  // TODO: This isn't needed now, but may be useful on multi-camera systems in
  // the future
  scanRequestPacket.push_back(camera_id);
  // TODO: This also isn't needed now.
  scanRequestPacket.push_back(laser_id);
  // deprecated exposure setting
  scanRequestPacket.push_back(DEPRECATED_DO_NOT_USE);
  scanRequestPacket.push_back(flags);
  index += 6;

  index += SerializeIntegralToBytes(scanRequestPacket, &minimumLaserExposure);
  index += SerializeIntegralToBytes(scanRequestPacket, &defaultLaserExposure);
  index += SerializeIntegralToBytes(scanRequestPacket, &maximumLaserExposure);

  index += SerializeIntegralToBytes(scanRequestPacket, &minimumCameraExposure);
  index += SerializeIntegralToBytes(scanRequestPacket, &defaultCameraExposure);
  index += SerializeIntegralToBytes(scanRequestPacket, &maximumCameraExposure);

  index +=
    SerializeIntegralToBytes(scanRequestPacket, &laserDetectionThreshold);
  index += SerializeIntegralToBytes(scanRequestPacket, &saturationThreshold);
  index += SerializeIntegralToBytes(scanRequestPacket, &saturationPercent);
  index += SerializeIntegralToBytes(scanRequestPacket, &averageImageIntensity);

  index += SerializeIntegralToBytes(scanRequestPacket, &scanInterval);
  index += SerializeIntegralToBytes(scanRequestPacket, &scanOffset);
  index += SerializeIntegralToBytes(scanRequestPacket, &numberOfScans);

  index += SerializeIntegralToBytes(scanRequestPacket, &dataTypes);
  index += SerializeIntegralToBytes(scanRequestPacket, &startCol);
  index += SerializeIntegralToBytes(scanRequestPacket, &endCol);

  // for each type, uint16 for step
  for (const auto &step : steps) {
    index += SerializeIntegralToBytes(scanRequestPacket, &step);
  }

  // static_assert(index == Length());

  return scanRequestPacket;
}

void ScanRequest::SetDataTypesAndSteps(DataType types,
                                       std::vector<uint16_t> steps)
{
  unsigned int num_types = 0;
  for (int i = 1; i <= types; i <<= 1) {
    if (i & types) {
      num_types += 1;
    }
  }

  if (num_types == steps.size()) {
    this->steps = steps;
    dataTypes = types;
  }
}

void ScanRequest::SetLaserExposure(uint32_t min, uint32_t def, uint32_t max)
{
  if (min <= def && def <= max) {
    minimumLaserExposure = min;
    defaultLaserExposure = def;
    maximumLaserExposure = max;
  }
}

void ScanRequest::SetCameraExposure(uint32_t min, uint32_t def, uint32_t max)
{
  if (min <= def && def <= max) {
    minimumCameraExposure = min;
    defaultCameraExposure = def;
    maximumCameraExposure = max;
  }
}

bool ScanRequest::operator==(const ScanRequest &other) const
{
  bool same = true;
  if (magic != other.magic || requestType != other.requestType ||
      scan_head_id != other.scan_head_id || camera_id != other.camera_id ||
      laser_id != other.laser_id ||
      minimumLaserExposure != other.minimumLaserExposure ||
      defaultLaserExposure != other.defaultLaserExposure ||
      maximumLaserExposure != other.maximumLaserExposure ||
      minimumCameraExposure != other.minimumCameraExposure ||
      defaultCameraExposure != other.defaultCameraExposure ||
      maximumCameraExposure != other.maximumCameraExposure ||
      laserDetectionThreshold != other.laserDetectionThreshold ||
      saturationThreshold != other.saturationThreshold ||
      saturationPercent != other.saturationPercent ||
      averageImageIntensity != other.averageImageIntensity ||
      scanInterval != other.scanInterval || scanOffset != other.scanOffset ||
      numberOfScans != other.numberOfScans ||
      clientAddress != other.clientAddress || clientPort != other.clientPort ||
      flags != other.flags || requestSequence != other.requestSequence ||
      dataTypes != other.dataTypes || startCol != other.startCol ||
      endCol != other.endCol || steps.size() != other.steps.size()) {
    same = false;
  }

  if (same) {
    for (unsigned int i = 0; i < steps.size(); i++) {
      if (steps[i] != other.steps[i]) {
        same = false;
      }
    }
  }

  return same;
}

bool ScanRequest::operator!=(const ScanRequest &other) const
{
  return !(*this == other);
}
