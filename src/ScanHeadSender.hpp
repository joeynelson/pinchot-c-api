/**
 * Copyright (c) JoeScan Inc. All Rights Reserved.
 *
 * Licensed under the BSD 3 Clause License. See LICENSE.txt in the project
 * root for license information.
 */

#ifndef JOESCAN_SCAN_HEAD_SENDER_H
#define JOESCAN_SCAN_HEAD_SENDER_H

#include "DataPacket.hpp"
#include "Profile.hpp"
#include "NetworkIncludes.hpp"
#include "StatusMessage.hpp"

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>

namespace joescan {
class ScanHeadSender {
 public:
  ScanHeadSender();
  ~ScanHeadSender();

  void Send(Datagram datagram, uint32_t ip_address);
  void EnqueueScanRequests(std::vector<std::pair<uint32_t, Datagram>> requests);
  void ClearScanRequests();

  void Start();
  void Stop();
  void Shutdown();

 private:
  /**
   * @brief A small helper class used to queue up datagrams to send to
   * specific scan heads.
   */
  struct ScanHeadSendMessage {
    /** @brief The destination IP address of the data. */
    uint32_t dst_addr;
    /** @brief Pointer to byte data to send as UDP datagram. */
    std::shared_ptr<Datagram> data;

    /**
     * @brief Initializes a new `SendMessage` struct
     *
     * @param addr IP address to send datagram to.
     * @param datagram The byte data to send as UDP datagram.
     */
    ScanHeadSendMessage(uint32_t addr, Datagram datagram)
    {
      dst_addr = addr;
      data = std::make_shared<Datagram>(datagram);
    }
  };

  void SendMain();
  void TimerMain();

  int scan_request_interval_ms = 500;

  std::vector<std::pair<uint32_t, Datagram>> scan_request_packets;
  std::mutex scan_request_mutex;
  std::thread thread_sender;
  std::thread thread_scan_timer;

  /** @brief Queue of UDP messages to send to scan heads. */
  std::queue<ScanHeadSendMessage> send_message;
  /** @brief Condition variable used to signal new messages. */
  std::condition_variable condition_send;
  /** @brief Provides access lock to `send_message` queue. */
  std::mutex mutex_send;

  SOCKET sockfd;
  std::atomic<bool> is_running;
  std::atomic<bool> is_scanning;
};
} // namespace joescan

#endif
