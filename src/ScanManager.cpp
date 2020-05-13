/**
 * Copyright (c) JoeScan Inc. All Rights Reserved.
 *
 * Licensed under the BSD 3 Clause License. See LICENSE.txt in the project
 * root for license information.
 */

#include "ScanManager.hpp"
#include "ScanHead.hpp"

#include "BroadcastConnectMessage.hpp"
#include "DisconnectMessage.hpp"
#include "NetworkInterface.hpp"
#include "NetworkingTypes.hpp"
#include "Profile.hpp"
#include "ScanRequestMessage.hpp"
#include "SetWindowMessage.hpp"
#include "StatusMessage.hpp"
#include "VersionCompatibilityException.hpp"
#include "VersionParser.hpp"

#include <cmath>
#include <cstring>
#include <ctime>
#include <sstream>

using namespace joescan;

ScanManager::ScanManager()
{
  session_id = 1;
}

ScanManager::~ScanManager()
{
  for (auto const &pair : scanners_by_serial) {
    std::string serial = pair.first;
    ScanHead *scanner = pair.second;
    ScanHeadReceiver *receiver = receivers_by_serial[serial];
    ScanHeadShared *shared = shares_by_serial[serial];

    // stop the receiver thread from grabbing more UDP data
    receiver->Shutdown();

    // delete objects in opposite order that they were created
    delete receiver;
    delete scanner;
    delete shared;
  }

  sender.Shutdown();
  RemoveAllScanners();
}

ScanHead *ScanManager::CreateScanner(std::string serial_number, uint32_t id)
{
  ScanHead *scanner = nullptr;

  if (IsScanning()) {
    std::string error_msg = "Can not add scanner while scanning.";
    throw std::runtime_error(error_msg);
  }

  if (scanners_by_serial.find(serial_number) != scanners_by_serial.end()) {
    std::string error_msg = "Scanner " + serial_number + " is already managed.";
    throw std::runtime_error(error_msg);
  }

  if (scanners_by_id.find(id) != scanners_by_id.end()) {
    std::string error_msg = "Id is already assigned to another scanner.";
    throw std::runtime_error(error_msg);
  }

  ScanHeadShared *shared = new ScanHeadShared(serial_number, id);
  shares_by_serial[serial_number] = shared;

  ScanHeadReceiver *receiver = new ScanHeadReceiver(*shared);
  receivers_by_serial[serial_number] = receiver;

  scanner = new ScanHead(*this, *shared);
  scanners_by_serial[serial_number] = scanner;
  scanners_by_id[id] = scanner;

  return scanner;
}

ScanHead *ScanManager::GetScanner(std::string serial_number)
{
  auto scanner = scanners_by_serial.find(serial_number);

  if (scanner == scanners_by_serial.end()) {
    std::string error_msg = "Scanner is not managed.";
    throw std::runtime_error(error_msg);
  }

  return scanner->second;
}

ScanHead *ScanManager::GetScanner(uint32_t id)
{
  auto scanner = scanners_by_id.find(id);
  if (scanner == scanners_by_id.end()) {
    std::string error_msg = "Scanner is not managed.";
    throw std::runtime_error(error_msg);
  }

  return scanner->second;
}

void ScanManager::RemoveScanner(std::string serial_number)
{
  if (IsScanning()) {
    std::string error_msg = "Can not remove scanner while scanning";
    throw std::runtime_error(error_msg);
  }

  auto scanner = scanners_by_serial.find(serial_number);
  if (scanner != scanners_by_serial.end()) {
    uint32_t id = scanner->second->GetId();
    scanners_by_serial.erase(serial_number);
    if (scanners_by_id.find(id) != scanners_by_id.end()) {
      scanners_by_id.erase(id);
    } else {
      std::string error_msg = "Scanner ID was not found.";
      throw std::runtime_error(error_msg);
    }
  } else {
    std::string error_msg = "Scanner is not managed.";
    throw std::runtime_error(error_msg);
  }
}

void ScanManager::RemoveScanner(ScanHead *scanner)
{
  if (scanner != nullptr) {
    RemoveScanner(scanner->GetSerialNumber());
  } else {
    throw std::runtime_error("Null pointer passed to RemoveScanner");
  }
}

void ScanManager::RemoveAllScanners()
{
  if (IsScanning()) {
    std::string error_msg = "Can not remove scanners while scanning";
    throw std::runtime_error(error_msg);
  }

  scanners_by_serial.clear();
  scanners_by_id.clear();
}

uint32_t ScanManager::GetNumberScanners()
{
  return scanners_by_serial.size();
}

