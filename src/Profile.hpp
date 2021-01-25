/**
 * Copyright (c) JoeScan Inc. All Rights Reserved.
 *
 * Licensed under the BSD 3 Clause License. See LICENSE.txt in the project
 * root for license information.
 */

#ifndef JOESCAN_PROFILE_H
#define JOESCAN_PROFILE_H

#include <array>
#include <memory>
#include <vector>

#include "NetworkTypes.hpp"
#include "Point2D.hpp"
#include "joescan_pinchot.h"

namespace joescan {
class Profile {
 public:
  /**
   * Initializes the profile according to the data contents it is to hold.
   *
   * @param mask A bitmask of the data types the profile will hold.
   */
  Profile(DataType mask);

  /**
   * Sets the scan head identifier for the profile.
   *
   * @param scan_head The ID to set for the scan head.
   */
  void SetScanHead(uint8_t scan_head);

  /**
   * Sets the camera identifier for the profile.
   *
   * @param camera The ID to set for the camera.
   */
  void SetCamera(jsCamera camera);

  /**
   * Sets the laser identifier for the profile.
   *
   * @param laser The ID to set for the laser.
   */
  void SetLaser(jsLaser laser);

  /**
   * Sets the camera timestamp for the profile.
   *
   * @param timestamp The camera timestamp.
   */
  void SetTimestamp(uint64_t timestamp);

  /**
   * Sets the encoder values associated with the given profile.
   *
   * @param encoders Vector of encoder values.
   */
  void SetEncoderValues(std::vector<int64_t> encoders);

  /**
   * Sets the camera exposure time for the profile in microseconds.
   *
   * @param exposure The exposure time in microseconds.
   */
  void SetExposureTime(uint32_t exposure);

  /**
   * Sets the laser pulse width time in microseconds.
   *
   * @param laser_on_time Pulse width time in microseconds.
   */
  void SetLaserOnTime(uint32_t laser_on_time);

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
  void InsertImageSlice(uint32_t idx, uint8_t* start, uint32_t len);

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
   * Obtains the camera's timestamp for this profile.
   *
   * @return The camera's timestamp.
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
   * camera ID, and 7 to 0 are the laser ID.
   *
   * @return Identifier to source of profile data.
   */
  int SourceId() const;

 private:
  static const int kMaxColumns = JS_CAMERA_IMAGE_DATA_MAX_WIDTH;
  static const int kMaxRows = JS_CAMERA_IMAGE_DATA_MAX_HEIGHT;

  uint8_t scan_head;
  jsCamera camera;
  jsLaser laser;
  uint64_t timestamp;
  uint32_t udp_packets_expected;
  uint32_t udp_packets_received;
  std::vector<int64_t> encoder_vals;
  uint32_t exposure_time;
  uint32_t laser_on_time;
  std::array<jsProfileData, JS_PROFILE_DATA_LEN> data;
  std::vector<uint8_t> image;
  uint32_t data_size;
  uint32_t image_size;
  uint32_t num_valid_geometry;
  uint32_t num_valid_brightness;
};

/*
 * Note: We inline these functions for performance benefits.
 */

inline void Profile::InsertBrightness(uint32_t idx, uint8_t value)
{
  if (idx < data_size) {
    data[idx].brightness = static_cast<int32_t>(value);
    num_valid_brightness++;
  }
}

inline void Profile::InsertPoint(uint32_t idx, Point2D<int32_t> value)
{
  if (idx < data_size) {
    data[idx].x = value.x;
    data[idx].y = value.y;
    num_valid_geometry++;
  }
}
} // namespace joescan

#endif // JOESCAN_PROFILE_H
