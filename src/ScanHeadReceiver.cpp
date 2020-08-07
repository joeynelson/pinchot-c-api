/**
 * Copyright (c) JoeScan Inc. All Rights Reserved.
 *
 * Licensed under the BSD 3 Clause License. See LICENSE.txt in the project
 * root for license information.
 */

#include <ctime>
#include <memory>
#include <sstream>

#include "ScanHeadReceiver.hpp"
#include "joescan_pinchot.h"

using namespace joescan;

ScanHeadReceiver::ScanHeadReceiver(ScanHeadShared &shared) : shared(shared)
{
  packet_buf = new uint8_t[kMaxPacketSize];
  packet_buf_len = kMaxPacketSize;
  profile_ptr = nullptr;
  state = RECEIVER_STOP;

  {
    net_iface iface = NetworkInterface::InitRecvSocket(INADDR_ANY, 0);
    sockfd = iface.sockfd;
    sockport = iface.port;
  }

  std::thread receive_thread(&ScanHeadReceiver::ReceiveMain, this);
  receiver = std::move(receive_thread);
}

ScanHeadReceiver::~ScanHeadReceiver()
{
  if (RECEIVER_SHUTDOWN != state) {
    // It's probably too late at this point, but....
    Shutdown();
  }

  delete[] packet_buf;
}

ScanHeadShared &ScanHeadReceiver::GetScanHeadShared()
{
  return shared;
}

int ScanHeadReceiver::GetPort() const
{
  return sockport;
}

std::string ScanHeadReceiver::GetSerial() const
{
  return shared.GetSerial();
}

void ScanHeadReceiver::Start()
{
  {
    std::lock_guard<std::mutex> lk(lock);
    profile_ptr = nullptr;
    packets_received = 0;
    complete_profiles_received = 0;
    last_profile_source = 0;
    last_profile_timestamp = 0;
    state = RECEIVER_START;
    shared.EnableWaitUntilAvailable();
  }

  sync.notify_all();
}

void ScanHeadReceiver::Stop()
{
  {
    std::lock_guard<std::mutex> lk(lock);
    state = RECEIVER_STOP;
  }

  sync.notify_all();
}

void ScanHeadReceiver::Shutdown()
{
  {
    std::lock_guard<std::mutex> lk(lock);
    state = RECEIVER_SHUTDOWN;
  }

  NetworkInterface::CloseSocket(sockfd);

  sync.notify_all();
  receiver.join();
}

void ScanHeadReceiver::ReceiveMain()
{
  int nfds;
  fd_set rfds;
  struct timeval tv;
  int ret;

  while (RECEIVER_SHUTDOWN != state) {
    if (RECEIVER_STOP == state) {
      shared.DisableWaitUntilAvailable();
      std::unique_lock<std::mutex> lck(lock);
      sync.wait(lck);
    } else if (RECEIVER_START == state) {
      nfds = static_cast<int>(sockfd) + 1;
      FD_ZERO(&rfds);
      FD_SET(sockfd, &rfds);
      tv.tv_sec = 1;
      tv.tv_usec = 0;

      // Poll for activity on on the file descriptor, timeout if no activity.
      ret = select(nfds, &rfds, NULL, NULL, &tv);
      if (0 < ret) {
        // Activity indicated, read out data from socket.
        int num_bytes =
          recv(sockfd, reinterpret_cast<char *>(packet_buf), packet_buf_len, 0);

        // Check to make sure we are still running in case recv returns due to
        // its socket fd being closed.
        if (RECEIVER_START == state) {
          std::lock_guard<std::mutex> lk(lock);

          if (static_cast<std::size_t>(num_bytes) < sizeof(DatagramHeader)) {
            throw std::runtime_error("Short header");
          }

          uint16_t magic = (packet_buf[0] << 8) | (packet_buf[1]);
          if (kDataMagic == magic) {
            packets_received++;

            DataPacket packet(packet_buf, num_bytes, 0);
            ProcessPacket(packet);
          } else if (kResponseMagic == magic) {
            StatusMessage status_message = StatusMessage(packet_buf, num_bytes);
            expected_packets_received = status_message.GetNumPacketsSent();
            expected_profiles_received = status_message.GetNumProfilesSent();
            shared.SetStatusMessage(status_message);
          } else {
            throw std::runtime_error("Unknown magic");
          }
        }
      }
    }
  }
}

