/**
 * Copyright (c) JoeScan Inc. All Rights Reserved.
 *
 * Licensed under the BSD 3 Clause License. See LICENSE.txt in the project
 * root for license information.
 */

/**
 * @file configure_and_connect.cpp
 * @brief Example showing how to configure and connect to a single scan head.
 *
 * This example application demonstrates how to configure, connect, and
 * disconnect from a single scan head. For configuring the scan head, functions
 * and data structures from the joescanapi will be introduced and utilized in
 * a friendly manner. Following successful configuration, the application will
 * connect to the scan head, print out its current status, and then finally
 * disconnect.
 */

#include <iostream>
#include "joescan_pinchot.h"

void print_type_and_capabilities(jsScanHeadType t, jsScanHeadCapabilities &c)
{
  switch (t) {
  case (JS_SCAN_HEAD_JS50WX):
    std::cout << "JS-50WX" << std::endl;
    break;
  case (JS_SCAN_HEAD_JS50WSC):
    std::cout << "JS-50WSC" << std::endl;
    break;
  default:
    std::cout << "INVALID" << std::endl;
    return;
  }

  std::cout << "\tcamera_brightness_bit_depth=" <<
    c.camera_brightness_bit_depth << std::endl;
  std::cout << "\tmax_camera_image_height=" << c.max_camera_image_height <<
    std::endl;
  std::cout << "\tmax_camera_image_width=" << c.max_camera_image_width <<
    std::endl;
  std::cout << "\tmax_scan_rate=" << c.max_scan_rate << std::endl;
  std::cout << "\tnum_cameras=" << c.num_cameras << std::endl;
  std::cout << "\tnum_encoders=" << c.num_encoders << std::endl;
  std::cout << "\tnum_lasers=" << c.num_lasers << std::endl;
}

/**
 * @brief Prints the contents of a `jsScanHeadStatus` data type to standard out.
 *
 * @param stat Reference to scan head status to print.
 */
void print_scan_head_status(jsScanHeadStatus &stat)
{
  std::cout << "jsScanHeadStatus" << std::endl;
  std::cout << "\tglobal_time_ns=" << stat.global_time_ns << std::endl;
  std::cout << "\tnum_encoder_values=" << stat.num_encoder_values << std::endl;

  std::cout << "\tencoder_values=";
  for (int n = 0; n < JS_ENCODER_MAX; n++) {
    std::cout << stat.encoder_values[n];
    if (n != (JS_ENCODER_MAX - 1)) {
      std::cout << ",";
    } else {
      std::cout << std::endl;
    }
  }

  std::cout << "\tcamera_pixels_in_window=";
  for (int n = 0; n < JS_CAMERA_MAX; n++) {
    std::cout << stat.camera_pixels_in_window[n];
    if (n != (JS_CAMERA_MAX - 1)) {
      std::cout << ",";
    } else {
      std::cout << std::endl;
    }
  }

  std::cout << "\tcamera_temp=";
  for (int n = 0; n < JS_CAMERA_MAX; n++) {
    std::cout << stat.camera_temp[n];
    if (n != (JS_CAMERA_MAX - 1)) {
      std::cout << ",";
    } else {
      std::cout << std::endl;
    }
  }
  std::cout << "\tmainboard_temp=" << stat.mainboard_temp << std::endl;

  std::cout << "\tnum_profiles_sent=" << stat.num_profiles_sent << std::endl;

  std::cout << "\tfirmware_version_major=" << stat.firmware_version_major
            << std::endl;
  std::cout << "\tfirmware_version_minor=" << stat.firmware_version_minor
            << std::endl;
  std::cout << "\tfirmware_version_patch=" << stat.firmware_version_patch
            << std::endl;
}

