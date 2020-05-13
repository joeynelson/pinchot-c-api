/**
 * Copyright (c) JoeScan Inc. All Rights Reserved.
 *
 * Licensed under the BSD 3 Clause License. See LICENSE.txt in the project
 * root for license information.
 */

#include "ScanHeadConfiguration.hpp"

#include <stdexcept>

using namespace joescan;

void ScanHeadConfiguration::SetLaserOnTime(double min, double def, double max)
{
  // NOTE: If the laser on time is set to zero, the laser will be turned off.
  if ((0.0 != min) && (min < 0.01 || min > 650)) {
    throw std::range_error("min_laser_on out of range (10μs - 650000μs)");
  }
  if ((0.0 != def) && (def < 0.01 || def > 650)) {
    throw std::range_error("default_laser_on out of range (10μs - 650000μs)");
  }
  if ((0.0 != max) && (max < 0.01 || max > 650)) {
    throw std::range_error("max_laser_on out of range (10μs - 650000μs)");
  }
  if (min > def) {
    throw std::range_error(
      "min_laser_on must be less than or equal to default_laser_on");
  }
  if (max < def) {
    throw std::range_error(
      "max_laser_on must be greater than or equal to default_laser_on");
  }

  min_laser_on = min;
  default_laser_on = def;
  max_laser_on = max;
}

void ScanHeadConfiguration::SetCameraExposure(double min, double def,
                                              double max)
{
  if (min < 0.01 || min > 6000) {
    throw std::range_error("min_camera_on out of range (0.1ms - 6s)");
  }
  if (def < 0.01 || def > 6000) {
    throw std::range_error("default_camera_on out of range (0.1ms - 6s)");
  }
  if (max < 0.01 || max > 6000) {
    throw std::range_error("max_camera_on out of range (0.1ms - 6s)");
  }
  if (min > def) {
    throw std::range_error(
      "min_camera_on must be less than or equal to default_camera_on");
  }
  if (max < def) {
    throw std::range_error(
      "max_camera_on must be greater than or equal to default_camera_on");
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

void ScanHeadConfiguration::SetLaserDetectionThreshold(int threshold)
{
  if (threshold < 0 || threshold > 1023) {
    throw std::range_error("Laser detection threshold out of range (0-1023)");
  }
  this->laser_detection_threshold = threshold;
}

void ScanHeadConfiguration::SetSaturationThreshold(int threshold)
{
  if (threshold < 0 || threshold > 1023) {
    throw std::range_error("Saturation threshold out of range (0-1023)");
  }
  this->saturation_threshold = threshold;
}

void ScanHeadConfiguration::SetSaturationPercentage(int percentage)
{
  if (percentage < 1 || percentage > 100) {
    throw std::range_error("Saturation percentage out of range (1-100)");
  }

  this->saturation_percentage = percentage;
}

void ScanHeadConfiguration::SetAverageIntensity(int intensity)
{
  if (intensity < 0 || intensity > 255) {
    throw std::range_error("Average intensity out of range (0-255)");
  }
  average_image_intensity = intensity;
}

void ScanHeadConfiguration::SetScanOffset(double offset)
{
  scan_offset = offset;
}

double ScanHeadConfiguration::MinLaserOn() const
{
  return min_laser_on;
}

double ScanHeadConfiguration::DefaultLaserOn() const
{
  return default_laser_on;
}

double ScanHeadConfiguration::MaxLaserOn() const
{
  return max_laser_on;
}

double ScanHeadConfiguration::MinExposure() const
{
  return min_exposure;
}

double ScanHeadConfiguration::DefaultExposure() const
{
  return default_exposure;
}

double ScanHeadConfiguration::MaxExposure() const
{
  return max_exposure;
}

int ScanHeadConfiguration::GetLaserDetectionThreshold() const
{
  return laser_detection_threshold;
}

int ScanHeadConfiguration::GetSaturationThreshold() const
{
  return saturation_threshold;
}

int ScanHeadConfiguration::SaturatedPercentage() const
{
  return saturation_percentage;
}

int ScanHeadConfiguration::AverageIntensity() const
{
  return average_image_intensity;
}

double ScanHeadConfiguration::GetScanOffset() const
{
  return scan_offset;
}

ScanWindow ScanHeadConfiguration::ScanWindow() const
{
  return window;
}

AlignmentParams ScanHeadConfiguration::Alignment(int camera) const
{
  if (camera < 0 || camera > 1) {
    throw std::runtime_error("Camera should be either 0 or 1");
  }

  return alignment[camera];
}

ScanHeadConfiguration::ScanHeadConfiguration()
{
}
