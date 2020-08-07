/**
 * Copyright (c) JoeScan Inc. All Rights Reserved.
 *
 * Licensed under the BSD 3 Clause License. See LICENSE.txt in the project
 * root for license information.
 */

#ifndef JSCAN_SERVER_BROADCASTCONNECT_MESSAGE_H
#define JSCAN_SERVER_BROADCASTCONNECT_MESSAGE_H

#include <vector>
#include "Enums.hpp"
#include "NetworkTypes.hpp"

namespace joescan {
class BroadcastConnectMessage {
 public:
  BroadcastConnectMessage(uint32_t ip, uint16_t port, uint8_t session_id,
                          uint8_t scan_head_id, uint32_t serial_number,
                          ConnectionType conn_type = ConnectionType::Normal);
  BroadcastConnectMessage();
  ~BroadcastConnectMessage() = default;

  static BroadcastConnectMessage Deserialize(std::vector<uint8_t> &data);
  std::vector<uint8_t> Serialize() const;

  ConnectionType GetConnectionType()
  {
    return packet.conn_type;
  }
  uint32_t GetSerialNumber()
  {
    return packet.serial_number;
  }

 private:
#pragma pack(push, 1)
  struct BroadcastConnectMessagePacket {
    InfoHeader header;
    uint32_t ip = 0;
    uint16_t port = 0;
    uint8_t session_id = 0;
    uint8_t scan_head_id = 0;
    uint32_t serial_number = 0;
    ConnectionType conn_type = ConnectionType::Normal;
  };
#pragma pack(pop)

  static const int kBroadcastConnectMessageSize =
    sizeof(BroadcastConnectMessagePacket);

  BroadcastConnectMessagePacket packet;

  static void ValidateHeader(const InfoHeader &hdr);
};
} // namespace joescan

#endif // JSCAN_SERVER_BROADCASTCONNECT_MESSAGE_H
