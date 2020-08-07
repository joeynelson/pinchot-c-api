/**
 * Copyright (c) JoeScan Inc. All Rights Reserved.
 *
 * Licensed under the BSD 3 Clause License. See LICENSE.txt in the project
 * root for license information.
 */

#ifndef JOESCAN_SCAN_HEAD_RECEIVER_H
#define JOESCAN_SCAN_HEAD_RECEIVER_H

#include "DataPacket.hpp"
#include "Profile.hpp"
#include "ScanHeadShared.hpp"
#include "NetworkIncludes.hpp"
#include "NetworkInterface.hpp"
#include "StatusMessage.hpp"

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <string>
#include <thread>

namespace joescan {
class ScanManager;

class ScanHeadReceiver {
 public:
  ScanHeadReceiver(ScanHeadShared &shared);
  ~ScanHeadReceiver();

  ScanHeadShared &GetScanHeadShared();
  int GetPort() const;
  std::string GetSerial() const;
  std::shared_ptr<Profile> GetProfile();

  void Start();
  void Stop();
  // This should gracefully bring down the threads, if the caller can do this
  // prior to its dtor
  void Shutdown();

 private:
  enum ScanHeadReceiverState {
    RECEIVER_STOP,
    RECEIVER_START,
    RECEIVER_SHUTDOWN,
  };

  void ReceiveMain();
  void ProcessPacket(DataPacket &packet);

  // The JS-50 theoretical max packet size is 8k plus header, in reality the
  // max size is 1456 * 4 + header. Using 6k.
  static const int kMaxPacketSize = 6144;
  // JS-50 in image mode will have 4 rows of 1456 pixels for each packet.
  static const int kImageDataSize = 4 * 1456;

  std::condition_variable sync;
  std::mutex lock;
  std::thread receiver;

  std::shared_ptr<Profile> profile_ptr;

  ScanHeadShared &shared;
  std::atomic<enum ScanHeadReceiverState> state;
  SOCKET sockfd;
  int sockport;
  uint8_t *packet_buf;
  uint32_t packet_buf_len;
  uint64_t packets_received;
  uint32_t packets_received_for_profile;
  uint64_t complete_profiles_received;
  uint64_t expected_packets_received;
  uint64_t expected_profiles_received;
  uint32_t last_profile_source;
  uint64_t last_profile_timestamp;
};
} // namespace joescan

#endif
