/**
 * Copyright (c) JoeScan Inc. All Rights Reserved.
 *
 * Licensed under the BSD 3 Clause License. See LICENSE.txt in the project
 * root for license information.
 */

#include "Profile.hpp"
#include <cstring>
#include <stdexcept>

using namespace joescan;

Profile::Profile(DataPacket &packet) :
  m_data_size(0),
  m_image_size(0),
  m_num_valid_brightness(0),
  m_num_valid_geometry(0),
  m_udp_packets_expected(0),
  m_udp_packets_received(0)
{
  const uint16_t datatype_mask = packet.GetContents();

  if (datatype_mask & DataType::Image) {
    m_image.resize(kMaxColumns * kMaxRows, 0);
    // this is a small optimization, cache the value to avoid repeated calls
    // to std::vector size() function
    m_image_size = static_cast<uint32_t>(m_image.size());
  }

  if ((datatype_mask & DataType::Brightness) ||
      (datatype_mask & DataType::XYData)) {
    m_data.fill({
      JS_PROFILE_DATA_INVALID_XY,         // x
      JS_PROFILE_DATA_INVALID_XY,         // y
      JS_PROFILE_DATA_INVALID_BRIGHTNESS, // brightness
    });
    // this is a small optimization, cache the value to avoid repeated calls
    // to std::vector size() function
    m_data_size = static_cast<uint32_t>(m_data.size());
  }

  if (datatype_mask & DataType::Subpixel) {
    // TODO: FKS-252
    throw std::runtime_error("Subpixel DataType currently not supported.");
  }

  m_camera = static_cast<jsCamera>(packet.m_hdr.camera_id);
  m_laser = static_cast<jsLaser>(packet.m_hdr.laser_id);
  m_exposure_time = packet.m_hdr.exposure_time_us;
  m_laser_on_time = packet.m_hdr.laser_on_time_us;
  m_scan_head_id = packet.m_hdr.scan_head_id;
  m_timestamp = packet.m_hdr.timestamp_ns;

  if (JS_ENCODER_MAX <= packet.m_encoders.size()) {
    throw std::runtime_error("Cannot add more than 3 encoders to a profile.");
  }
  this->m_encoder_vals = packet.m_encoders;
}

void Profile::SetScanHead(uint8_t m_scan_head_id)
{
  this->m_scan_head_id = m_scan_head_id;
}

void Profile::SetCamera(jsCamera m_camera)
{
  this->m_camera = m_camera;
}

void Profile::SetLaser(jsLaser m_laser)
{
  this->m_laser = m_laser;
}

void Profile::SetTimestamp(uint64_t m_timestamp)
{
  this->m_timestamp = m_timestamp;
}

void Profile::SetEncoderValues(std::vector<int64_t> encoders)
{
  if (JS_ENCODER_MAX <= encoders.size()) {
    throw std::runtime_error("Cannot add more than 3 encoders to a profile.");
  }
  this->m_encoder_vals = encoders;
}

void Profile::SetExposureTime(uint32_t exposure)
{
  m_exposure_time = exposure;
}

void Profile::SetLaserOnTime(uint32_t m_laser_on_time)
{
  this->m_laser_on_time = m_laser_on_time;
}

void Profile::SetUDPPacketInfo(uint32_t packets_received,
                               uint32_t packets_expected)
{
  m_udp_packets_expected = packets_expected;
  m_udp_packets_received = packets_received;
}

std::pair<uint32_t, uint32_t> Profile::GetUDPPacketInfo()
{
  std::pair<uint32_t, uint32_t> info;
  info = std::make_pair(m_udp_packets_received, m_udp_packets_expected);
  return info;
}

void Profile::InsertPixelCoordinate(uint32_t idx, Point2D<int32_t> value)
{
  // if (idx < m_data_size) {
  // m_data[idx].subpixel = value;
  //}

  // TODO: FKS-252
  throw std::runtime_error("Subpixel DataType currently not supported.");
}

void Profile::InsertImage(uint32_t idx, uint8_t value)
{
  if (idx < m_image_size) {
    m_image[idx] = value;
  }
}

void Profile::InsertImageSlice(uint32_t idx, const uint8_t* start, uint32_t len)
{
  if ((idx + len) <= m_image_size) {
    memcpy(&m_image[idx], start, len);
  }
}

uint8_t Profile::GetScanHeadId() const
{
  return m_scan_head_id;
}

jsCamera Profile::GetCamera() const
{
  return m_camera;
}

jsLaser Profile::GetLaser() const
{
  return m_laser;
}

uint64_t Profile::GetTimestamp() const
{
  return m_timestamp;
}

std::vector<int64_t> Profile::GetEncoderValues() const
{
  return m_encoder_vals;
}

uint32_t Profile::GetExposureTime() const
{
  return m_exposure_time;
}

uint32_t Profile::GetLaserOnTime() const
{
  return m_laser_on_time;
}

std::array<jsProfileData, JS_PROFILE_DATA_LEN>& Profile::Data()
{
  return m_data;
}

std::vector<uint8_t> Profile::Image() const
{
  return m_image;
}

int Profile::SourceId() const
{
  return (m_scan_head_id << 16) | (m_camera << 8) | m_laser;
}

uint32_t Profile::GetNumberValidBrightness()
{
  return m_num_valid_brightness;
}

uint32_t Profile::GetNumberValidGeometry()
{
  return m_num_valid_geometry;
}
