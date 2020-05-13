/**
 * Copyright (c) JoeScan Inc. All Rights Reserved.
 *
 * Licensed under the BSD 3 Clause License. See LICENSE.txt in the project
 * root for license information.
 */

/**
 * @file basic_scanning.cpp
 * @brief Example demonstrating how to read profile data from scan heads.
 *
 * This application shows the fundamentals of how to stream profile data
 * from scan heads up through the client API and into your own code. Each scan
 * head will be initially configured before scanning using generous settings
 * that should guarantee that valid profile data is obtained. Following
 * configuration, a limited number of profiles will be collected before halting
 * the scan and disconnecting from the scan heads.
 */

#include <chrono>
#include <iostream>
#include <thread>
#include <vector>
#include "joescan_pinchot.h"

/**
 * @brief This function is a small utility function used to explore profile
 * data. In this case, it will iterate over the valid profile data and
 * find the highest measurement in the Y axis.
 *
 * @param profiles Array of profiles from a single scan head.
 * @param num_profiles Total number of profiles contained in array.
 * @return jsProfileData The profile measurement with the greatest Y axis value
 * from the array of profiles passed in.
 */
jsProfileData find_scan_profile_highest_point(jsProfile *profiles,
                                              uint32_t num_profiles)
{
  jsProfileData p = {0, 0, 0};

  for (unsigned int i = 0; i < num_profiles; i++) {
    for (unsigned int j = 0; j < profiles[i].data_len; j++) {
      if (profiles[i].data[j].y > p.y) {
        p.brightness = profiles[i].data[j].brightness;
        p.x = profiles[i].data[j].x;
        p.y = profiles[i].data[j].y;
      }
    }
  }

  return p;
}

