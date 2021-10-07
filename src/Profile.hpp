/**
 * Copyright (c) JoeScan Inc. All Rights Reserved.
 *
 * Licensed under the BSD 3 Clause License. See LICENSE.txt in the project
 * root for license information.
 */

#ifndef JOESCAN_PROFILE_H
#define JOESCAN_PROFILE_H

#include <array>
#include <cassert>
#include <memory>
#include <vector>

#include "DataPacket.hpp"
#include "NetworkTypes.hpp"
#include "Point2D.hpp"
#include "joescan_pinchot.h"

namespace joescan {
class Profile {
 public:
  /**
   * Initializes the profile according to the data contents it is to hold.
   *
   * @param packet The initial datapacket to construct the Profile from.
   */
  Profile(DataPacket &packet);

  /**
   * Sets the scan head identifier for the profile.
   *
   * @param m_scan_head_id The ID to set for the scan head.
   */
  void SetScanHead(uint8_t m_scan_head_id);

  /**
   * Sets the m_camera identifier for the profile.
   *
   * @param m_camera The ID to set for the m_camera.
   */
  void SetCamera(jsCamera m_camera);

  /**
   * Sets the laser identifier for the profile.
   *
   * @param laser The ID to set for the laser.
   */
  void SetLaser(jsLaser laser);

  /**
   * Sets the m_camera m_timestamp for the profile.
   *
   * @param m_timestamp The m_camera m_timestamp.
   */
  void SetTimestamp(uint64_t m_timestamp);

  /**
   * Sets the encoder values associated with the given profile.
   *
   * @param encoders Vector of encoder values.
   */
  void SetEncoderValues(std::vector<int64_t> encoders);

  /**
   * Sets the m_camera exposure time for the profile in microseconds.
   *
   * @param exposure The exposure time in microseconds.
   */
  void SetExposureTime(uint32_t exposure);

  /**
   * Sets the laser pulse width time in microseconds.
   *
   * @param m_laser_on_time Pulse width time in microseconds.
   */
  void SetLaserOnTime(uint32_t m_laser_on_time);

  /**
   * Sets information about the packet relating to the number of UDP packets
   * used for this profile.
   *
   * @param packets_received Number of packets received for this profile.
   * @param packets_expected Number of packets expected for this profile.
   */
  void SetUDPPacketInfo(uint32_t packets_received, uint32_t packets_expected);

  /**
   * Inserts brightness measurement at a given position into the profile.
   *
   * @param idx The absolute position in the profile.
   * @param value The measured brightness.
   */
  inline void InsertBrightness(uint32_t idx, uint8_t value);

  /**
   * Inserts a XY geometry point at a given position into the profile.
   *
   * @param idx The absolute position in the profile.
   * @param value The XY geometry.
   */
  inline void InsertPoint(uint32_t idx, Point2D<int32_t> value);

  /**
   * Inserts a XY geometry point and brightness at a given position into the
   * profile.
   *
   * @param idx The absolute position in the profile.
   * @param value The XY geometry.
   */
  inline void InsertPointAndBrightness(uint32_t idx,
                                       Point2D<int32_t> point,
                                       uint8_t brightness);

  /**
   * Inserts a subpixel point at a given position into the profile.
   *
   * @param idx The absolute posiion in the profile.
   * @param value The subpixel value.
   */
  void InsertPixelCoordinate(uint32_t idx, Point2D<int32_t> value);

  /**
   * Inserts a greyscale image pixel at a given position into the profile.
   *
   * @param idx The absolute position in the profile.
   * @param value Greyscale image pixel value.
   */
  void InsertImage(uint32_t idx, uint8_t value);

  /**
   * Inserts a continuous array of image pixels into a given position in the
   * profile.
   *
   * @param idx The absolute position in the profile.
   * @param start Start of image pixel array.
   * @param len The length of the image pixel array.
   */
  void InsertImageSlice(uint32_t idx, const uint8_t* start, uint32_t len);

  /**
   * The scan head identifier for this profile.
   *
   * @return The scan head ID.
   */
  uint8_t GetScanHeadId() const;

  /**
   * The camera identifier for this profile.
   *
   * @return The camera ID.
   */
  jsCamera GetCamera() const;