std::map<std::string, ScanHead *> ScanManager::Connect(uint32_t timeout_s)
{
  if (IsConnected()) {
    std::string error_msg = "Already connected.";
    throw std::runtime_error(error_msg);
  }

  if (IsScanning()) {
    std::string error_msg = "Already scanning.";
    throw std::runtime_error(error_msg);
  }

  std::map<std::string, ScanHead *> connected;
  if (scanners_by_serial.empty()) {
    return connected;
  }

  for (auto const &pair : scanners_by_serial) {
    std::string serial = pair.first;
    ScanHead *scan_head = pair.second;
    ScanHeadReceiver *receiver = receivers_by_serial[serial];

    if (!scan_head->ValidateConfig()) {
      std::string error_msg = "Configuration validation failed for scanner " +
                              scan_head->GetSerialNumber();

      throw std::runtime_error(error_msg);
    }

    receiver->Start();
  }

  // TODO: The idea is to ignore packets from different session id, eventually.
  // for all receivers_by_serial call SetSessionId(session_id);
  session_id++;
  connected = BroadcastConnect(timeout_s);
  if (connected.size() == scanners_by_serial.size()) {
    state = SystemState::Connected;
  }

  if (SystemState::Connected == state) {
    sender.Start();

    for (auto const &pair : scanners_by_serial) {
      ScanHead *scan_head = pair.second;
      uint32_t ip_addr = scan_head->GetIpAddress();
      std::vector<WindowConstraint> constraints =
        scan_head->GetConfig().ScanWindow().Constraints();

      // camera 0 window constraint configuration
      auto msg0 = SetWindowMessage(0);
      for (auto const &constraint : constraints) {
        Point2D<int32_t> p0, p1;
        int32_t x, y;
        // note, units are in 1/1000 inch
        // calculate the first point of our window constraint
        x = static_cast<int32_t>(constraint.constraints[0].x);
        y = static_cast<int32_t>(constraint.constraints[0].y);
        // convert the point to the camera's coordinate system
        p0 = scan_head->GetConfig().Alignment(0).MillToCamera(x, y);
        // calculate the second point of out window constraint
        x = static_cast<int32_t>(constraint.constraints[1].x);
        y = static_cast<int32_t>(constraint.constraints[1].y);
        // convert the point to the camera's coordinate system
        p1 = scan_head->GetConfig().Alignment(0).MillToCamera(x, y);
        // pass constraint points to message to create the constraint
        if (scan_head->GetConfig().Alignment(0).GetFlipX()) {
          msg0.AddConstraint(p1.x, p1.y, p0.x, p0.y);
        } else {
          msg0.AddConstraint(p0.x, p0.y, p1.x, p1.y);
        }
      }
      // send the constraint message to the scan server
      sender.Send(msg0.Serialize(), ip_addr);

      // camera 1 window constraint configuration
      auto msg1 = SetWindowMessage(1);
      for (auto const &constraint : constraints) {
        Point2D<int32_t> p0, p1;
        int32_t x, y;
        // note, units are in 1/1000 inch
        // calculate the first point of our window constraint
        x = static_cast<int32_t>(constraint.constraints[0].x);
        y = static_cast<int32_t>(constraint.constraints[0].y);
        // convert the point to the camera's coordinate system
        p0 = scan_head->GetConfig().Alignment(1).MillToCamera(x, y);
        // calculate the second point of out window constraint
        x = static_cast<int32_t>(constraint.constraints[1].x);
        y = static_cast<int32_t>(constraint.constraints[1].y);
        // convert the point to the camera's coordinate system
        p1 = scan_head->GetConfig().Alignment(1).MillToCamera(x, y);
        // pass constraint points to message to create the constraint
        if (scan_head->GetConfig().Alignment(1).GetFlipX()) {
          msg1.AddConstraint(p1.x, p1.y, p0.x, p0.y);
        } else {
          msg1.AddConstraint(p0.x, p0.y, p1.x, p1.y);
        }
      }
      // send the constraint message to the scan server
      sender.Send(msg1.Serialize(), ip_addr);
    }

    // allow enough time for the scan heads to fully configure
    std::this_thread::sleep_for(std::chrono::seconds(1));

    for (auto const &pair : scanners_by_serial) {
      ScanHead *scan_head = pair.second;
      double rate_hz = scan_head->GetStatusMessage().GetMaxScanRate();
      if (rate_hz < scan_rate_hz_current_max) {
        scan_rate_hz_current_max = rate_hz;
      }
    }
  }

  return connected;
}