int main(int argc, char *argv[])
{
  jsScanSystem scan_system = nullptr;
  std::vector<jsScanHead> scan_heads;
  std::vector<uint32_t> serial_numbers;
  jsProfile **profiles = nullptr;
  uint32_t total_profiles = 1000;
  int32_t r = 0;

  if (2 > argc) {
    std::cout << "Usage: " << argv[0] << " SERIAL..." << std::endl;
    return 1;
  }

  // Grab the serial number(s) passed in through the command line.
  for (int i = 1; i < argc; i++) {
    serial_numbers.emplace_back(strtoul(argv[i], NULL, 0));
  }

  {
    const char *version_str;
    jsGetAPIVersion(&version_str);
    std::cout << "joescanapi " << version_str << std::endl;
  }

  try {
    // First step is to create a scan manager to manage the scan heads.
    scan_system = jsScanSystemCreate();
    if (nullptr == scan_system) {
      throw std::runtime_error("failed to create scan system");
    }

    // Create a scan head software object for each serial number passed in
    // through the command line. We'll assign each one a unique ID starting at
    // zero; we'll use this as an easy index for associating profile data with
    // a given scan head.
    int32_t id = 0;
    for (auto serial : serial_numbers) {
      auto scan_head = jsScanSystemCreateScanHead(scan_system, serial, id);
      if (nullptr == scan_head) {
        throw std::runtime_error("failed to create scan_head");
      }
      scan_heads.emplace_back(scan_head);
      id++;
    }

    // For this example application, we'll just use the same configuration
    // settings we made use of in the "Configure and Connect" example. The
    // only real difference here is that we will be applying this configuration
    // to multiple scan heads, using a "for" loop to configure each scan head
    // one after the other.
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

    for (auto scan_head : scan_heads) {
      r = jsScanHeadConfigure(scan_head, &config);
      if (0 > r) {
        throw std::runtime_error("failed to set scan head configuration");
      }

      // To illustrate that each scan head can be configured independently,
      // we'll alternate between two different windows for each scan head. The
      // other options we will leave the same only for the sake of convenience;
      // these can be independently configured as needed.
      uint32_t serial = jsScanHeadGetSerial(scan_head);
      uint32_t id = jsScanHeadGetId(scan_head);
      if (id % 2) {
        std::cout << serial << ": scan window is 20, -20, -20, 20" << std::endl;
        r = jsScanHeadSetWindowRectangular(scan_head, 20.0, -20.0, -20.0, 20.0);
      } else {
        std::cout << serial << ": scan window is 30, -30, -30, 30" << std::endl;
        r = jsScanHeadSetWindowRectangular(scan_head, 30.0, -30.0, -30.0, 30.0);
      }
      if (0 > r) {
        throw std::runtime_error("failed to set window");
      }

      r = jsScanHeadSetAlignment(scan_head, 0.0, 0.0, 0.0, false);
      if (0 > r) {
        throw std::runtime_error("failed to set alignment");
      }
    }

    // Now that the scan heads are configured, we'll connect to the heads.
    r = jsScanSystemConnect(scan_system, 10);
    if (0 > r) {
      // This error condition indicates that something wrong happened during
      // the connection process itself and should be understood by extension
      // that none of the scan heads are connected.
      throw std::runtime_error("failed to connect");
    } else if (jsScanSystemGetNumberScanHeads(scan_system) != r) {
      // On this error condition, connection was successful to some of the scan
      // heads in the system. We can query the scan heads to determine which
      // one successfully connected and which ones failed.
      for (auto scan_head : scan_heads) {
        if (false == jsScanHeadIsConnected(scan_head)) {
          uint32_t serial = jsScanHeadGetSerial(scan_head);
          std::cout << serial << " is NOT connected" << std::endl;
        }
      }
      throw std::runtime_error("failed to connect to all scan heads");
    }

    // Once configured, we can then read the status from the scan head. Since
    // each scan head was configured with a different scan window, they'll
    // each have a different maximum scan rate.
    for (auto scan_head : scan_heads) {
      jsScanHeadStatus status;
      r = jsScanHeadGetStatus(scan_head, &status);
      if (0 > r) {
        throw std::runtime_error("failed to read scan head status");
      }
      uint32_t serial = jsScanHeadGetSerial(scan_head);
      uint32_t max_rate = status.max_scan_rate;
      std::cout << serial << ": max scan rate is " << max_rate << " hz"
                << std::endl;
    }

    // Allocate memory for the profiles we will read out from the scan head
    // when we start scanning.
    profiles = new jsProfile *[scan_heads.size()];
    for (unsigned int i = 0; i < scan_heads.size(); i++) {
      profiles[i] = new jsProfile[total_profiles];
    }

    // To begin scanning on all of the scan heads, all we need to do is
    // command the scan system to start scanning. This will cause all of the
    // scan heads associated with it to begin scanning at the specified rate
    // and data format.
    jsDataFormat data_format = JS_DATA_FORMAT_XY_FULL_LM_FULL;
    double scan_rate_hz = 500;
    std::cout << "start scanning" << std::endl;
    r = jsScanSystemStartScanning(scan_system, scan_rate_hz, data_format);
    if (0 > r) {
      throw std::runtime_error("failed to start scanning");
    }

    // We'll read out a small number of profiles for each scan head, servicing
    // each one in a round robin fashion until the requested number of profiles
    // have been obtained.
    uint32_t max_profiles = 10;
    for (unsigned int i = 0; i < total_profiles; i += max_profiles) {
      for (unsigned int j = 0; j < scan_heads.size(); j++) {
        // Wait until we have 10 profiles available for reading out. Note that
        // this function will block, placing this process in a low CPU state
        // until the requested number of profiles are available.
        r = jsScanHeadWaitUntilProfilesAvailable(scan_heads[j], max_profiles,
                                                 1000000);
        if (0 > r) {
          throw std::runtime_error("failed to wait for profiles");
        }

        // When we arrive here, there should be profile data to read out
        // from the API. We'll call the following function to read out profiles
        // into our array, processing them later.
        r = jsScanHeadGetProfiles(scan_heads[j], &profiles[j][i], max_profiles);
        if (0 > r) {
          throw std::runtime_error("failed to get profiles");
        }
      }
    }

    // We've collected all of our data; time to stop scanning. Calling this
    // function will cause each scan head within the entire scan system to
    // stop scanning. Once we're done scanning, we'll process the data.
    std::cout << "stop scanning" << std::endl;
    r = jsScanSystemStopScanning(scan_system);
    if (0 > r) {
      throw std::runtime_error("failed to stop scanning");
    }

    for (auto scan_head : scan_heads) {
      uint32_t serial = jsScanHeadGetSerial(scan_head);
      uint32_t id = jsScanHeadGetId(scan_head);
      auto p = find_scan_profile_highest_point(profiles[id], total_profiles);
      std::cout << serial << ": highest point x=" << p.x << ",y=" << p.y
                << ",brightness=" << p.brightness << std::endl;
    }

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

  jsScanSystemFree(scan_system);

  // Free memory used to hold the received profiles.
  if (nullptr != profiles) {
    for (unsigned int i = 0; i < scan_heads.size(); i++) {
      delete profiles[i];
    }
    delete profiles;
  }

  return (0 == r) ? 0 : 1;
}