void ScanHeadReceiver::ProcessPacket(DataPacket &packet)
{
  uint32_t source = 0;
  uint64_t timestamp = 0;
  uint32_t raw_bytes_len = 0;
  uint8_t *raw_bytes = packet.GetRawBytes(&raw_bytes_len);
  uint32_t total_packets = packet.GetNumParts();
  uint32_t current_packet = packet.GetPartNum();
  DataType datatype_mask = packet.GetContents();

  source = packet.GetSourceId();
  timestamp = packet.GetTimeStamp();

  if ((source != last_profile_source) ||
      (timestamp != last_profile_timestamp)) {
    if (nullptr != profile_ptr) {
      // have a partial profile, push it back despite loss
      profile_ptr->SetUDPPacketInfo(packets_received_for_profile,
                                    total_packets);

      shared.PushProfile(profile_ptr);
    }

    last_profile_source = source;
    last_profile_timestamp = timestamp;
    packets_received_for_profile = 0;

    profile_ptr = std::make_shared<Profile>(datatype_mask);
    profile_ptr->SetScanHead(packet.GetScanHeadId());
    profile_ptr->SetCamera(packet.GetCamera());
    profile_ptr->SetLaser(packet.GetLaser());
    profile_ptr->SetTimestamp(packet.GetTimeStamp());
    profile_ptr->SetLaserOnTime(packet.GetLaserOnTime());
    profile_ptr->SetExposureTime(packet.GetExposureTime());
    if (0 != packet.NumEncoderVals()) {
      profile_ptr->SetEncoderValues(packet.GetEncoderValues());
    }
  }

  if (datatype_mask & DataType::Brightness) {
    FragmentLayout layout = packet.GetFragmentLayout(DataType::Brightness);
    const uint32_t start_column = packet.GetStartColumn();
    uint32_t idx = 0;

    for (unsigned int j = 0; j < layout.num_vals; j++) {
      idx = (j * total_packets + current_packet) * layout.step + start_column;

      uint8_t brightness = raw_bytes[layout.offset + j];
      if (JS_PROFILE_DATA_INVALID_BRIGHTNESS != brightness) {
        profile_ptr->InsertBrightness(idx, brightness);
      }
    }
  }

  if (datatype_mask & DataType::XYData) {
    FragmentLayout layout = packet.GetFragmentLayout(DataType::XYData);
    uint32_t m = 0;
    uint32_t n = layout.offset;
    int16_t x_raw = 0;
    int16_t y_raw = 0;
    int camera_id = packet.GetCamera();
    AlignmentParams alignment = shared.GetConfiguration().Alignment(camera_id);

    for (unsigned int j = 0; j < layout.num_vals; j++) {
      x_raw = htons(*(reinterpret_cast<int16_t *>(&(raw_bytes[n]))));
      n += sizeof(int16_t);
      y_raw = htons(*(reinterpret_cast<int16_t *>(&(raw_bytes[n]))));
      n += sizeof(int16_t);

      if ((JS_PROFILE_DATA_INVALID_XY != x_raw) &&
          (JS_PROFILE_DATA_INVALID_XY != y_raw)) {
        int32_t x = static_cast<int32_t>(x_raw);
        int32_t y = static_cast<int32_t>(y_raw);
        Point2D<int32_t> point = alignment.CameraToMill(x, y);

        // destination
        m = packet.GetStartColumn();
        m += (j * total_packets + current_packet) * layout.step;
        profile_ptr->InsertPoint(m, point);
      }
    }
  }

#if 0
  // TODO: FKS-252
  // Support in the future. How do we expose Subpixel data? Don't think we want
  // normal customers to use it, just internal/special customers.
  if (datatype_mask & DataType::Subpixel) {
    FragmentLayout layout = packet.GetFragmentLayout(DataType::Subpixel);
    uint32_t m = 0;
    uint32_t n = layout.offset;
    uint16_t pixel = 0;

    for (unsigned int j = 0; j < layout.num_vals; j++) {
      pixel = htons(*(reinterpret_cast<uint16_t *>(&(raw_bytes[n]))));
      n += sizeof(uint16_t);

      if (kInvalidSubpixel != pixel) {
        m = packet.GetStartColumn();
        m += (j * total_packets + current_packet) * layout.step;

        Point2D point(pixel, m);
        profile_ptr->InsertPixelCoordinate(m, point);
      }
    }
  }
#endif

  if (datatype_mask & DataType::Image) {
    // skip subpixel packet
    if ((packets_received_for_profile + 1) != total_packets) {
      FragmentLayout layout = packet.GetFragmentLayout(DataType::Image);
      uint32_t len = kImageDataSize;
      uint32_t m = current_packet * len;
      uint32_t n = layout.offset;
      // HACK HACK HACK: need to account for the fact that the scan_server sends
      // back exposure value right shifted by 8 for image mode
      profile_ptr->SetExposureTime(packet.GetExposureTime() << 8);
      profile_ptr->InsertImageSlice(m, &(raw_bytes[n]), len);
    }
  }

  packets_received_for_profile++;
  if (packets_received_for_profile == total_packets) {
    // received all packets for the profile
    profile_ptr->SetUDPPacketInfo(total_packets, total_packets);
    shared.PushProfile(profile_ptr);
    profile_ptr = nullptr;
    complete_profiles_received++;
  }
}