  /**
   * The laser identifer for this profile.
   *
   * @return The laser ID.
   */
  jsLaser GetLaser() const;

  /**
   * Obtains the camera's m_timestamp for this profile.
   *
   * @return The camera's m_timestamp.
   */
  uint64_t GetTimestamp() const;

  /**
   * The encoder values associated with the given profile.
   *
   * @return Vector of encoder values for this profile.
   */
  std::vector<int64_t> GetEncoderValues() const;

  /**
   * Obtains the camera exposure time for this profile in microseconds.
   *
   * @return The camera exposure time in microseconds.
   */
  uint32_t GetExposureTime() const;

  /**
   * Obtains the laser pulse width for this profile in microseconds.
   *
   * @return The laser pulse width in microseconds.
   */
  uint32_t GetLaserOnTime() const;

  /**
   * Gets information relating to the number of UDP packets used for this
   * profile.
   *
   * @return Pair where first value is number of packets received, the second
   * is the number of packets expected.
   */
  std::pair<uint32_t, uint32_t> GetUDPPacketInfo();

  /**
   * Obtains the total number of valid brightness values in this profile.
   *
   * @return Number of valid brightness values.
   */
  uint32_t GetNumberValidBrightness();

  /**
   * Obtains the total number of valid X/Y geometry values in this profile.
   *
   * @return Number of valid X/Y geometry values.
   */
  uint32_t GetNumberValidGeometry();

  /**
   * Returns all profile data associated with a given profile. Note, not all
   * `jsProfileData` entries in this vector are guranteed to be valid.
   *
   * @return Vector of point data for given profile.
   */
  std::array<jsProfileData, JS_PROFILE_DATA_LEN>& Data();

  /**
   * For image mode, obtains all of the pixel data for a given profile.
   *
   * @return Vector of pixel data for given profile.
   */
  std::vector<uint8_t> Image() const;

  /**
   * Unique identifier for origin of profile data. The returned value is
   * a bit mask where bits 31 to 16 are the scan head ID, 8 to 15 are the
   * m_camera ID, and 7 to 0 are the laser ID.
   *
   * @return Identifier to source of profile data.
   */
  int SourceId() const;

 private:
  static const int kMaxColumns = JS_CAMERA_IMAGE_DATA_MAX_WIDTH;
  static const int kMaxRows = JS_CAMERA_IMAGE_DATA_MAX_HEIGHT;

  uint8_t m_scan_head_id;
  jsCamera m_camera;
  jsLaser m_laser;
  uint64_t m_timestamp;
  uint32_t m_data_size;
  uint32_t m_image_size;
  uint32_t m_num_valid_brightness;
  uint32_t m_num_valid_geometry;
  uint32_t m_udp_packets_expected;
  uint32_t m_udp_packets_received;
  std::vector<int64_t> m_encoder_vals;
  uint32_t m_exposure_time;
  uint32_t m_laser_on_time;
  std::array<jsProfileData, JS_PROFILE_DATA_LEN> m_data;
  std::vector<uint8_t> m_image;
};

/*
 * Note: We inline these functions for performance benefits.
 */

inline void Profile::InsertBrightness(uint32_t idx, uint8_t value)
{
  #ifdef _DEBUG
  assert(idx < m_data_size);
  #endif

  m_data[idx].brightness = static_cast<int32_t>(value);
  m_num_valid_brightness++;
}

inline void Profile::InsertPoint(uint32_t idx, Point2D<int32_t> value)
{
  #ifdef _DEBUG
  assert(idx < m_data_size);
  #endif

  m_data[idx].x = value.x;
  m_data[idx].y = value.y;
  m_num_valid_geometry++;
}

inline void Profile::InsertPointAndBrightness(uint32_t idx,
                                              Point2D<int32_t> point,
                                              uint8_t brightness)
{
  #ifdef _DEBUG
  assert(idx < m_data_size);
  #endif

  m_data[idx].x = point.x;
  m_data[idx].y = point.y;
  m_data[idx].brightness = brightness;
  m_num_valid_geometry++;
  m_num_valid_brightness++;
}
} // namespace joescan

#endif // JOESCAN_PROFILE_H
