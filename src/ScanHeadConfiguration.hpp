/**
 * Copyright (c) JoeScan Inc. All Rights Reserved.
 *
 * Licensed under the BSD 3 Clause License. See LICENSE.txt in the project
 * root for license information.
 */

#ifndef JSCANAPI_SCAN_HEAD_CONFIGURATION_H
#define JSCANAPI_SCAN_HEAD_CONFIGURATION_H

#include "AlignmentParams.hpp"
#include "ScanWindow.hpp"
#include "joescan_pinchot.h"

namespace joescan {

class ScanHeadConfiguration {
 public:
  /**
   * Sets the clamping values for the laser autoexposure algorithm. To
   * disable autoexposure, set `min`, `def`, and `max` to the same value.
   *
   * @param min The minimum laser on time in milliseconds.
   * @param def The default laser on time in milliseconds.
   * @param max The maxium laser on time in milliseconds.
   */
  void SetLaserOnTime(double min, double def, double max);

  /**
   * Sets the clamping values for the camera autoexposure algorithm. To
   * disable autoexposure, set `min`, `def`, and `max` to the same value.
   * Note, camera autoexposure is not used in image mode which instead
   * applies the `def` value for exposure.
   *
   * @param min The minimum camera exposure time in milliseconds.
   * @param def The default camera exposure time in milliseconds.
   * @param max The maxium camera exposure time in milliseconds.
   */
  void SetCameraExposure(double min, double def, double max);

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
  void SetLaserDetectionThreshold(int threshold);

  /**
   * Set how bright a data point must be to be considered saturated.
   *
   * @param threshold A value between `0` and `1023`.
   */
  void SetSaturationThreshold(int threshold);

  /**
   * Set the maximum percentage of the pixels in a scan that are allowed to
   * be brighter than the saturation threshold.
   *
   * @param percentage A percentage value between `0` and `100`.
   */
  void SetSaturationPercentage(int percentage);

  /**
   * In modes where image data is requested, the autoexposure control will
   * try to keep the image's average brightness at this level. If you find
   * the image mode is too dark or too bright, then raise or lower this value
   * accordingly. This setting has no effect on the measurement of points; it
   * only changes how the image data is scaled.
   *
   * @param intensity The average intensity value between `0` and `255`.
   */
  void SetAverageIntensity(int intensity);

  /**
   * Apply a time delay to when a scan begins on a given scan head. This can
   * be used to ensure that multiple scan heads are performing scans at
   * distinct points in time rather than at the same time.
   *
   * @param offset The time delay in microseconds.
   */
  void SetScanOffset(double offset);

  /**
   * Get the minimum laser on time in milliseconds.
   *
   * @return The minimum time in milliseconds.
   */
  double MinLaserOn() const;

  /**
   * Get the default laser on time in milliseconds.
   *
   * @return The default time in milliseconds.
   */
  double DefaultLaserOn() const;

  /**
   * Get the maximum laser on time in milliseconds.
   *
   * @return The maximum time in milliseconds.
   */
  double MaxLaserOn() const;

  /**
   * Get the minimum camera exposure time in milliseconds.
   *
   * @return The minimum time in milliseconds.
   */
  double MinExposure() const;

  /**
   * Get the default camera exposure time in milliseconds.
   *
   * @return The default time in milliseconds.
   */
  double DefaultExposure() const;

  /**
   * Get the maximum camera exposure time in milliseconds.
   *
   * @return The minimum time in milliseconds.
   */
  double MaxExposure() const;

  /**
   * Get the laser detection brightness threshold.
   *
   * @return The threshold value.
   */
  int GetLaserDetectionThreshold() const;

  /**
   * Get the brightness saturation threshold.
   *
   * @return The threshold value.
   */
  int GetSaturationThreshold() const;

  /**
   * Get the brightness saturation percentage.
   *
   * @return The percentage value.
   */
  int SaturatedPercentage() const;

  /**
   * Get the average intensity value used for image mode.
   *
   * @return The average intensity value.
   */
  int AverageIntensity() const;

  /**
   * Get the time offset applied to scan in microseconds.
   *
   * @return The time offset in microseconds.
   */
  double GetScanOffset() const;

  /**
   * Gets the scan window configured for the scan head.
   *
   * @return The configured scan window.
   */
  joescan::ScanWindow ScanWindow() const;

  /**
   * Gets the alignment configuration for a given camera.
   *
   * @param camera The camera ID.
   * @return The alignment parameters for the camera.
   */
  AlignmentParams Alignment(int camera) const;

  ScanHeadConfiguration(const ScanHeadConfiguration &other) = default;
  ScanHeadConfiguration();
  ~ScanHeadConfiguration() = default;

 private:
  double min_laser_on;
  double default_laser_on;
  double max_laser_on;
  double min_exposure;
  double default_exposure;
  double max_exposure;
  int laser_detection_threshold;
  int saturation_threshold;
  int saturation_percentage;
  int average_image_intensity;
  double scan_offset;
  joescan::ScanWindow window = {100.0, -100.0, -100.0, 100.0};
  AlignmentParams alignment[JS_CAMERA_MAX];
};
} // namespace joescan

#endif // JSCANAPI_SCAN_HEAD_CONFIGURATION_H