void ScanManager::Disconnect()
{
  if (!IsConnected()) {
    std::string error_msg = "Not connected.";
    throw std::runtime_error(error_msg);
  }

  if (IsScanning()) {
    std::string error_msg = "Can not disconnect wile still scanning";
    throw std::runtime_error(error_msg);
  }

  auto message = DisconnectMessage().Serialize();
  for (auto const &pair : scanners_by_serial) {
    std::string serial = pair.first;
    ScanHead *scan_head = pair.second;
    ScanHeadReceiver *receiver = receivers_by_serial[serial];

    sender.Send(message, scan_head->GetIpAddress());
    receiver->Stop();
  }
  sender.Stop();

  // TODO: Do we need to clear out the status message for the scan heads?
  // Maybe just something to make sure the Scan Heads don't still report being
  // connected.

  state = SystemState::Disconnected;
}

void ScanManager::StartScanning()
{
  double scan_interval_us = (1.0 / scan_rate_hz) * 1e6;

  if (!IsConnected()) {
    std::string error_msg = "Not connected.";
    throw std::runtime_error(error_msg);
  }

  if (IsScanning()) {
    std::string error_msg = "Already scanning.";
    throw std::runtime_error(error_msg);
  }

  // Just create & enqueue scan request messages in the SenderReceiver.
  std::vector<std::pair<uint32_t, Datagram>> requests;
  requests.reserve(scanners_by_serial.size());

  for (auto const &pair : scanners_by_serial) {
    const uint32_t interval = static_cast<uint32_t>(scan_interval_us);
    std::string serial = pair.first;
    ScanHead *scan_head = pair.second;
    ScanHeadReceiver *receiver = receivers_by_serial[serial];

    scan_head->Flush();
    receiver->Start();

    ScanRequest request(scan_head->GetDataFormat(), 0, receiver->GetPort(),
                        scan_head->GetId(),
                        static_cast<uint32_t>(ceil(interval)),
                        0xFFFFFFFF, // uint32_t max
                        scan_head->GetConfig());

    auto ip_addr_and_request =
      std::make_pair(scan_head->GetIpAddress(), request.Serialize(session_id));

    requests.push_back(ip_addr_and_request);
  }

  sender.EnqueueScanRequests(requests);

  state = SystemState::Scanning;
}

void ScanManager::StartScanning(ScanHead *scan_head)
{
  double scan_interval_us = (1.0 / scan_rate_hz) * 1e6;

  if (!IsConnected()) {
    std::string error_msg = "Not connected.";
    throw std::runtime_error(error_msg);
  }

  if (IsScanning()) {
    std::string error_msg = "Already scanning.";
    throw std::runtime_error(error_msg);
  }

  auto pair = scanners_by_id.find(scan_head->GetId());
  if (pair == scanners_by_id.end()) {
    std::string error_msg = "Scanner is not managed.";
    throw std::runtime_error(error_msg);
  }

  ScanHeadReceiver *receiver =
    receivers_by_serial[scan_head->GetSerialNumber()];
  std::vector<std::pair<uint32_t, Datagram>> requests;
  requests.reserve(1);

  scan_head->Flush();
  receiver->Start();

  const uint32_t interval = static_cast<uint32_t>(scan_interval_us);
  ScanRequest request(scan_head->GetDataFormat(), 0, receiver->GetPort(),
                      scan_head->GetId(), interval,
                      0xFFFFFFFF, // uint32_t max
                      scan_head->GetConfig());

  requests.push_back(
    std::make_pair(scan_head->GetIpAddress(), request.Serialize(session_id)));

  sender.EnqueueScanRequests(requests);

  state = SystemState::Scanning;
}

void ScanManager::StopScanning()
{
  if (!IsScanning()) {
    std::string error_msg = "Not scanning.";
    throw std::runtime_error(error_msg);
  }

  sender.ClearScanRequests();

  state = SystemState::Connected;
}

void ScanManager::SetScanRate(double rate_hz)
{
  if ((rate_hz > kScanRateHzMax) || (rate_hz < kScanRateHzMin)) {
    std::stringstream error_msg;

    error_msg << "scan rate " << rate_hz << " out of range, must be between "
              << kScanRateHzMin << " Hz and " << kScanRateHzMax << "Hz";

    throw std::runtime_error(error_msg.str());
  } else if (rate_hz > scan_rate_hz_current_max) {
    std::stringstream error_msg;

    error_msg << "scan rate " << rate_hz
              << " exceeds max scan rate allowed by window, must be less than "
              << scan_rate_hz_current_max << "Hz";

    throw std::runtime_error(error_msg.str());
  }

  scan_rate_hz = rate_hz;
}

