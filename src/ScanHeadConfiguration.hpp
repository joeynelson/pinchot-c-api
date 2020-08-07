/**
 * Copyright (c) JoeScan Inc. All Rights Reserved.
 *
 * Licensed under the BSD 3 Clause License. See LICENSE.txt in the project
 * root for license information.
 */

#ifndef JOESCAN_SCAN_HEAD_CONFIGURATION_H
#define JOESCAN_SCAN_HEAD_CONFIGURATION_H

#include "AlignmentParams.hpp"
#include "ScanWindow.hpp"
#include "joescan_pinchot.h"

namespace joescan {

class ScanHeadConfiguration {
 public:
  ScanHeadConfiguration(const ScanHeadConfiguration &other) = default;
  ScanHeadConfiguration() = default;
  ~ScanHeadConfiguration() = default;

  /**
   * Sets the clamping values for the laser autoexposure algorithm. To
   * disable autoexposure, set `min`, `def`, and `max` to the same value.
   *
   * @param min The minimum laser on time in microseconds.
   * @param def The default laser on time in microseconds.
   * @param max The maxium laser on time in microseconds.
   */
  void SetLaserOnTime(uint32_t min, uint32_t def, uint32_t max);

  /**
   * Sets the clamping values for the camera autoexposure algorithm. To
   * disable autoexposure, set `min`, `def`, and `max` to the same value.
   * Note, camera autoexposure is not used in image mode which instead
   * applies the `def` value for exposure.
   *
   * @param min The minimum camera exposure time in microseconds.
   * @param def The default camera exposure time in microseconds.
   * @param max The maxium camera exposure time in microseconds.
   */
  void SetCameraExposure(uint32_t min, uint32_t def, uint32_t max);

  /**
   * Configure the parameters used to align a scan head.
   *
   * @param camera The camera to apply alignment to.
   * @param roll Rotate XY profile data around mill coordinate system origin.
   * @param shift_x Apply shift in X direction in inches.
   * @param shift_y Apply shift in Y direction in inches.
   */
  void SetAlignment(jsCamera camera, double roll, double shift_x,
                    double shift_y);

  /**
   * Configure the parameters used to align a scan head according to given
   * `AlignmentParams` object.
   *
   * @param camera The camera to apply alignment to.
   * @param alignment Desired alignment parameters.
   */
  void SetAlignment(jsCamera camera, AlignmentParams alignment);

  /**
   * Configure the window that the cameras will view for laser.
   *
   * @param window The window dimensions to be applied.
   */
  void SetWindow(const ScanWindow &window);

  /**
   * Set the minimum brightness a data point must have to be considered a
   * valid data point.
   *
   * @param threshold A value between `0` and `1023`.
   */
  void SetLaserDetectionThreshold(uint32_t threshold);

  /**
   * Set how bright a data point must be to be considered saturated.
   *
   * @param threshold A value between `0` and `1023`.
   */
  void SetSaturationThreshold(uint32_t threshold);

  /**
   * Set the maximum percentage of the pixels in a scan that are allowed to
   * be brighter than the saturation threshold.
   *
   * @param percentage A percentage value between `0` and `100`.
   */
  void SetSaturationPercentage(uint32_t percentage);

  /**
   * In modes where image data is requested, the autoexposure control will
   * try to keep the image's average brightness at this level. If you find
   * the image mode is too dark or too bright, then raise or lower this value
   * accordingly. This setting has no effect on the measurement of points; it
   * only changes how the image data is scaled.
   *
   * @param intensity The average intensity value between `0` and `255`.
   */
  void SetAverageIntensity(uint32_t intensity);

  /**
   * Apply a time delay to when a scan begins on a given scan head. This can
   * be used to ensure that multiple scan heads are performing scans at
   * distinct points in time rather than at the same time.
   *
   * @param offset The time delay in microseconds.
   */
  void SetScanOffset(uint32_t offset);

  /**
   * Get the minimum laser on time in microseconds.
   *
   * @return The minimum time in microseconds.
   */
  uint32_t GetMinLaserOn() const;

  /**
   * Get the default laser on time in microseconds.
   *
   * @return The default time in microseconds.
   */
  uint32_t GetDefaultLaserOn() const;

  /**
   * Get the maximum laser on time in microseconds.
   *
   * @return The maximum time in microseconds.
   */
  uint32_t GetMaxLaserOn() const;

  /**
   * Get the minimum camera exposure time in microseconds.
   *
   * @return The minimum time in microseconds.
   */
  uint32_t GetMinExposure() const;

  /**
   * Get the default camera exposure time in microseconds.
   *
   * @return The default time in microseconds.
   */
  uint32_t GetDefaultExposure() const;

  /**
   * Get the maximum camera exposure time in microseconds.
   *
   * @return The minimum time in microseconds.
   */
  uint32_t GetMaxExposure() const;

  /**
   * Get the laser detection brightness threshold.
   *
   * @return The threshold value.
   */
  uint32_t GetLaserDetectionThreshold() const;

  /**
   * Get the brightness saturation threshold.
   *
   * @return The threshold value.
   */
  uint32_t GetSaturationThreshold() const;

  /**
   * Get the brightness saturation percentage.
   *
   * @return The percentage value.
   */
  uint32_t GetSaturatedPercentage() const;

  /**
   * Get the average intensity value used for image mode.
   *
   * @return The average intensity value.
   */
  uint32_t GetAverageIntensity() const;

  /**
   * Get the time offset applied to scan in microseconds.
   *
   * @return The time offset in microseconds.
   */
  uint32_t GetScanOffset() const;

  /**
   * Gets the scan window configured for the scan head.
   *
   * @return The configured scan window.
   */
  joescan::ScanWindow GetScanWindow() const;

  /**
   * Gets the alignment configuration for a given camera.
   *
   * @param camera The camera ID.
   * @return The alignment parameters for the camera.
   */
  AlignmentParams Alignment(int camera) const;

 private:
  static const uint32_t kMinLaserOnTimeUsec = 15;
  static const uint32_t kMaxLaserOnTimeUsec = 650000;
  static const uint32_t kMinCameraExposureUsec = 15;
  static const uint32_t kMaxCameraExposureUsec = 2000000;

  uint32_t min_laser_on;
  uint32_t default_laser_on;
  uint32_t max_laser_on;
  uint32_t min_exposure;
  uint32_t default_exposure;
  uint32_t max_exposure;
  uint32_t laser_detection_threshold;
  uint32_t saturation_threshold;
  uint32_t saturation_percentage;
  uint32_t average_image_intensity;
  uint32_t scan_offset;
  joescan::ScanWindow window = {100.0, -100.0, -100.0, 100.0};
  AlignmentParams alignment[JS_CAMERA_MAX];
};
} // namespace joescan

#endif // JOESCAN_SCAN_HEAD_CONFIGURATION_H
