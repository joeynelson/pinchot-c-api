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

Profile::Profile(DataType mask)
{
  camera = JS_CAMERA_MAX;
  laser = JS_LASER_MAX;
  data_size = 0;
  exposure_time = 0;
  image_size = 0;
  laser_on_time = 0;
  num_valid_brightness = 0;
  num_valid_geometry = 0;
  scan_head = 0;
  timestamp = 0;
  udp_packets_expected = 0;
  udp_packets_received = 0;

  if (mask & DataType::Image) {
    image.resize(kMaxColumns * kMaxRows, 0);
    // this is a small optimization, cache the value to avoid repeated calls
    // to std::vector size() function
    image_size = static_cast<uint32_t>(image.size());
  }

  if ((mask & DataType::Brightness) || (mask & DataType::XYData)) {
    data.fill({
      JS_PROFILE_DATA_INVALID_XY,         // x
      JS_PROFILE_DATA_INVALID_XY,         // y
      JS_PROFILE_DATA_INVALID_BRIGHTNESS, // brightness
    });
    // this is a small optimization, cache the value to avoid repeated calls
    // to std::vector size() function
    data_size = static_cast<uint32_t>(data.size());
  }

  if (mask & DataType::Subpixel) {
    // TODO: FKS-252
    throw std::runtime_error("Subpixel DataType currently not supported.");
  }
}

void Profile::SetScanHead(uint8_t scan_head)
{
  this->scan_head = scan_head;
}

void Profile::SetCamera(jsCamera camera)
{
  this->camera = camera;
}

void Profile::SetLaser(jsLaser laser)
{
  this->laser = laser;
}

void Profile::SetTimestamp(uint64_t timestamp)
{
  this->timestamp = timestamp;
}

void Profile::SetEncoderValues(std::vector<int64_t> encoders)
{
  if (JS_ENCODER_MAX <= encoders.size()) {
    throw std::runtime_error("Cannot add more than 3 encoders to a profile.");
  }
  this->encoder_vals = encoders;
}

void Profile::SetExposureTime(uint32_t exposure)
{
  exposure_time = exposure;
}

void Profile::SetLaserOnTime(uint32_t laser_on_time)
{
  this->laser_on_time = laser_on_time;
}

void Profile::SetUDPPacketInfo(uint32_t packets_received,
                               uint32_t packets_expected)
{
  udp_packets_expected = packets_expected;
  udp_packets_received = packets_received;
}

std::pair<uint32_t, uint32_t> Profile::GetUDPPacketInfo()
{
  std::pair<uint32_t, uint32_t> info;
  info = std::make_pair(udp_packets_received, udp_packets_expected);
  return info;
}

void Profile::InsertPixelCoordinate(uint32_t idx, Point2D<int32_t> value)
{
  // if (idx < data_size) {
  // data[idx].subpixel = value;
  //}

  // TODO: FKS-252
  throw std::runtime_error("Subpixel DataType currently not supported.");
}

void Profile::InsertImage(uint32_t idx, uint8_t value)
{
  if (idx < image_size) {
    image[idx] = value;
  }
}

void Profile::InsertImageSlice(uint32_t idx, uint8_t* start, uint32_t len)
{
  if ((idx + len) <= image_size) {
    memcpy(&image[idx], start, len);
  }
}

uint8_t Profile::GetScanHeadId() const
{
  return scan_head;
}

jsCamera Profile::GetCamera() const
{
  return camera;
}

jsLaser Profile::GetLaser() const
{
  return laser;
}

uint64_t Profile::GetTimestamp() const
{
  return timestamp;
}

std::vector<int64_t> Profile::GetEncoderValues() const
{
  return encoder_vals;
}

uint32_t Profile::GetExposureTime() const
{
  return exposure_time;
}

uint32_t Profile::GetLaserOnTime() const
{
  return laser_on_time;
}

std::array<jsProfileData, JS_PROFILE_DATA_LEN>& Profile::Data()
{
  return data;
}

std::vector<uint8_t> Profile::Image() const
{
  return image;
}

int Profile::SourceId() const
{
  return (scan_head << 16) | (camera << 8) | laser;
}

uint32_t Profile::GetNumberValidBrightness()
{
  return num_valid_brightness;
}

uint32_t Profile::GetNumberValidGeometry()
{
  return num_valid_geometry;
}