double ScanManager::GetScanRate() const
{
  return scan_rate_hz;
}

void ScanManager::SetRequestedDataFormat(jsDataFormat format)
{
  for (auto const &m : scanners_by_id) {
    m.second->SetDataFormat(format);
  }
}

void ScanManager::FillVersionInformation(VersionInformation &vi)
{
  vi.major = std::stoi(VERSION_MAJOR);
  vi.minor = std::stoi(VERSION_MINOR);
  vi.patch = std::stoi(VERSION_PATCH);
  vi.commit = std::stoul(VERSION_COMMIT, nullptr, 16);
}

std::map<std::string, ScanHead *> ScanManager::BroadcastConnect(
  uint32_t timeout_s)
{
  std::map<std::string, ScanHead *> connected;
  std::vector<net_iface> ifaces;

  /////////////////////////////////////////////////////////////////////////////
  // STEP 1: Get all available interfaces.
  /////////////////////////////////////////////////////////////////////////////
  {
    auto ip_addrs = NetworkInterface::GetActiveIpAddresses();
    for (auto const &ip_addr : ip_addrs) {
      try {
        net_iface iface = NetworkInterface::InitBroadcastSocket(ip_addr, 0);
        ifaces.push_back(iface);
      } catch (const std::runtime_error &) {
        // Failed to init socket, continue since there might be other ifaces
      }
    }

    if (ifaces.size() == 0) {
      throw std::runtime_error("No valid broadcast interfaces");
    }
  }

  /////////////////////////////////////////////////////////////////////////////
  // STEP 2: Send out BroadcastConnect packet for each scan head.
  /////////////////////////////////////////////////////////////////////////////
  {
    // spam each network interface with our connection message
    for (auto const &iface : ifaces) {
      for (auto const &pair : scanners_by_serial) {
        std::string serial = pair.first;
        ScanHead *scan_head = pair.second;
        uint32_t scan_id = scan_head->GetId();
        uint32_t ip_addr = iface.ip_addr;
        uint16_t port = receivers_by_serial[serial]->GetPort();

        // we want the scan head to connect to the client with these params
        auto bytes = BroadcastConnectMessage(ip_addr, port, session_id, scan_id,
                                             std::stoul(serial))
                       .Serialize();

        int sock = iface.sockfd;
        const char *src = reinterpret_cast<const char *>(bytes.data());
        const unsigned int len = bytes.size();

        // client will send payload out according to these values
        sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
        addr.sin_port = htons(kScanServerPort);

        int r = sendto(sock, src, len, 0, reinterpret_cast<sockaddr *>(&addr),
                       sizeof(addr));
        if (0 >= r) {
          // failed to send data to interface
          break;
        }
      }
    }
  }
  /////////////////////////////////////////////////////////////////////////////
  // STEP 3: See which (if any) scan heads responded.
  /////////////////////////////////////////////////////////////////////////////
  {
    static const int kConnectPollMs = 100;
    uint64_t time_start = std::time(nullptr);
    int32_t timeout_ms = timeout_s * 1000;
    bool is_connected = false;
    while ((false == is_connected) && (0 < timeout_ms)) {
      for (auto const &pair : scanners_by_serial) {
        std::string serial = pair.first;
        ScanHead *scan_head = pair.second;
        StatusMessage msg = scan_head->GetStatusMessage();
        uint64_t timestamp = 0;

        // get timestamp where status message was received
        timestamp = scan_head->GetScanHeadShared().GetStatusMessageTimestamp();

        if ((connected.end() == connected.find(serial)) &&
            (timestamp > time_start)) {
          VersionInformation client_version;
          FillVersionInformation(client_version);

          auto scanner_version = msg.GetVersionInformation();
          if (!VersionParser::AreVersionsCompatible(client_version,
                                                    scanner_version)) {
            throw VersionCompatibilityException(client_version,
                                                scanner_version);
          }

          // found an active scan head!
          scan_head->SetIpAddress(msg.GetScanHeadIp());
          connected[serial] = scan_head;
        }
      }

      if (connected.size() != scanners_by_serial.size()) {
        // still waiting for status messages...
        std::this_thread::sleep_for(std::chrono::milliseconds(kConnectPollMs));
        timeout_ms -= kConnectPollMs;
      } else {
        is_connected = true;
      }
    }
  }

  /////////////////////////////////////////////////////////////////////////////
  // STEP 4: Clean up and return.
  /////////////////////////////////////////////////////////////////////////////
  for (auto const &iface : ifaces) {
    NetworkInterface::CloseSocket(iface.sockfd);
  }

  return connected;
}
