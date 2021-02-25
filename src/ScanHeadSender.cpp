/**
 * Copyright (c) JoeScan Inc. All Rights Reserved.
 *
 * Licensed under the BSD 3 Clause License. See LICENSE.txt in the project
 * root for license information.
 */

#include "ScanHeadSender.hpp"
#include "NetworkInterface.hpp"

#include <chrono>
#include <cstring>
#include <sstream>

using namespace joescan;

using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::chrono::steady_clock;

ScanHeadSender::ScanHeadSender()
{
  is_running = true;
  is_scanning = false;

  {
    net_iface iface = NetworkInterface::InitSendSocket(INADDR_ANY, 0);
    sockfd = iface.sockfd;
  }

  std::thread send_thread(&ScanHeadSender::SendMain, this);
  thread_sender = std::move(send_thread);
  std::thread scan_thread(&ScanHeadSender::TimerMain, this);
  thread_scan_timer = std::move(scan_thread);
}

ScanHeadSender::~ScanHeadSender()
{
  if (is_running) {
    // It's probably too late at this point, but....
    Shutdown();
  }
}

void ScanHeadSender::Send(Datagram datagram, uint32_t ip_address)
{
  ScanHeadSendMessage msg(ip_address, datagram);

  {
    std::unique_lock<std::mutex> lock(mutex_send);
    send_message.push(msg);
    condition_send.notify_all();
  }
}

void ScanHeadSender::EnqueueScanRequests(
  std::vector<std::pair<uint32_t, Datagram>> requests)
{
  std::lock_guard<std::mutex> lock(scan_request_mutex);
  scan_request_packets.clear();

  for (auto &request : requests) {
    scan_request_packets.push_back(request);
  }
}

void ScanHeadSender::ClearScanRequests()
{
  std::lock_guard<std::mutex> lock(scan_request_mutex);
  scan_request_packets.clear();
}

void ScanHeadSender::Start()
{
  is_scanning = true;
}

void ScanHeadSender::Stop()
{
  is_scanning = false;
}

void ScanHeadSender::Shutdown()
{
  is_running = false;
  is_scanning = false;

  {
    std::unique_lock<std::mutex> lock(mutex_send);
    condition_send.notify_all();
  }

  NetworkInterface::CloseSocket(sockfd);
  thread_sender.join();
  thread_scan_timer.join();
}

void ScanHeadSender::SendMain()
{
  while (is_running) {
    try {
      std::unique_lock<std::mutex> lock(mutex_send);
      if (0 == send_message.size()) {
        condition_send.wait(lock);
      } else {
        ScanHeadSendMessage msg = send_message.front();
        send_message.pop();
        uint32_t ip_addr = msg.dst_addr;
        std::shared_ptr<Datagram> datagram = msg.data;

        if (ip_addr != 0) {
          sockaddr_in scanner_addr;
          memset(&scanner_addr, 0, sizeof(scanner_addr));
          scanner_addr.sin_family = AF_INET;
          scanner_addr.sin_addr.s_addr = htonl(ip_addr);
          scanner_addr.sin_port = htons(kScanServerPort);

          const char *data = reinterpret_cast<const char *>(datagram->data());
          const uint32_t len = static_cast<uint32_t>(datagram->size());
          int r = sendto(sockfd, data, len, 0,
                         reinterpret_cast<sockaddr *>(&scanner_addr),
                         sizeof(scanner_addr));

          if (0 >= r) {
            std::stringstream error_msg;
            error_msg << "Failed sendto IP address " << std::hex << ip_addr;
            throw std::runtime_error(error_msg.str());
          }

          // HACK: slight delay needed to make sure Windows doesn't drop
          // UDP packets?
          std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
      }
    }
    // TODO: Better handling of socket closure & so forth..
    catch (std::runtime_error &error) {
      // TODO: Log error?
      (void)error;
      break;
    }
  }
}

void ScanHeadSender::TimerMain()
{
  auto last_send = steady_clock::now();

  while (is_running) {
    if (is_scanning) {
      // extra scope so the lock is automatically released before sleep_for
      {
        std::unique_lock<std::mutex> lock(scan_request_mutex);
        if (!scan_request_packets.empty()) {
          auto elapsed = steady_clock::now() - last_send;
          if (elapsed >= milliseconds(scan_request_interval_ms)) {
            for (auto &request : scan_request_packets) {
              Send(request.second, request.first);
            }
            last_send = steady_clock::now();
          }
        }
      }
    }

    std::this_thread::sleep_for(milliseconds(100));
  }
}
