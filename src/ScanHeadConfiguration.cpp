/**
 * Copyright (c) JoeScan Inc. All Rights Reserved.
 *
 * Licensed under the BSD 3 Clause License. See LICENSE.txt in the project
 * root for license information.
 */

#include "ScanHeadConfiguration.hpp"

#include <stdexcept>

using namespace joescan;

void ScanHeadConfiguration::SetLaserOnTime(uint32_t min, uint32_t def,
                                           uint32_t max)
{
  // NOTE: If the laser on time is set to zero, the laser will be turned off.
  if ((0 != min) && (min < kMinLaserOnTimeUsec || min > kMaxLaserOnTimeUsec)) {
    throw std::range_error("min laser on out of range");
  }
  if ((0 != def) && (def < kMinLaserOnTimeUsec || def > kMaxLaserOnTimeUsec)) {
    throw std::range_error("default laser on out of range");
  }
  if ((0 != max) && (max < kMinLaserOnTimeUsec || max > kMaxLaserOnTimeUsec)) {
    throw std::range_error("max laser out of range");
  }
  if (min > def) {
    throw std::range_error("min laser on must be less than default");
  }
  if (max < def) {
    throw std::range_error("max laser on must be greater than default");
  }

  min_laser_on = min;
  default_laser_on = def;
  max_laser_on = max;
}

void ScanHeadConfiguration::SetCameraExposure(uint32_t min, uint32_t def,
                                              uint32_t max)
{
  if ((min < kMinCameraExposureUsec) || (min > kMaxCameraExposureUsec)) {
    throw std::range_error("min exposure out of range");
  }
  if ((def < kMinCameraExposureUsec) || (def > kMaxCameraExposureUsec)) {
    throw std::range_error("default exposure out of range");
  }
  if ((max < kMinCameraExposureUsec) || (max > kMaxCameraExposureUsec)) {
    throw std::range_error("max exposure out of range");
  }
  if (min > def) {
    throw std::range_error("min exposure must be less than default");
  }
  if (max < def) {
    throw std::range_error("max exposure must be greater than default");
  }

  min_exposure = min;
  default_exposure = def;
  max_exposure = max;
}

void ScanHeadConfiguration::SetAlignment(jsCamera camera, double roll,
                                         double shift_x, double shift_y)
{
  if (JS_CAMERA_MAX <= camera) {
    throw std::range_error("Invalid camera");
  }

  alignment[camera] = {roll, shift_x, shift_y};
}

void ScanHeadConfiguration::SetAlignment(jsCamera camera,
                                         AlignmentParams alignment)
{
  if (JS_CAMERA_MAX <= camera) {
    throw std::range_error("Invalid camera");
  }

  this->alignment[camera] = alignment;
}

void ScanHeadConfiguration::SetWindow(const joescan::ScanWindow &window)
{
  this->window = window;
}

void ScanHeadConfiguration::SetLaserDetectionThreshold(uint32_t threshold)
{
  if ((threshold < 0) || (threshold > 1023)) {
    throw std::range_error("Laser detection threshold out of range (0-1023)");
  }
  this->laser_detection_threshold = threshold;
}

void ScanHeadConfiguration::SetSaturationThreshold(uint32_t threshold)
{
  if ((threshold < 0) || (threshold > 1023)) {
    throw std::range_error("Saturation threshold out of range (0-1023)");
  }
  this->saturation_threshold = threshold;
}

void ScanHeadConfiguration::SetSaturationPercentage(uint32_t percentage)
{
  if ((percentage < 1) || (percentage > 100)) {
    throw std::range_error("Saturation percentage out of range (1-100)");
  }

  this->saturation_percentage = percentage;
}

void ScanHeadConfiguration::SetAverageIntensity(uint32_t intensity)
{
  if ((intensity < 0) || (intensity > 255)) {
    throw std::range_error("Average intensity out of range (0-255)");
  }
  average_image_intensity = intensity;
}

void ScanHeadConfiguration::SetScanOffset(uint32_t offset)
{
  scan_offset = offset;
}

uint32_t ScanHeadConfiguration::GetMinLaserOn() const
{
  return min_laser_on;
}

uint32_t ScanHeadConfiguration::GetDefaultLaserOn() const
{
  return default_laser_on;
}

uint32_t ScanHeadConfiguration::GetMaxLaserOn() const
{
  return max_laser_on;
}

uint32_t ScanHeadConfiguration::GetMinExposure() const
{
  return min_exposure;
}

uint32_t ScanHeadConfiguration::GetDefaultExposure() const
{
  return default_exposure;
}

uint32_t ScanHeadConfiguration::GetMaxExposure() const
{
  return max_exposure;
}

uint32_t ScanHeadConfiguration::GetLaserDetectionThreshold() const
{
  return laser_detection_threshold;
}

uint32_t ScanHeadConfiguration::GetSaturationThreshold() const
{
  return saturation_threshold;
}

uint32_t ScanHeadConfiguration::GetSaturatedPercentage() const
{
  return saturation_percentage;
}

uint32_t ScanHeadConfiguration::GetAverageIntensity() const
{
  return average_image_intensity;
}

uint32_t ScanHeadConfiguration::GetScanOffset() const
{
  return scan_offset;
}

ScanWindow ScanHeadConfiguration::GetScanWindow() const
{
  return window;
}

AlignmentParams ScanHeadConfiguration::Alignment(int camera) const
{
  if ((camera < 0) || (camera > 1)) {
    throw std::runtime_error("Camera should be either 0 or 1");
  }

  return alignment[camera];
}
