/**
 * Copyright (c) JoeScan Inc. All Rights Reserved.
 *
 * Licensed under the BSD 3 Clause License. See LICENSE.txt in the project
 * root for license information.
 */

/**
 * @file advanced_scanning.cpp
 * @brief Example demonstrating how to read profile data from scan heads in a
 * performant manner suitable for real time applications.
 *
 * This application shows how one can stream profile data from multiple scan
 * heads in a manner that allows for real time processing of the data. To
 * accomplish this, multiple threads are created to break up the work of
 * reading in new profile data and acting upon it. Configuration of the scan
 * heads will mostly be identical to previous examples although some values
 * may be changed to allow for faster streaming of data.
 */

#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include "joescan_pinchot.h"

static std::mutex lock;
static uint64_t received_profiles = 0;

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

/**
 * @brief This function receives profile data from a given scan head. We start
 * a thread for each scan head to pull out the data as fast as possible.
 *
 * @param scan_head Reference to the scan head to recieve profile data from.
 */
static void receiver(jsScanHead scan_head)
{
  // Allocate memory to recieve profile data for this scan head.
  const int max_profiles = 100;
  jsProfile *profiles = nullptr;
  profiles = new jsProfile[max_profiles];
  auto id = jsScanHeadGetId(scan_head);

  {
    std::lock_guard<std::mutex> guard(lock);
    std::cout << "begin receiving on scan head ID " << id << std::endl;
  }

  // For this example, we'll grab some profiles and then act on the data before
  // repeating this process again. Note that for high performance applications,
  // printing to standard out while receiving data should be avoided as it
  // can add significant latency. This example only prints to standard out to
  // provide some illustrative feedback to the user, indicating that data
  // is actively being worked on in multiple threads.
  uint32_t timeout_us = 1000000;
  int32_t r = 0;
  do {
    jsScanHeadWaitUntilProfilesAvailable(scan_head, max_profiles, timeout_us);
    r = jsScanHeadGetProfiles(scan_head, profiles, max_profiles);
    if (0 < r) {
      auto p = find_scan_profile_highest_point(profiles, r);
      {
        std::lock_guard<std::mutex> guard(lock);
        std::cout << "highest point for scan head ID " << id << " is x=" << p.x
                  << ",y=" << p.y << ",brightness=" << p.brightness
                  << std::endl;
        received_profiles += r;
      }
    }
  } while (0 < r);

  {
    std::lock_guard<std::mutex> guard(lock);
    std::cout << "end receiving on scan head ID " << id << std::endl;
  }

  delete[] profiles;
}

int main(int argc, char *argv[])
{
  jsScanSystem scan_system = nullptr;
  std::vector<jsScanHead> scan_heads;
  std::thread *threads = nullptr;
  int32_t r = 0;

  if (argc < 2) {
    std::cout << "Usage: " << argv[0] << " SERIAL..." << std::endl;
    return 0;
  }

  std::vector<uint32_t> serial_numbers;
  for (int i = 1; i < argc; i++) {
    serial_numbers.emplace_back(strtoul(argv[i], NULL, 0));
  }

  {
    const char *version_str;
    jsGetAPIVersion(&version_str);
    std::cout << "joescanapi " << version_str << std::endl;
  }

  try {
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

    scan_system = jsScanSystemCreate();
    if (nullptr == scan_system) {
      throw std::runtime_error("failed to create scan system");
    }

    // Create a scan head for each serial number passed in on the command line
    // and configure each one with the same parameters. Note that there is
    // nothing stopping users from configuring each scan head independently.
    for (unsigned int i = 0; i < serial_numbers.size(); i++) {
      uint32_t serial = serial_numbers[i];
      auto scan_head = jsScanSystemCreateScanHead(scan_system, serial, i);
      if (nullptr == scan_head) {
        throw std::runtime_error("failed to create scan head");
      }
      scan_heads.push_back(scan_head);

      r = jsScanHeadConfigure(scan_head, &config);
      if (0 > r) {
        throw std::runtime_error("failed to configure scan head");
      }

      r = jsScanHeadSetWindowRectangular(scan_head, 20.0, -20.0, -20.0, 20.0);
      if (0 > r) {
        throw std::runtime_error("failed to set scan window");
      }
    }

    r = jsScanSystemConnect(scan_system, 10);
    if (0 > r) {
      throw std::runtime_error("failed to connect");
    } else if (scan_heads.size() != static_cast<unsigned int>(r)) {
      throw std::runtime_error("failed to connect to all scan heads");
    }

    std::cout << "start scanning" << std::endl;
    jsDataFormat data_format = JS_DATA_FORMAT_XY_FULL_LM_FULL;
    double scan_rate_hz = 500;
    r = jsScanSystemStartScanning(scan_system, scan_rate_hz, data_format);
    if (0 > r) {
      throw std::runtime_error("failed to start scanning");
    }

    // In order to achieve a performant application, we'll create a thread
    // for each scan head. This allows the CPU load of reading out profiles
    // to be distributed across all the cores available on the system rather
    // than keeping the heavy lifting in an application within a single process.
    threads = new std::thread[scan_heads.size()];
    for (unsigned long i = 0; i < scan_heads.size(); i++) {
      threads[i] = std::thread(receiver, scan_heads[i]);
    }

    // Put this thread to sleep until the total scan time is done.
    unsigned long int scan_time_sec = 10;
    std::this_thread::sleep_for(std::chrono::seconds(scan_time_sec));

    r = jsScanSystemStopScanning(scan_system);
    if (0 > r) {
      throw std::runtime_error("failed to stop scanning");
    }

    for (unsigned long i = 0; i < scan_heads.size(); i++) {
      threads[i].join();
    }
    delete[] threads;
    threads = nullptr;
    std::cout << "stop scanning" << std::endl;

    // Calling `jsScanSystemStopScanning` will return immediately rather than
    // blocking until the scan heads have fully stopped. As a consequence, we
    // will need to add a small delay before the scan heads begin sending
    // new status updates.
    std::cout << "delay for status update" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // We can verify that we received all of the profiles sent by the scan
    // heads by reading each scan head's status message and summing up the
    // number of profiles that were sent. If everything went well and the
    // CPU load didn't exceed what the system can manage, this value should
    // be equal to the number of profiles we received in this application.
    jsScanHeadStatus status;
    unsigned int expected = 0;
    for (auto scan_head : scan_heads) {
      r = jsScanHeadGetStatus(scan_head, &status);
      if (0 > r) {
        throw std::runtime_error("failed to obtain status message");
      }
      expected += status.num_profiles_sent;
    }
    std::cout << "received " << received_profiles << " profiles" << std::endl;
    std::cout << "expected " << expected << " profiles" << std::endl;

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

  // Free memory allocated for threads if not already done.
  if (nullptr != threads) {
    for (unsigned long i = 0; i < scan_heads.size(); i++) {
      threads[i].join();
    }
    delete[] threads;
  }

  if (nullptr != scan_system) {
    jsScanSystemFree(scan_system);
  }

  return (0 == r) ? 0 : 1;
}