int main(int argc, char *argv[])
{
  jsScanSystem scan_system = nullptr;
  jsScanHead scan_head = nullptr;
  int32_t r = 0;

  if (2 > argc) {
    std::cout << "Usage: " << argv[0] << " SERIAL..." << std::endl;
    return 1;
  }

  {
    // Display the API version to console output for visual confirmation as to
    // the version being used for this example.
    uint32_t major, minor, patch;
    jsGetAPISemanticVersion(&major, &minor, &patch);
    std::cout << "Joescan API version " << major << "." << minor << "." << patch
              << std::endl;
  }

  try {
    // Grab the serial number of the scan head from the command line.
    uint32_t serial_number = 0;
    serial_number = strtoul(argv[1], nullptr, 0);

    // One of the first calls to the API should be to create a scan manager
    // software object. This object will be used to manage groupings of scan
    // heads, telling them when to start and stop scanning.
    scan_system = jsScanSystemCreate();
    if (nullptr == scan_system) {
      throw std::runtime_error("failed to create scan system");
    }

    // Create a scan head software object for the user's specified serial
    // number and associate it with the scan manager we just created. We'll
    // also assign it a user defined ID that can be used within the application
    // as an optional identifier if prefered over the serial number. Note that
    // at this point, we haven't connected with the physical scan head yet.
    int32_t id = 0;
    scan_head = jsScanSystemCreateScanHead(scan_system, serial_number, id);
    if (nullptr == scan_head) {
      throw std::runtime_error("failed to create scan head");
    }

    // Now that we have successfully created the required software objects
    // needed to interface with the scan head and the scan system it is
    // associated with, we can begin to configure the scan head.

    // Many of the settings directly related to the operation of the cameras
    // and lasers can be found in the `jsScanHeadConfiguration` struct. Refer
    // to the API documentation for specific details regarding each field. For
    // this example, we'll use some generic values not specifically set for any
    // particular scenario.
    jsScanHeadConfiguration config;
    config.scan_offset_us = 0;
    config.camera_exposure_time_min_us = 10000;
    config.camera_exposure_time_def_us = 47000;
    config.camera_exposure_time_max_us = 900000;
    config.laser_on_time_min_us = 100;
    config.laser_on_time_def_us = 100;
    config.laser_on_time_max_us = 1000;
    config.laser_detection_threshold = 120;
    config.saturation_threshold = 800;
    config.saturation_percentage = 30;
    r = jsScanHeadConfigure(scan_head, &config);
    if (0 > r) {
      throw std::runtime_error("failed to set scan head configuration");
    }

    // Proper window selection can be crucial to successful scanning as it
    // allows users to limit the region of interest for scanning; filtering out
    // other sources of light that could complicate scanning. It is worth
    // noting that there is an inverse relationship with the scan window and
    // the overall scan rate a system can run at. Using larger scan windows
    // will reduce the maximum scan rate of a system, whereas using a smaller
    // scan window will increase the maximum scan rate.
    r = jsScanHeadSetWindowRectangular(scan_head, 30.0, -30.0, -30.0, 30.0);
    if (0 > r) {
      throw std::runtime_error("failed to set window");
    }

    // Setting the alignment through the following function can help to
    // correct for any mounting issues with a scan head that could affect
    // the 3D measurement. For this example, we'll assume that the scan head
    // is mounted perfectly such that the laser is pointed directly at the scan
    // target.
    r = jsScanHeadSetAlignment(scan_head, 0.0, 0.0, 0.0, false);
    if (0 > r) {
      throw std::runtime_error("failed to set alignment");
    }

    // We've now successfully configured the scan head. Now comes the time to
    // connect to the physical scanner and transmit the configuration values
    // we previously set up.
    r = jsScanSystemConnect(scan_system, 10);
    if (jsScanSystemGetNumberScanHeads(scan_system) != r) {
      throw std::runtime_error("failed to connect");
    }

    jsScanHeadType type = jsScanHeadGetType(scan_head);
    if (JS_SCAN_HEAD_INVALID_TYPE == type) {
      throw std::runtime_error("invalid type");
    }

    jsScanHeadCapabilities cap;
    r = jsGetScanHeadCapabilities(type, &cap);
    if (0 > r) {
      throw std::runtime_error("failed to get capabilities");
    }

    print_type_and_capabilities(type, cap);

    // Now that we are connected, we can query the scan head to get it's
    // current status. Note that the status will be updated periodically by the
    // scan head and calling this function multiple times will provide the
    // last reported status of the scan head.
    jsScanHeadStatus status;
    r = jsScanHeadGetStatus(scan_head, &status);
    if (0 > r) {
      throw std::runtime_error("failed to get scan head status");
    }

    print_scan_head_status(status);

    // Once connected, this is the point where we could command the scan system
    // to start scanning; obtaining profile data from the scan heads associated
    // with it. This will be the focus of a later example.

    // We've accomplished what we set out to do for this example; now it's time
    // to bring down our system.
    r = jsScanSystemDisconnect(scan_system);
    if (0 > r) {
      throw std::runtime_error("failed to disconnect");
    }
  } catch (std::exception &e) {
    std::cout << "ERROR: " << e.what() << std::endl;

    // If we end up with an error from the API, we can get some additional
    // diagnostics by looking at the error code.
    if (0 > r) {
      const char *err_str = nullptr;
      jsGetError(r, &err_str);
      std::cout << "jsError (" << r << "): " << err_str << std::endl;
    }
  }

  // Clean up data allocated by the scan manager.
  if (nullptr != scan_system) {
    jsScanSystemFree(scan_system);
  }

  return (0 == r) ? 0 : 1;
}
